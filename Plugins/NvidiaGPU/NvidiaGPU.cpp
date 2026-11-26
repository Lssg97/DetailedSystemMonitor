// NvidiaGPU.cpp

#include <Windows.h>                                                                // Windows 基础 API 头文件（线程、事件、字符转换等）
#include <stdio.h>                                                                  // C 运行时标准 I/O 头文件，提供 swprintf_s 等安全格式化函数
#include <atomic>                                                                   // C++ 原子操作头文件，用于保证跨线程标志位的读写安全
#include "../API/Rainmeter/RainmeterAPI.h"                                          // Rainmeter 插件接口头文件（RmReadInt、RmLog、PLUGIN_EXPORT 等）
#include "../API/NVIDIA/nvapi.h"                                                    // NVIDIA NVAPI 头文件（NvAPI_* 函数与数据结构）

constexpr DWORD N_MAX_GPUS = NVAPI_MAX_PHYSICAL_GPUS;                               // 最大物理 GPU 处理数量，与 NVAPI 枚举容量保持一致，避免枚举时数组越界
constexpr DWORD GPU_USAGE_UNAVAILABLE = 0;                                          // 使用率不可用时的返回值，0 在百分比语义下表示“无数据”，避免与满载 100% 混淆

static std::atomic<bool> thread{ false };                                           // 后台线程运行标志，原子变量保证主线程写入与工作线程读取之间没有数据竞争
static HANDLE hEvent = NULL;                                                        // 后台线程唤醒事件句柄，用于在“有刷新请求”时唤醒工作线程
static HANDLE hThread = NULL;                                                       // 后台线程句柄，用于在卸载时等待线程真正退出
static std::atomic<uint8_t> bUpdate[N_MAX_GPUS] = {};                               // 每个 GPU 的待刷新标志位，原子类型保证 |= 和 &= 操作不会丢失更新

static DWORD WINAPI ThreadProc(LPVOID lpParameter);                                 // 后台线程函数前置声明，供 CreateThread 引用

enum MeasureType                                                                    // 定义 Measure 可输出的测量类型
{
    GPU_CurTemp,                                                                    // 当前温度
    GPU_CurMaxTemp,                                                                 // 本次运行期间记录到的最高温度
    GPU_MaxTemp,                                                                    // 驱动报告的默认最高温度
    GPU_Usage,                                                                      // 当前使用率
    GPU_MaxUsage                                                                    // 本次运行期间记录到的最高使用率
};

class Measure                                                                       // 每个 Rainmeter Measure 实例对应的类
{
public:

    MeasureType type;                                                               // 此 Measure 的测量类型，在 Reload 阶段根据 InfoType 决定

    static std::atomic<bool> advance;                                               // 是否使用高级温度读取方式；静态原子变量，因为后台线程的静态函数需要跨实例共享此回退状态
    DWORD nGPUToGet;                                                                // 此 Measure 对应的 GPU 序号，由配置项 Number 指定

    static DWORD nCurTemp[N_MAX_GPUS];                                              // 每个 GPU 的当前温度缓存
    static DWORD nCurMaxTemp[N_MAX_GPUS];                                           // 每个 GPU 的运行期间最高温度缓存
    static DWORD nMaxTemp[N_MAX_GPUS];                                              // 每个 GPU 的默认最高温度缓存
    static DWORD nUsage[N_MAX_GPUS];                                                // 每个 GPU 的当前使用率缓存
    static DWORD nMaxUsage[N_MAX_GPUS];                                             // 每个 GPU 的运行期间最高使用率缓存
    static std::atomic<int> iNvGPUInit;                                             // NVAPI 初始化实例计数，原子变量保证多实例初始化/销毁时计数不丢失
    static NvU32 nNvGPU;                                                            // 实际枚举到的物理 GPU 数量
    static NvPhysicalGpuHandle hNvGPU[N_MAX_GPUS];                                  // 每个物理 GPU 的 NVAPI 句柄数组

    Measure() : type(GPU_CurTemp), nGPUToGet(0) {}                                  // 构造函数，显式初始化测量类型为当前温度、GPU 序号为 0，避免成员未初始化
    ~Measure() {}                                                                   // 析构函数，实例本身不持有额外资源

    void InitializeGPU(bool createThread)                                           // 非静态成员函数：初始化 NVAPI，并按需创建后台线程
    {
        if (iNvGPUInit.load() <= 0)                                                 // 如果当前没有任何实例完成过 NVAPI 初始化
        {
            NvAPI_Status initStatus = NvAPI_Initialize();                           // 调用 NVAPI 初始化函数
            if (initStatus != NVAPI_OK)                                             // 如果初始化失败
            {
                NvAPI_ShortString errMsg;                                           // 用于接收错误描述文本的缓冲区
                NvAPI_GetErrorMessage(initStatus, errMsg);                          // 把错误码翻译成可读文本
                WCHAR buf[256];                                                     // 用于拼装日志消息的宽字符缓冲区
                swprintf_s(buf, _countof(buf), L"[NvidiaGPU] NvAPI_Initialize failed: %S", errMsg);    // 拼装初始化失败日志，%S 在宽字符格式化中表示窄字符串参数
                RmLog(LOG_ERROR, buf);                                              // 把初始化失败原因写入 Rainmeter 日志
                return;                                                             // 初始化失败，不修改计数，等待下一次尝试
            }

            if (NvAPI_EnumPhysicalGPUs(hNvGPU, &nNvGPU) != NVAPI_OK)                // 枚举系统中所有物理 GPU，填充句柄数组和数量
            {
                RmLog(LOG_ERROR, L"[NvidiaGPU] NvAPI_EnumPhysicalGPUs failed");     // 枚举失败时记录日志
                NvAPI_Unload();                                                     // 卸载 NVAPI，释放已分配的驱动侧资源
                return;                                                             // 返回，不修改计数，下一次仍然会尝试初始化
            }

            iNvGPUInit.store(1);                                                    // 把初始化计数置为 1，表示当前有一个实例在用 NVAPI

            if (createThread)                                                       // 如果调用方要求使用后台线程
            {
                thread.store(true);                                                 // 把线程运行标志置为 true，线程开始时会持续运行
                hEvent = CreateEvent(NULL, FALSE, TRUE, NULL);                      // 创建自动重置事件，初始已触发，用于唤醒线程
                hThread = CreateThread(NULL, 0, ThreadProc, nullptr, 0, NULL);      // 创建后台线程，线程函数不依赖任何 Measure 实例
            }
        }
        else                                                                        // 如果已经有实例完成了 NVAPI 初始化
        {
            iNvGPUInit.fetch_add(1);                                                // 原子递增实例计数，不重复初始化 NVAPI
        }

        if (nNvGPU == 0 || nGPUToGet >= nNvGPU || nGPUToGet >= N_MAX_GPUS)          // 检查 GPU 序号是否越界，或系统中没有 GPU
        {
            if (nGPUToGet != 0)                                                     // 只有在确实需要重置时才记录日志，避免重复刷屏
            {
                nGPUToGet = 0;                                                      // 把 GPU 序号重置为 0，避免后续数组访问越界
                RmLog(LOG_ERROR, L"[NvidiaGPU] Invalid GPU index, fallback to GPU0"); // 记录无效 GPU 序号已回退
            }
        }
    }

    static void FinalizeGPU()                                                       // 静态函数：释放后台线程与 NVAPI 资源（不依赖实例状态）
    {
        if (hThread)                                                                // 如果后台线程存在
        {
            thread.store(false);                                                    // 通知后台线程退出循环
            SetEvent(hEvent);                                                       // 唤醒可能正在等待事件的后台线程

            DWORD waitResult = WaitForSingleObject(hThread, 500);                   // 先尝试等待 500ms，覆盖绝大多数正常场景
            if (waitResult == WAIT_TIMEOUT)                                         // 如果 500ms 内线程仍未退出
            {
                RmLog(LOG_WARNING, L"[NvidiaGPU] Background thread still busy in NVAPI call, waiting..."); // 告知用户卡顿的原因
                WaitForSingleObject(hThread, INFINITE);                             // 继续等待直到线程真正退出，保证内存和 NVAPI 安全
            }

            CloseHandle(hEvent);                                                    // 线程已退出，关闭事件句柄
            hEvent = NULL;                                                          // 重置事件句柄，防止后续误用
            CloseHandle(hThread);                                                   // 关闭线程句柄
            hThread = NULL;                                                         // 重置线程句柄，防止后续误用
        }
        NvAPI_Unload();                                                             // 卸载 NVAPI；仅在最后一个实例释放时调用，顺序上已确保线程完全停止
    }

    static void GetGPUTemperature(DWORD nToGet)                                     // 静态函数：读取指定 GPU 的当前温度
    {
        if (iNvGPUInit.load() == 0 || nToGet >= nNvGPU || hNvGPU[nToGet] == NULL)   // 检查 NVAPI 状态、索引范围和句柄有效性
        {
            return;                                                                 // 条件不满足时直接返回，保持缓存中的旧值
        }
        if (advance.load())                                                         // 如果全局高级温度模式已启用
        {
            GetGPUTemperatureAdvance(nToGet);                                       // 走逐传感器筛选的高级路径
            return;                                                                 // 高级路径已处理完毕，直接返回
        }

        NV_GPU_THERMAL_SETTINGS thermal = {};                                       // 用于接收热设置信息的结构体，显式清零所有字段
        thermal.version = NV_GPU_THERMAL_SETTINGS_VER;                              // 填入当前头文件默认的结构体版本号
        thermal.count = NVAPI_MAX_THERMAL_SENSORS_PER_GPU;                          // 告诉 NVAPI 结构体最多可容纳的传感器数量
        NvAPI_Status ret = NvAPI_GPU_GetThermalSettings(hNvGPU[nToGet], 0, &thermal); // 调用 NVAPI 读取所有热传感器信息
        if (ret != NVAPI_OK)                                                        // 如果基础路径调用失败
        {
            NvAPI_ShortString errMsg;                                               // 用于接收错误描述文本
            NvAPI_GetErrorMessage(ret, errMsg);                                     // 把错误码翻译成可读文本
            WCHAR logBuf[256];                                                      // 用于拼装日志消息的缓冲区
            swprintf_s(logBuf, _countof(logBuf), L"[NvidiaGPU] GetThermalSettings failed (%S), switching to advance mode", errMsg); // 拼装失败日志
            RmLog(LOG_WARNING, logBuf);                                             // 记录失败原因，便于诊断驱动/API 问题
            advance.store(true);                                                    // 全局切换到高级模式，后续不再尝试基础路径
            GetGPUTemperatureAdvance(nToGet);                                       // 立即用高级模式再尝试一次
            return;                                                                 // 高级模式已处理完毕
        }

        if (thermal.count == 0)                                                     // 如果 API 成功但没有任何传感器返回
        {
            nCurTemp[nToGet] = 0;                                                   // 把当前温度视为 0，避免后续访问 sensor[0]
            return;                                                                 // 直接返回
        }

        DWORD tempSum = 0;                                                          // 用于累加所有目标为 GPU 的传感器温度
        DWORD tempCount = 0;                                                        // 记录目标为 GPU 的传感器数量
        for (DWORD i = 0; i < thermal.count && i < NVAPI_MAX_THERMAL_SENSORS_PER_GPU; ++i) // 遍历返回的传感器，同时受 count 和数组容量双重约束
        {
            if (thermal.sensor[i].target == NVAPI_THERMAL_TARGET_GPU)               // 只选择 target 明确标记为 GPU 核心的传感器
            {
                tempSum += thermal.sensor[i].currentTemp;                           // 累加 GPU 核心温度
                tempCount++;                                                        // 计数加一
            }
        }

        if (tempCount > 0)                                                          // 如果找到了至少一个 GPU 目标传感器
        {
            nCurTemp[nToGet] = tempSum / tempCount;                                 // 取平均值作为当前 GPU 核心温度，避免单个异常传感器拉偏结果
        }
        else                                                                        // 如果没有任何传感器的 target 标记为 GPU
        {
            nCurTemp[nToGet] = thermal.sensor[0].currentTemp;                       // 退回到索引 0，兼容部分旧驱动或特殊硬件未正确标记 target 的情况
            RmLog(LOG_WARNING, L"[NvidiaGPU] No NVAPI_THERMAL_TARGET_GPU sensor found, using sensor[0]"); // 记录一次警告，提示语义可能不精确
        }

        if (nCurTemp[nToGet] > nCurMaxTemp[nToGet]) nCurMaxTemp[nToGet] = nCurTemp[nToGet]; // 如果超过历史最高温度则更新
        nMaxTemp[nToGet] = thermal.sensor[0].defaultMaxTemp;                        // 记录驱动报告的默认最高温度，作为 Rainmeter 的 maxValue
    }

    static void GetGPUUsage(DWORD nToGet)                                           // 静态函数：读取指定 GPU 的当前使用率
    {
        if (iNvGPUInit.load() == 0 || nToGet >= nNvGPU || hNvGPU[nToGet] == NULL)   // 检查 NVAPI 状态、索引范围和句柄有效性
        {
            nUsage[nToGet] = GPU_USAGE_UNAVAILABLE;                                 // 标记使用率为不可用
            return;                                                                 // 直接返回
        }

        NV_GPU_DYNAMIC_PSTATES_INFO_EX infoEx = {};                                 // 用于接收动态 P-State 信息的结构体，清零所有字段
        infoEx.version = NV_GPU_DYNAMIC_PSTATES_INFO_EX_VER;                        // 填入当前结构体版本号
        NvAPI_Status ret = NvAPI_GPU_GetDynamicPstatesInfoEx(hNvGPU[nToGet], &infoEx); // 调用 API 读取各引擎的使用率
        if (ret != NVAPI_OK || infoEx.utilization[0].bIsPresent == 0)               // 如果调用失败，或 GPU 域不可用
        {
            nUsage[nToGet] = GPU_USAGE_UNAVAILABLE;                                 // 标记使用率为不可用
            return;                                                                 // 直接返回
        }

        nUsage[nToGet] = infoEx.utilization[0].percentage;                          // utilization[0] 对应 GPU（图形引擎）域，取其忙碌百分比
        if (nUsage[nToGet] > nMaxUsage[nToGet]) nMaxUsage[nToGet] = nUsage[nToGet]; // 如果超过历史最高使用率则更新
    }

    static void GetGPUName(void)                                                    // 静态函数：枚举每个 GPU 并把名称写入 Rainmeter 日志
    {
        NvAPI_Status status;                                                        // 用于接收每次 API 调用的返回值
        NvAPI_ShortString sName;                                                    // 用于接收 GPU 名称的 ANSI 字符串缓冲区

        WCHAR info[128];                                                            // 用于拼装最终日志文本的宽字符缓冲区
        WCHAR nameW[64];                                                            // 用于接收从 ANSI 转成宽字符后的 GPU 名称
        for (DWORD n = 0; n < nNvGPU; n++)                                          // 遍历所有已枚举到的物理 GPU
        {
            status = NvAPI_GPU_GetFullName(hNvGPU[n], sName);                       // 调用 API 获取该 GPU 的完整名称
            if (status == NVAPI_OK)                                                 // 如果名称获取成功
            {
                MultiByteToWideChar(CP_ACP, 0, sName, -1, nameW, _countof(nameW)); // 把 ANSI 名称转为宽字符，目标缓冲区大小由 _countof 提供
                swprintf_s(info, _countof(info), L"GPU%u : %s", n, nameW);          // 拼装 "GPU序号 : 名称" 形式的日志文本
                RmLog(LOG_NOTICE, info);                                            // 把 GPU 名称以提示级别写入 Rainmeter 日志
            }
        }
    }

private:

    static void InitThermal(NV_GPU_THERMAL_SETTINGS& thermal, NvU32 Version)        // 静态辅助函数：初始化热设置结构体
    {
        ZeroMemory(&thermal, sizeof(NV_GPU_THERMAL_SETTINGS));                      // 把整个结构体清零，避免残留垃圾值
        thermal.version = Version;                                                  // 填入调用方指定的结构体版本号
        thermal.count = NVAPI_MAX_THERMAL_SENSORS_PER_GPU;                          // 告诉 NVAPI 结构体最多可容纳的传感器数量
    }

    static void GetGPUTemperatureAdvance(DWORD nToGet)                              // 高级温度路径：按结构体版本逐级降级，并按 target 筛选
    {
        NvAPI_Status ret = NVAPI_OK;                                                // 用于接收 API 调用的返回值，预置为成功
        NV_GPU_THERMAL_SETTINGS thermal;                                            // 用于接收热设置信息的结构体
        InitThermal(thermal, NV_GPU_THERMAL_SETTINGS_VER);                          // 先用当前头文件默认版本初始化
        ret = NvAPI_GPU_GetThermalSettings(hNvGPU[nToGet], 0, &thermal);            // 用默认版本调用 API 读取热设置
        if (ret == NVAPI_INCOMPATIBLE_STRUCT_VERSION)                               // 如果驱动认为结构体版本不兼容
        {
            InitThermal(thermal, NV_GPU_THERMAL_SETTINGS_VER_1);                    // 降级到版本 1 重新初始化
            ret = NvAPI_GPU_GetThermalSettings(hNvGPU[nToGet], 0, &thermal);        // 用版本 1 再次调用 API
            if (ret == NVAPI_INCOMPATIBLE_STRUCT_VERSION)                           // 如果版本 1 仍然不兼容
            {
                InitThermal(thermal, NV_GPU_THERMAL_SETTINGS_VER_2);                // 再降级到版本 2 重新初始化
                ret = NvAPI_GPU_GetThermalSettings(hNvGPU[nToGet], 0, &thermal);    // 用版本 2 再次调用 API
            }
        }

        if (ret != NVAPI_OK)                                                        // 如果所有版本都调用失败
        {
            nCurTemp[nToGet] = 0;                                                   // 把当前温度视为 0，表示读取失败
            return;                                                                 // 直接返回
        }

        if (thermal.count == 0)                                                     // 如果 API 成功但没有任何传感器返回
        {
            nCurTemp[nToGet] = 0;                                                   // 把当前温度视为 0，避免后面除零
            return;                                                                 // 直接返回
        }

        DWORD tempSum = 0;                                                          // 用于累加所有目标为 GPU 的传感器温度
        DWORD tempCount = 0;                                                        // 记录目标为 GPU 的传感器数量
        for (DWORD i = 0; i < thermal.count && i < NVAPI_MAX_THERMAL_SENSORS_PER_GPU; ++i) // 遍历返回的传感器，同时受 count 和数组容量约束
        {
            if (thermal.sensor[i].target == NVAPI_THERMAL_TARGET_GPU)               // 只选择 target 标记为 GPU 核心的传感器
            {
                tempSum += thermal.sensor[i].currentTemp;                           // 累加 GPU 核心温度
                tempCount++;                                                        // 计数加一
            }
        }

        if (tempCount > 0)                                                          // 如果找到了至少一个 GPU 目标传感器
        {
            nCurTemp[nToGet] = tempSum / tempCount;                                 // 取平均值作为当前 GPU 核心温度
        }
        else                                                                        // 如果没有任何传感器的 target 标记为 GPU
        {
            nCurTemp[nToGet] = thermal.sensor[0].currentTemp;                       // 退回到索引 0，兼容 target 标记不规范的场景
            RmLog(LOG_WARNING, L"[NvidiaGPU] Advance mode: no NVAPI_THERMAL_TARGET_GPU, using sensor[0]"); // 记录一次警告
        }

        if (nCurTemp[nToGet] > nCurMaxTemp[nToGet]) nCurMaxTemp[nToGet] = nCurTemp[nToGet]; // 如果超过历史最高温度则更新
        nMaxTemp[nToGet] = thermal.sensor[0].defaultMaxTemp;                        // 记录驱动报告的默认最高温度
    }
};

std::atomic<bool> Measure::advance{ false };                                        // 静态成员定义，全局高级温度模式标志，初始为 false
std::atomic<int> Measure::iNvGPUInit{ 0 };                                          // 静态成员定义，NVAPI 初始化实例计数，初始为 0
DWORD Measure::nCurTemp[N_MAX_GPUS];                                                // 静态成员定义，当前温度缓存数组
DWORD Measure::nCurMaxTemp[N_MAX_GPUS];                                             // 静态成员定义，运行期间最高温度缓存数组
DWORD Measure::nMaxTemp[N_MAX_GPUS];                                                // 静态成员定义，默认最高温度缓存数组
DWORD Measure::nUsage[N_MAX_GPUS];                                                  // 静态成员定义，当前使用率缓存数组
DWORD Measure::nMaxUsage[N_MAX_GPUS];                                               // 静态成员定义，运行期间最高使用率缓存数组
NvPhysicalGpuHandle Measure::hNvGPU[N_MAX_GPUS];                                    // 静态成员定义，物理 GPU 句柄数组
NvU32 Measure::nNvGPU;                                                              // 静态成员定义，物理 GPU 数量

#define UPDATE_GPUTEMP  0x01                                                        // 位标志：表示需要刷新温度
#define UPDATE_GPUUSAGE 0x02                                                        // 位标志：表示需要刷新使用率

static DWORD WINAPI ThreadProc(LPVOID lpParameter)                                  // 后台线程入口，参数未使用，线程完全依赖静态数据
{
    while (1)                                                                       // 线程主循环，持续运行直到 thread 标志为 false
    {
        for (DWORD n = 0; n < Measure::nNvGPU; n++)                                 // 遍历所有已枚举到的物理 GPU
        {
            if (bUpdate[n].load() & UPDATE_GPUTEMP)                                 // 原子读取该 GPU 的温度刷新标志
            {
                if (!thread.load()) return 0;                                       // 如果线程已被要求退出，立即返回
                Measure::GetGPUTemperature(n);                                      // 调用静态函数读取该 GPU 的温度
                bUpdate[n].fetch_and(static_cast<uint8_t>(~UPDATE_GPUTEMP));        // 原子清除温度刷新标志，避免与主线程的置位操作丢失更新
            }
            if (bUpdate[n].load() & UPDATE_GPUUSAGE)                                // 原子读取该 GPU 的使用率刷新标志
            {
                if (!thread.load()) return 0;                                       // 如果线程已被要求退出，立即返回
                Measure::GetGPUUsage(n);                                            // 调用静态函数读取该 GPU 的使用率
                bUpdate[n].fetch_and(static_cast<uint8_t>(~UPDATE_GPUUSAGE));       // 原子清除使用率刷新标志
            }
        }
        if (!thread.load()) return 0;                                               // 如果线程已被要求退出，立即返回
        WaitForSingleObject(hEvent, INFINITE);                                      // 等待事件被触发；自动重置事件在等待返回时自动复位
        if (!thread.load()) return 0;                                               // 被唤醒后再次检查线程退出标志
    }
}

PLUGIN_EXPORT void Initialize(void** data, void* rm)                                // Rainmeter 插件入口：创建新的 Measure 实例
{
    Measure* measure = new Measure;                                                 // 在堆上创建 Measure 实例
    *data = measure;                                                                // 把实例指针交还给 Rainmeter，后续回调会传回该指针

    int requestedGpu = RmReadInt(rm, L"Number", 0);                                 // 从配置中读取 GPU 序号，默认值为 0
    if (requestedGpu < 0) requestedGpu = 0;                                         // 如果配置为负数，钳制为 0，避免转换成无符号后越界
    measure->nGPUToGet = (DWORD)requestedGpu;                                       // 把配置的 GPU 序号写入实例字段

    bool createThread = RmReadInt(rm, L"NewThread", 0) > 0;                         // 读取是否启用后台线程的配置
    measure->InitializeGPU(createThread);                                           // 调用成员函数初始化 NVAPI，并按需创建后台线程

    measure->GetGPUTemperature(measure->nGPUToGet);                                 // 预先读取一次温度，让 Reload 阶段能拿到默认最高温度

    BOOL showlog = RmReadInt(rm, L"Showlog", 0) > 0 ? true : false;                 // 读取是否输出调试日志的配置

    if (!measure->advance.load()) measure->advance.store(RmReadInt(rm, L"Advance", 0) > 0 ? true : false); // 如果尚未自动进入高级模式，则根据配置决定是否启用

    if (showlog && measure->iNvGPUInit.load() == 1)                                 // 如果用户要求输出日志，并且当前是第一个完成初始化的实例
    {
        measure->GetGPUName();                                                      // 把所有 GPU 名称写入 Rainmeter 日志
        if (measure->advance.load()) RmLog(LOG_NOTICE, L"[NvidiaGPU] AdvanceMode"); // 如果启用了高级模式，额外记录一条日志
    }
}

PLUGIN_EXPORT void Reload(void* data, void* rm, double* maxValue)                   // Rainmeter 插件回调：设置测量类型和最大值
{
    Measure* measure = (Measure*)data;                                              // 把传入的通用指针还原成 Measure 指针

    LPCWSTR value = RmReadString(rm, L"InfoType", L"Temperature");                  // 读取 InfoType 配置项，默认值为 Temperature
    if (_wcsicmp(value, L"Temperature") == 0)                                       // 如果配置为当前温度
    {
        measure->type = GPU_CurTemp;                                                // 设置测量类型为当前温度
        *maxValue = measure->nMaxTemp[measure->nGPUToGet];                          // 把默认最高温度作为最大值交给 Rainmeter
    }
    else if (_wcsicmp(value, L"MaxTemperature") == 0)                               // 如果配置为运行期间最高温度
    {
        measure->type = GPU_CurMaxTemp;                                             // 设置测量类型为运行期间最高温度
        *maxValue = measure->nMaxTemp[measure->nGPUToGet];                          // 把默认最高温度作为最大值交给 Rainmeter
    }
    else if (_wcsicmp(value, L"DefaultMaxTemp") == 0)                               // 如果配置为默认最高温度
    {
        measure->type = GPU_MaxTemp;                                                // 设置测量类型为默认最高温度
    }
    else if (_wcsicmp(value, L"Usage") == 0)                                        // 如果配置为当前使用率
    {
        measure->type = GPU_Usage;                                                  // 设置测量类型为当前使用率
        *maxValue = 100.0;                                                          // 使用率是百分比，最大值设为 100
    }
    else if (_wcsicmp(value, L"MaxUsage") == 0)                                     // 如果配置为运行期间最高使用率
    {
        measure->type = GPU_MaxUsage;                                               // 设置测量类型为运行期间最高使用率
        *maxValue = 100.0;                                                          // 使用率是百分比，最大值设为 100
    }
    else
    {
        measure->type = GPU_CurTemp;                                                // 无法识别的 InfoType 退回到当前温度
        *maxValue = measure->nMaxTemp[measure->nGPUToGet];                          // 把默认最高温度作为最大值交给 Rainmeter
        RmLog(LOG_ERROR, L"[NvidiaGPU] Invalid InfoType");                          // 把无效 InfoType 记录到日志
    }
}

PLUGIN_EXPORT double Update(void* data)                                             // Rainmeter 插件回调：每次刷新时返回当前测量值
{
    Measure* measure = (Measure*)data;                                              // 把传入的通用指针还原成 Measure 指针

    if (measure->type == GPU_CurTemp)                                               // 如果测量类型是当前温度
    {
        if (thread.load())                                                          // 如果后台线程正在运行
        {
            bUpdate[measure->nGPUToGet].fetch_or(UPDATE_GPUTEMP);                   // 原子置位温度刷新标志，请求后台线程更新
            SetEvent(hEvent);                                                       // 触发事件唤醒后台线程
        }
        else measure->GetGPUTemperature(measure->nGPUToGet);                        // 否则直接在当前线程读取温度
        return measure->nCurTemp[measure->nGPUToGet];                               // 返回缓存的当前温度
    }
    else if (measure->type == GPU_CurMaxTemp)                                       // 如果测量类型是运行期间最高温度
    {
        return measure->nCurMaxTemp[measure->nGPUToGet];                            // 直接返回缓存的历史最高温度
    }
    else if (measure->type == GPU_MaxTemp)                                          // 如果测量类型是默认最高温度
    {
        return measure->nMaxTemp[measure->nGPUToGet];                               // 直接返回缓存的默认最高温度
    }
    else if (measure->type == GPU_Usage)                                            // 如果测量类型是当前使用率
    {
        if (thread.load())                                                          // 如果后台线程正在运行
        {
            bUpdate[measure->nGPUToGet].fetch_or(UPDATE_GPUUSAGE);                  // 原子置位使用率刷新标志
            SetEvent(hEvent);                                                       // 触发事件唤醒后台线程
        }
        else measure->GetGPUUsage(measure->nGPUToGet);                              // 否则直接在当前线程读取使用率
        return measure->nUsage[measure->nGPUToGet];                                 // 返回缓存的当前使用率
    }
    else if (measure->type == GPU_MaxUsage)                                         // 如果测量类型是运行期间最高使用率
    {
        return measure->nMaxUsage[measure->nGPUToGet];                              // 直接返回缓存的历史最高使用率
    }
    else return 0.0;                                                                // 其他未知类型统一返回 0
}

PLUGIN_EXPORT void Finalize(void* data)                                             // Rainmeter 插件回调：销毁 Measure 实例
{
    Measure* measure = (Measure*)data;                                              // 把传入的通用指针还原成 Measure 指针

    if (measure->iNvGPUInit.load() > 0) measure->iNvGPUInit.fetch_sub(1);           // 原子递减 NVAPI 实例计数，避免计数变成负数
    if (measure->iNvGPUInit.load() < 1) measure->FinalizeGPU();                     // 如果已经没有实例在使用 NVAPI，则停止线程并卸载 NVAPI

    delete measure;                                                                 // 释放 Measure 实例占用的堆内存
}
