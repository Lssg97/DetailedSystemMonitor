// PawnTemp.cpp

// ==================== 引用头文件和外部库 ====================
#include <windows.h>                          // Windows API 总头（CreateFileW、DeviceIoControl、DllMain 等）
#include <atomic>                             // std::atomic：跨线程无锁读写
#include <thread>                             // std::thread：采集线程
#include <vector>                             // std::vector
#include <cstring>                            // std::memcpy / strlen / strcmp
#include <cstdint>                            // uint32_t / int64_t 等定长整数
#include <cstdio>                             // swprintf_s
#include <intrin.h>                           // __cpuid
#include "../API/Rainmeter/RainmeterAPI.h"    // Rainmeter 插件 API 声明

// ==================== 嵌入外部二进制资源（需与 .rc 文件 ID 一致） ====================
#define IDR_INTEL_MSR    101    // 嵌入 IntelMSR.bin 模块
#define IDR_AMD_FAMILY17 102    // 嵌入 AMDFamily17.bin 模块（Zen、Zen+、Zen2 / Zen3、Zen4 / Zen5）
#define IDR_AMD_FAMILY10 103    // 嵌入 AMDFamily10.bin 模块（K10 / Turion / K12 / Bobcat / Bulldozer、Piledriver、Steamroller、Excavator / Jaguar、Puma）
#define IDR_AMD_FAMILY0F 104    // 嵌入 AMDFamily0F.bin 模块（K8、Hammer）

// ==================== 保存插件 DLL 自身句柄（后面 FindResourceW 需查内嵌资源） ====================
static HMODULE g_hThisDll = nullptr;

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID /*lpvReserved*/) {
    if (fdwReason == DLL_PROCESS_ATTACH) {    // 当 DLL 被加载到进程时
        g_hThisDll = hinstDLL;                // 记下自己的句柄
    }
    return TRUE;                              // 加载成功返回 True
}

// ==================== PawnIO 常量 ====================
constexpr uint32_t PAWNIO_DEVICE_TYPE    = 41394u << 16;                               // PawnIO 驱动约定魔数
constexpr uint32_t IOCTL_PIO_EXECUTE_FN  = 0x841 << 2;                                 // 让驱动执行某个已加载模块里的函数
constexpr uint32_t IOCTL_PIO_LOAD_BINARY = 0x821 << 2;                                 // 把一段二进制模块加载进驱动
constexpr int      FN_NAME_LENGTH        = 32;                                         // PawnIO 输入缓冲区函数名占固定 32 字节
constexpr const wchar_t* PAWNIO_DEVICE_PATH = L"\\\\?\\GLOBALROOT\\Device\\PawnIO";    // 绕过 DOS 路径解析，直接访问内核对象命名空间的设备路径

// ==================== Intel MSR ====================
constexpr uint32_t MSR_IA32_THERM_STATUS       = 0x19C;    // 距 TjMax 相差温度
constexpr uint32_t MSR_IA32_TEMPERATURE_TARGET = 0x1A2;    // bit[23:16] 是 TjMax 温度

// ==================== AMD SMN (17h+) ====================
constexpr uint32_t SMN_THM_TCON_CUR_TMP = 0x00059800;      // 封装温度寄存器（readout × 0.125°C，bit19 为 range select，需减 49℃）

constexpr uint32_t F17H_M70H_CCD1_TEMP = 0x00059954;       // 通用 CCD 基址（0x59800 + 0x154）
constexpr uint32_t F17H_M61H_CCD1_TEMP = 0x00059B08;       // Raphael (0x61) / Granite Ridge (0x44) 专用（0x59800 + 0x308）
constexpr uint32_t F1AH_M00H_CCD1_TEMP = 0x000599F0;       // Turin / Shimada Peak (Family 1Ah Model ≤ 0x2F)（0x59800 + 0x1F0）

constexpr int      MAX_CCD_COUNT         = 16;             // 支持最多 16 个 CCD（Turin 上限）
constexpr uint32_t ZEN_CCD_TEMP_VALID    = (1u << 11);     // CCD 温度有效位，仅 Turin 使用
constexpr uint32_t ZEN_CCD_TEMP_MASK     = 0xFFF;          // 取低 12 位

// ==================== AMD MiscCtl (10-16h) / SMU (Excavator) ====================

constexpr uint32_t AMD10_REG_REPORTED_TEMP = 0xA4;              // Reported Temperature Control 寄存器（PCI Function 3）
constexpr uint32_t AMD15H_M60H_SMU_TEMP_OFFSET = 0xD8200CA4;    // Excavator 部分型号访问的 SMU 间接寄存器

// ==================== AMD ThermTrip (0Fh) ====================

constexpr uint32_t AMD0F_THERMTRIP_REG = 0xE4;                  // Thermtrip Status 寄存器（PCI Function 3）

// ==================== 线程调度参数 ====================
constexpr int MIN_PER_CORE_MS = 20;                // 相邻两次核心切换之间的最小间隔（ms）
constexpr DWORD SMN_MUTEX_MAX_TIMEOUT_MS = 100;    // SMN 互斥体等待上限，让 Finalize 的 join 更快返回，减少与 LHM 竞争时的阻塞

// ==================== 错误码 ====================
enum PluginError : int {
    ERR_NONE                   = 0,    // 正常
    ERR_OPEN_DEVICE            = 1,    // 打不开 PawnIO 设备（未安装 / 无管理员权限）
    ERR_LOAD_MODULE            = 2,    // 内嵌模块加载失败
    ERR_UNSUPPORTED_CPU_FAMILY = 3,    // 不支持的 CPU 家族（非 Intel、非 AMD 0Fh~1Ah）
    ERR_ENUM_CORES             = 4,    // 枚举物理核失败
    ERR_PCI_MUTEX              = 5,    // PCI 互斥体获取超时（SMN 总线被占用）
    ERR_READ_TJMAX             = 6,    // 读不到 TjMax（MSR 被拒）
};

// ==================== CPU 身份识别 ====================
enum class CpuVendor { Intel, AmdFamily17, AmdFamily10, AmdFamily0F, Unsupported };

struct CpuIdentity {
    CpuVendor vendor    = CpuVendor::Unsupported;    // 判定后的厂商 / 家族类别
    uint32_t  family    = 0;                         // 组合后（含扩展位）的 Family
    uint32_t  model     = 0;                         // 组合后（含扩展位）的 Model
    char      brand[13] = {};                        // 12 字节厂商字符串 + '\0'（"GenuineIntel" / "AuthenticAMD"）
    char      name[49]  = {};                        // 48 字节 CPU 完整型号名 + '\0'（如 "AMD Ryzen 7 1800X ..."）
};

static CpuIdentity DetectCpu() {
    CpuIdentity id;
    int regs[4] = {};

    // CPUID 0：取厂商字符串 "GenuineIntel" / "AuthenticAMD"
    __cpuid(regs, 0);
    memcpy(id.brand,     &regs[1], 4);    // EBX
    memcpy(id.brand + 4, &regs[3], 4);    // EDX
    memcpy(id.brand + 8, &regs[2], 4);    // ECX

    // CPUID 1：取 Family / Model
    __cpuid(regs, 1);
    uint32_t eax = regs[0];
    uint32_t baseFamily = (eax >> 8) & 0xF;
    uint32_t baseModel  = (eax >> 4) & 0xF;
    uint32_t extFamily  = (eax >> 20) & 0xFF;
    uint32_t extModel   = (eax >> 16) & 0xF;

    const bool isIntel = (strcmp(id.brand, "GenuineIntel") == 0);
    const bool isAmd   = (strcmp(id.brand, "AuthenticAMD") == 0);

    // 组合 Family
    id.family = (baseFamily == 0xF) ? (baseFamily + extFamily) : baseFamily;

    if (isIntel) {
        id.model  = (baseFamily == 0x6 || baseFamily == 0xF)
                    ? ((extModel << 4) + baseModel) : baseModel;    // Intel：Family 6 / F 用扩展 Model，其余仅 baseModel
        id.vendor = CpuVendor::Intel;
    } else if (isAmd) {
        id.model = (baseFamily == 0xF)
                   ? ((extModel << 4) + baseModel) : baseModel;     // AMD：Family F 用扩展 Model
        if (id.family == 0x0F) {
            id.vendor = CpuVendor::AmdFamily0F;                     // Family 0Fh
        } else if (id.family >= 0x10 && id.family <= 0x16) {
            id.vendor = CpuVendor::AmdFamily10;                     // Family 10h~16h
        } else if (id.family >= 0x17 && id.family <= 0x1A) {
            id.vendor = CpuVendor::AmdFamily17;                     // Family 17h~1Ah
        } else {
            id.vendor = CpuVendor::Unsupported;
        }
    }

    // CPUID 0x80000002~0x80000004：完整 CPU 型号，用于 Tctl offset 判断。
    __cpuid(regs, 0x80000000);
    if (static_cast<uint32_t>(regs[0]) >= 0x80000004u) {    // 只有在 CPUID 0x80000000 支持的最大扩展叶 >= 0x80000004 时才读取
        __cpuid(regs, 0x80000002);
        memcpy(id.name,      &regs[0], 4);
        memcpy(id.name + 4,  &regs[1], 4);
        memcpy(id.name + 8,  &regs[2], 4);
        memcpy(id.name + 12, &regs[3], 4);
        __cpuid(regs, 0x80000003);
        memcpy(id.name + 16, &regs[0], 4);
        memcpy(id.name + 20, &regs[1], 4);
        memcpy(id.name + 24, &regs[2], 4);
        memcpy(id.name + 28, &regs[3], 4);
        __cpuid(regs, 0x80000004);
        memcpy(id.name + 32, &regs[0], 4);
        memcpy(id.name + 36, &regs[1], 4);
        memcpy(id.name + 40, &regs[2], 4);
        memcpy(id.name + 44, &regs[3], 4);
        id.name[48] = '\0';
    }
    return id;
}

// 根据 CPU 完整型号名计算 Zen 1/1+ 特定型号的 Tctl offset。数据来源：Linux k10temp.c 及 LibreHardwareMonitor Amd17Cpu.cs
static float ComputeTctlOffset(const char* name) {
    if (name == nullptr || name[0] == '\0') return 0.0f;
    if (strstr(name, "1600X") || strstr(name, "1700X") || strstr(name, "1800X")) {
        return -20.0f;    // Ryzen 1000X 系列
    }
    if (strstr(name, "Threadripper 19") || strstr(name, "Threadripper 29")) {
        return -27.0f;    // Threadripper 1000/2000 系列
    }
    if (strstr(name, "2700X")) {
        return -10.0f;    // Ryzen 2000X 系列
    }
    return 0.0f;
}

// ==================== MeasureData ====================
struct MeasureData {
    CRITICAL_SECTION cs{};                              // 临界区，保护下面若干字段
    std::vector<int> sharedTemps;                       // Intel：每物理核温度（-1 = 无效）
                                                        // AMD：多 CCD 时存储每个 CCD 的温度 ×1000（0.001℃ 精度），单 CCD 时仅 sharedTemps[0] 温度 ×1000
    std::vector<int> coreToCcd;                         // 物理核心索引 对应 CCD 索引的映射表（仅 AMD 多 CCD 使用），前 (coreCount % ccdCount) 个 CCD 各分配 (coreCount / ccdCount + 1) 个核心，其余分配 (coreCount / ccdCount) 个
    int tjMax = 0;                                      // 温度上限
    int physicalCoreCount = 0;                          // 物理核数（AMD 用于 CoreIndex 越界判断）
    int ccdCount = 0;                                   // CCD 数量（AMD 用于 CoreIndex 有效性判断）
    uint32_t ccdOffset = 0;                             // CCD 寄存器基址（复用字段，实际存的是 ccdBase）
    bool isTurinCcd = false;                            // 是否为 Turin / Shimada Peak（决定有效位判断方式）
    char cpuName[49] = {};                              // CPU 型号，用于 Tctl offset 判断
    float  amdLegacyOffset = 0.0f;                      // AMD 0Fh-16h 的固定温度偏移
    bool   amdHasSmuTemp   = false;                     // Family 15h Model 60h/70h 走 SMU 间接访问
    std::atomic<bool> running{ false };                 // 采集线程是否应继续跑
    std::atomic<int> collectIntervalMs{ 100 };          // 一轮（所有核各读一遍）的目标周期
    std::atomic<int> requestedCore{ -1 };               // -1=最大值, -2=平均值, >=0=指定核
    std::atomic<int> errorCode{ ERR_NONE };             // 采集线程写入的错误
    std::atomic<int> lastReportedError{ ERR_NONE };     // 主线程已上报的错误（去重用）
    std::thread collector;                              // 采集线程句柄
    HANDLE hStopEvent = nullptr;                        // 停止事件：Finalize 里 SetEvent 后，采集线程立刻从等待中醒来，否则 join() 会等采集线程的 sleep_for 自然到期，导致刷新皮肤卡顿
    void* rm = nullptr;                                 // Rainmeter 的 Measure 句柄

    // CPU 识别结果：Reload 首次写入，之后 ReadOnly
    bool      cpuDetected = false;
    CpuVendor vendor      = CpuVendor::Unsupported;
    uint32_t  cpuFamily   = 0;
    uint32_t  cpuModel    = 0;
};

// ==================== PawnIO 访问 ====================
class PawnIoAccess {                         // 访问封装
public:
    HANDLE hDevice = INVALID_HANDLE_VALUE;   // 设备句柄
    bool moduleLoaded = false;               // 模块是否加载成功

    ~PawnIoAccess() { if (hDevice != INVALID_HANDLE_VALUE) CloseHandle(hDevice); }

    bool Open() {
        hDevice = CreateFileW(PAWNIO_DEVICE_PATH,
            GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,    // 读写权限 & 允许别的进程也打开
            nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);             // 设备必须已存在，只打开、不新建
        return hDevice != INVALID_HANDLE_VALUE;
    }

    bool LoadModuleFromResource(int resourceId) {                                     // 从 DLL 中加载模块
        HMODULE hExe = g_hThisDll;
        if (!hExe) return false;                                                      // 没用 DllMain 存到句柄

        HRSRC hRes = FindResourceW(hExe, MAKEINTRESOURCEW(resourceId), RT_RCDATA);    // 找模块
        if (!hRes) return false;

        HGLOBAL hData = LoadResource(hExe, hRes);                                     // 把资源句柄转成内存块
        if (!hData) return false;

        DWORD size = SizeofResource(hExe, hRes);                                      // 资源字节数
        void* data = LockResource(hData);                                             // 得到指向资源数据的指针
        if (!data || size == 0) return false;

        DWORD bytesReturned = 0;
        BOOL ok = DeviceIoControl(                                                    // 将内嵌模块通过 DeviceIoControl 交给驱动加载
            hDevice,
            PAWNIO_DEVICE_TYPE | IOCTL_PIO_LOAD_BINARY,                               // 用按位或拼出 IOCTL 码
            data, size,                                                               // 输入缓冲区 = 模块二进制
            nullptr, 0,                                                               // 无输出
            &bytesReturned, nullptr);

        moduleLoaded = (ok != FALSE);
        return moduleLoaded;
    }

    // 通用执行接口：Intel 用 "ioctl_read_msr"，AMD 用 "ioctl_read_smn"
    bool Execute(const char* fnName, uint64_t inputValue, uint64_t& output) {
        if (!moduleLoaded) return false;
        const size_t nameLen = strlen(fnName);
        if (nameLen >= FN_NAME_LENGTH) return false;

        const int inputSize = FN_NAME_LENGTH + sizeof(int64_t);            // 执行函数输入格式：函数名字符串 32 字节 + 参数 8 字节
        std::vector<uint8_t> input(inputSize, 0);                          // 全 0 输入缓冲区
        memcpy(input.data(), fnName, nameLen);                             // 函数名（含结尾 \0，其余保留 0）
        int64_t param = static_cast<int64_t>(inputValue);
        memcpy(input.data() + FN_NAME_LENGTH, &param, sizeof(int64_t));    // 参数写入后 8 字节

        int64_t outValue = 0;
        DWORD bytesReturned = 0;
        BOOL ok = DeviceIoControl(
            hDevice,
            PAWNIO_DEVICE_TYPE | IOCTL_PIO_EXECUTE_FN,                     // 执行已加载模块里的函数
            input.data(), inputSize,                                       // 输入
            &outValue, sizeof(outValue),                                   // 输出：8 字节值
            &bytesReturned, nullptr);
        if (!ok) return false;
        output = static_cast<uint64_t>(outValue);
        return true;
    }

    // 双参数执行接口：用于需要两个输入参数（cpuIndex + registerOffset / coreIndex）的函数
    bool Execute2(const char* fnName, uint64_t arg0, uint64_t arg1, uint64_t& output) {
        if (!moduleLoaded) return false;
        const size_t nameLen = strlen(fnName);
        if (nameLen >= FN_NAME_LENGTH) return false;

        const int inputSize = FN_NAME_LENGTH + 2 * sizeof(int64_t);
        std::vector<uint8_t> input(inputSize, 0);
        memcpy(input.data(), fnName, nameLen);
        int64_t p0 = static_cast<int64_t>(arg0);
        int64_t p1 = static_cast<int64_t>(arg1);
        memcpy(input.data() + FN_NAME_LENGTH,        &p0, sizeof(int64_t));
        memcpy(input.data() + FN_NAME_LENGTH + 8,    &p1, sizeof(int64_t));

        int64_t outValue = 0;
        DWORD bytesReturned = 0;
        BOOL ok = DeviceIoControl(
            hDevice,
            PAWNIO_DEVICE_TYPE | IOCTL_PIO_EXECUTE_FN,
            input.data(), inputSize,
            &outValue, sizeof(outValue),
            &bytesReturned, nullptr);
        if (!ok) return false;
        output = static_cast<uint64_t>(outValue);
        return true;
    }

    bool ReadMsr(uint32_t msrIndex, uint64_t& value) {                             // 读一个 MSR
        return Execute("ioctl_read_msr", msrIndex, value);
    }

    bool ReadSmn(uint32_t smnIndex, uint64_t& value) {                             // 读 SMN 寄存器
        return Execute("ioctl_read_smn", smnIndex, value);
    }

    bool ReadMiscCtl(uint32_t cpuIndex, uint32_t regOffset, uint64_t& value) {     // 读 MISCCTL 寄存器（传双参数）
        return Execute2("ioctl_read_miscctl", cpuIndex, regOffset, value);
    }

    bool ReadSmu(uint32_t smuAddr, uint64_t& value) {                              // 读 SMU 间接寄存器
        return Execute("ioctl_read_smu", smuAddr, value);
    }

    bool GetThermtrip(uint32_t cpuIndex, uint32_t coreIndex, uint64_t& value) {    // 读 Thermtrip 状态寄存器（传双参数）
        return Execute2("ioctl_get_thermtrip", cpuIndex, coreIndex, value);
    }
};

// ==================== 物理核心枚举 ====================
std::vector<DWORD_PTR> GetPhysicalCoreMasks() {
    DWORD len = 0;
    // 第一次调用预期失败并返回 ERROR_INSUFFICIENT_BUFFER
    GetLogicalProcessorInformation(nullptr, &len);
    if (GetLastError() != ERROR_INSUFFICIENT_BUFFER) return {};
    // 第二次调用，填数据
    std::vector<SYSTEM_LOGICAL_PROCESSOR_INFORMATION> buf(
        len / sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION));
    if (!GetLogicalProcessorInformation(buf.data(), &len)) return {};
    // 只关注"物理核心"条目
    std::vector<DWORD_PTR> masks;
    for (const auto& info : buf) {
        if (info.Relationship != RelationProcessorCore) continue;
        DWORD_PTR lowest = info.ProcessorMask & (~info.ProcessorMask + 1);    // x & (~x + 1) 等价于 x & -x，取最低位代表该物理核
        if (lowest != 0) masks.push_back(lowest);
    }
    return masks;                                                             // 这里每个元素都是某个物理核的一个代表逻辑处理器的亲和性掩码
}

// ==================== Intel 温度计算 ====================
bool ReadTjMax(PawnIoAccess& pio, int& tjMax) {
    uint64_t v = 0;
    if (!pio.ReadMsr(MSR_IA32_TEMPERATURE_TARGET, v)) return false;    // 读 0x1A2
    tjMax = static_cast<int>((v >> 16) & 0xFF);                        // 取 bit[23:16] 共 8 位，就是 TjMax 摄氏度。
    return tjMax > 0;
}

bool ReadCoreTemp(PawnIoAccess& pio, int tjMax, int& tempC) {
    uint64_t v = 0;
    if (!pio.ReadMsr(MSR_IA32_THERM_STATUS, v)) return false;    // 读 0x19C
    uint32_t eax = static_cast<uint32_t>(v & 0xFFFFFFFF);        // 取低 32 位（EAX）
    if ((eax & 0x80000000) == 0) return false;                   // bit31 = 1 时读数有效
    int readout = static_cast<int>((eax >> 16) & 0x7F);          // bit[22:16] 共 7 位
    tempC = tjMax - readout;                                     // 计算差值，得到真实温度
    return true;
}

// ==================== 线程的核心切换 ====================
bool ReadTempOnCore(PawnIoAccess& pio, int tjMax, DWORD_PTR mask, int& tempC) {
    HANDLE hThread = GetCurrentThread();
    DWORD_PTR old = SetThreadAffinityMask(hThread, mask);    // 把当前线程临时绑定到指定逻辑 CPU
    if (old == 0) return false;                              // 切换失败
    bool ok = ReadCoreTemp(pio, tjMax, tempC);               // 在目标核上读
    SetThreadAffinityMask(hThread, old);                     // 恢复原亲和性
    return ok;
}

// ==================== AMD 17h+ 温度计算 ====================
class PciMutex {                                      // 读取 SMN 需要持有全局 PCI 互斥体，避免与其它硬件监控软件冲突
public:
    HANDLE hMutex = nullptr;

    // CreateMutexW 只做一次（或按需），不每周期销毁重建
    bool EnsureCreated() {
        if (hMutex) return true;
        hMutex = CreateMutexW(nullptr, FALSE, L"Global\\Access_PCI");
        return hMutex != nullptr;
    }

    // 支持停止事件：WaitForMultipleObjects 同时等互斥体和停止事件
    bool Acquire(HANDLE hStopEvent, DWORD timeoutMs) {
        if (!EnsureCreated()) return false;           // 一旦 hStopEvent 有信号，立即返回 false，不再死等 smnTimeout 到期

        HANDLE handles[2] = { hMutex, hStopEvent };
        DWORD  count      = (hStopEvent != nullptr) ? 2 : 1;
        DWORD  wr         = WaitForMultipleObjects(count, handles, FALSE, timeoutMs);

        if (wr == WAIT_OBJECT_0)     return true;     // 获得互斥体
        if (wr == WAIT_ABANDONED_0)  return true;     // 前任持有者崩溃，所有权已转移，可继续
        return false;                                 // WAIT_OBJECT_0 + 1 表示停止信号（调用方会检查 running 后退出），WAIT_TIMEOUT / WAIT_FAILED 都按失败处理
    }

    void Release() {
        if (hMutex) ReleaseMutex(hMutex);             // 不 CloseHandle，句柄在线程内复用
    }

    void Destroy() {
        if (hMutex) { CloseHandle(hMutex); hMutex = nullptr; }
    }

    ~PciMutex() { Destroy(); }
};

// 根据 CPU 家族/型号返回正确的 CCD 基址
static uint32_t GetCcdBase(uint32_t family, uint32_t model) {
    if (family == 0x1A && model <= 0x2F) {
        return F1AH_M00H_CCD1_TEMP;    // Turin / Shimada Peak
    }
    if (family == 0x19 && (model == 0x61 || model == 0x44)) {
        return F17H_M61H_CCD1_TEMP;    // Raphael / Granite Ridge
    }
    return F17H_M70H_CCD1_TEMP;        // 通用基址
}

// 判断 Turin
static bool IsTurinCpu(uint32_t family, uint32_t model) {
    return family == 0x1A && model <= 0x2F;
}

// 判断单个 CCD 读数是否有效
static bool IsCcdPresent(uint32_t raw, bool isTurin) {
    if (isTurin) {
        return (raw & ZEN_CCD_TEMP_VALID) != 0;    // 仅 Turin 用 bit 11
    }
    return (raw & ZEN_CCD_TEMP_MASK) > 0;          // 其他型号：低 12 位非零即认为存在
}

// 计算 CCD 温度（精度 0.001℃）
static int CalcCcdTempMilliC(uint32_t raw) {
    return static_cast<int>(raw & ZEN_CCD_TEMP_MASK) * 125 - 305000;    // 公式来自 LibreHardwareMonitor
}

// 获取一次互斥体，批量读取所有 CCD 温度后释放
static bool ReadAllCcdTemps(PawnIoAccess& pio, PciMutex& mutex, HANDLE hStopEvent,
                            const std::atomic<bool>& running,
                            uint32_t ccdBase, int ccdCount, bool isTurin, DWORD timeoutMs,
                            std::vector<int>& outMilliC) {
    outMilliC.assign(ccdCount, -1);

    if (!mutex.Acquire(hStopEvent, timeoutMs)) return false;

    bool anyValid = false;
    for (int i = 0; i < ccdCount; ++i) {
        // 每读一个 CCD 前检查停止信号：一旦 Finalize 置位，立即释放互斥体并返回
        if (!running.load()) {
            mutex.Release();
            return anyValid;                                       // 已读到的结果仍然有效，但不再继续
        }

        uint64_t raw = 0;
        uint32_t addr = ccdBase + static_cast<uint32_t>(i) * 4;    // 直接使用 ccdBase
        if (!pio.ReadSmn(addr, raw)) continue;

        uint32_t val = static_cast<uint32_t>(raw);
        if (!IsCcdPresent(val, isTurin)) continue;                 // 判断 CCD 是否存在

        int tempMilliC = CalcCcdTempMilliC(val);                   // 温度公式
        if (tempMilliC >= 125000) continue;                        // 过滤 >= 125℃ 的无效值

        outMilliC[i] = tempMilliC;
        anyValid = true;
    }

    mutex.Release();
    return anyValid;
}

// 封装温度读取，单次互斥体获取
int ReadAmdPackageTemp(PawnIoAccess& pio, PciMutex& mutex, HANDLE hStopEvent,
                       DWORD timeoutMs, double& tempC) {
    if (!mutex.Acquire(hStopEvent, timeoutMs)) return 1;             // 互斥体获取失败

    uint64_t raw = 0;
    bool ok = pio.ReadSmn(SMN_THM_TCON_CUR_TMP, raw);
    mutex.Release();
    if (!ok) return 2;                                               // SMN 读取失败

    uint32_t val = static_cast<uint32_t>(raw);
    int      readout     = static_cast<int>((val >> 21) & 0x7FF);    // 11 位读数
    bool     rangeSelect = (val & (1u << 19)) != 0;                  // bit19 = 1 表示偏置模式

    double temp = readout / 8.0;                                     // 0.125℃ 步进
    tempC = rangeSelect ? (temp - 49.0) : temp;
    return 0;
}

// ==================== AMD 10h-16h 温度计算 ====================
static int ReadAmd10TempMilliC(PawnIoAccess& pio, PciMutex& mutex, HANDLE hStopEvent,
                               const MeasureData* m, DWORD timeoutMs) {
    if (!mutex.Acquire(hStopEvent, timeoutMs)) return -1;

    uint64_t raw = 0;
    bool ok;
    if (m->amdHasSmuTemp) {
        ok = pio.ReadSmu(AMD15H_M60H_SMU_TEMP_OFFSET, raw);       // Family 15h Model 60h/70h：温度功能移至 SMU
    } else {
        ok = pio.ReadMiscCtl(0, AMD10_REG_REPORTED_TEMP, raw);    // 其余 10h-16h：直接读 MISCCTL 0xA4
    }
    mutex.Release();
    if (!ok) return -1;

    uint32_t val = static_cast<uint32_t>(raw);

    // LibreHardwareMonitor 的检测逻辑
    bool needsOffset =
        (m->cpuFamily == 0x15 || m->cpuFamily == 0x16) &&
        ((val & 0x30000) == 0x3000);

    int readout;
    if (needsOffset && m->cpuFamily == 0x15 && (m->cpuModel & 0xF0) == 0x00) {
        readout = static_cast<int>((val >> 21) & 0x7FC);    // Family 15h Model 00h 的特殊掩码
    } else {
        readout = static_cast<int>((val >> 21) & 0x7FF);    // 其余掩码
    }

    double tempC = readout / 8.0;                           // 0.125℃ 步进
    if (needsOffset) tempC -= 49.0;                         // Family 15h/16h 且 (val & 0x30000) == 0x3000 -49℃
    tempC += m->amdLegacyOffset;                            // Family 10h AM2+ 65nm 型号的额外偏移 +21℃

    return static_cast<int>(tempC * 1000.0 + 0.5);          // 精度 0.001℃
}

// ==================== AMD 0Fh 温度计算 ====================
static int ReadAmd0FTempMilliC(PawnIoAccess& pio, PciMutex& mutex, HANDLE hStopEvent,
                               const MeasureData* m, DWORD timeoutMs) {
    if (!mutex.Acquire(hStopEvent, timeoutMs)) return -1;

    uint64_t raw = 0;
    bool ok = pio.GetThermtrip(0, 0, raw);               // K8 通常单核或双核，coreIndex 0 足够
    mutex.Release();
    if (!ok) return -1;

    uint32_t val = static_cast<uint32_t>(raw);
    int tempC = static_cast<int>((val >> 16) & 0xFF);    // ((value >> 16) & 0xFF) + offset（bits[23:16] 整数值，方法来自 LibreHardwareMonitor）
    double result = tempC + m->amdLegacyOffset;          // 上面初始化时已确定偏移
    return static_cast<int>(result * 1000.0 + 0.5);
}

// ==================== 后台采集线程 ====================
void CollectorThread(MeasureData* m) {
    PawnIoAccess pio;    // pio 离开作用域时自动 CloseHandle

    // 初始化失败就把错误码写给主线程并结束
    if (!pio.Open()) {
        m->errorCode.store(ERR_OPEN_DEVICE);
        return;
    }

    // 根据厂商选择要加载的模块
    int resourceId;
    switch (m->vendor) {
    case CpuVendor::Intel:       resourceId = IDR_INTEL_MSR;    break;
    case CpuVendor::AmdFamily17: resourceId = IDR_AMD_FAMILY17; break;
    case CpuVendor::AmdFamily10: resourceId = IDR_AMD_FAMILY10; break;
    case CpuVendor::AmdFamily0F: resourceId = IDR_AMD_FAMILY0F; break;
    default: return;
    }
    if (!pio.LoadModuleFromResource(resourceId)) {
        m->errorCode.store(ERR_LOAD_MODULE);
        return;
    }

    // Intel 路径
    if (m->vendor == CpuVendor::Intel) {
        // tjMax 写入共享结构，用临界区保护
        int tjMaxLocal = 0;
        if (!ReadTjMax(pio, tjMaxLocal)) {
            m->errorCode.store(ERR_READ_TJMAX);
            return;
        }

        EnterCriticalSection(&m->cs);
        m->tjMax = tjMaxLocal;
        LeaveCriticalSection(&m->cs);

        // 初始化共享温度数组
        auto coreMasks = GetPhysicalCoreMasks();
        if (coreMasks.empty()) {
            m->errorCode.store(ERR_ENUM_CORES);
            return;
        }

        // 防止除以 0 异常
        const size_t coreCount = coreMasks.size();
        if (coreCount == 0) {
            m->errorCode.store(ERR_ENUM_CORES);
            return;
        }

        EnterCriticalSection(&m->cs);
        m->sharedTemps.assign(coreCount, -1);
        m->physicalCoreCount = static_cast<int>(coreCount);
        LeaveCriticalSection(&m->cs);

        // 切换核心操作均摊到整轮周期里
        while (m->running) {
            int userInterval = m->collectIntervalMs.load();              // 每轮开头重新计算每核间隔，使运行中修改 CollectInterval 在下一轮生效
            int perCore = userInterval / static_cast<int>(coreCount);    // perCore = CollectInterval / 核心数（向下取整）
            if (perCore < MIN_PER_CORE_MS) perCore = MIN_PER_CORE_MS;    // 但不低于 MIN_PER_CORE_MS

            // 依次访问每个核心，Finalize 设 false 后最多等一个 perCore 就能退出
            for (size_t i = 0; i < coreCount && m->running; ++i) {
                // 切换亲和性，读第 i 个核的温度
                int t = -1;
                ReadTempOnCore(pio, tjMaxLocal, coreMasks[i], t);
                // 结果写进共享数组，让主线程读
                EnterCriticalSection(&m->cs);
                m->sharedTemps[i] = t;
                LeaveCriticalSection(&m->cs);
                // 休眠 perCore，停止信号一到立即退出
                if (WaitForSingleObject(m->hStopEvent, perCore) == WAIT_OBJECT_0) break;
            }
        }
        return;
    }

    // AMD Family 17h+ 路径
    if (m->vendor == CpuVendor::AmdFamily17) {
        auto coreMasks = GetPhysicalCoreMasks();                       // 仍枚举物理核数，用于 Update 中 CoreIndex 越界判断
        if (coreMasks.empty()) {
            m->errorCode.store(ERR_ENUM_CORES);
            return;
        }

        PciMutex mutex;
        if (!mutex.EnsureCreated()) {
            m->errorCode.store(ERR_PCI_MUTEX);                         // 互斥体创建失败
            return;
        }

        // 确认的基址和 Turin 判断
        const bool isTurin = IsTurinCpu(m->cpuFamily, m->cpuModel);
        uint32_t ccdBase = GetCcdBase(m->cpuFamily, m->cpuModel);

        const float tctlOffset = ComputeTctlOffset(m->cpuName);        // 计算 Zen 1/1+ 特定型号的 Tctl offset

        // 探测 CCD 数量时使用正确的基址
        int detectedCcdCount = 0;
        const DWORD probeTimeout = 50;                                 // 给探测一个较短的超时，避免启动时卡住
        for (int i = 0; i < MAX_CCD_COUNT; ++i) {                      // 探测 CCD 数量：从 0 开始逐个试读，直到 VALID 位不再置位
            if (!m->running.load()) break;                             // 探测循环内也响应停止信号，避免 Finalize 时在探测阶段卡住
            if (!mutex.Acquire(m->hStopEvent, probeTimeout)) break;

            uint64_t raw = 0;
            uint32_t addr = ccdBase + static_cast<uint32_t>(i) * 4;    // 直接使用 ccdBase
            bool ok = pio.ReadSmn(addr, raw);
            mutex.Release();

            if (!ok) break;
            uint32_t val = static_cast<uint32_t>(raw);
            if (!IsCcdPresent(val, isTurin)) break;                    // 该索引没有 CCD
            ++detectedCcdCount;
        }

        if (detectedCcdCount == 0) detectedCcdCount = 1;               // 探测因互斥体竞争或停止信号提前退出时，detectedCcdCount 可能为 0。这时不能误判为“没有 CCD”，应按单 CCD 处理，主循环走封装温度路径。

        const int coreCount = static_cast<int>(coreMasks.size());      // 提前取出 coreCount，供映射表使用
        EnterCriticalSection(&m->cs);
        m->sharedTemps.assign(detectedCcdCount, -1);                   // 多 CCD 时为每个 CCD 预留一个槽位；单 CCD 时仍只用一个槽位存封装温度
        m->physicalCoreCount = coreCount;                              // 使用局部 coreCount
        m->ccdCount = detectedCcdCount;                                // 记录 CCD 数量
        m->ccdOffset = ccdBase;                                        // 记录最终基址
        m->isTurinCcd = isTurin;                                       // 记录是否 Turin

        // 计算物理核心索引到 CCD 索引的映射表
        {
            int base = coreCount / detectedCcdCount;
            int rem  = coreCount % detectedCcdCount;
            m->coreToCcd.assign(coreCount, 0);
            int idx = 0;
            for (int c = 0; c < detectedCcdCount; ++c) {
                int cnt = base + (c < rem ? 1 : 0);
                for (int k = 0; k < cnt && idx < coreCount; ++k) {
                    m->coreToCcd[idx++] = c;
                }
            }
        }
        LeaveCriticalSection(&m->cs);

        while (m->running) {
            int interval = m->collectIntervalMs.load();            // 读取当前采集间隔
            DWORD smnTimeout = static_cast<DWORD>(interval);
            if (smnTimeout < 100) smnTimeout = 100;                // SMN 互斥体超时与采集节奏挂钩：不低于 100ms，不高于 SMN_MUTEX_MAX_TIMEOUT_MS，也尽量不超过 interval
            if (smnTimeout > SMN_MUTEX_MAX_TIMEOUT_MS) smnTimeout = SMN_MUTEX_MAX_TIMEOUT_MS;

            int status = 2;                                        // 默认视为 SMN 读取失败
            double packageTemp = 0.0;

            if (detectedCcdCount == 1) {                           // 单 CCD 直接走封装温度路径，跳过 CCD 遍历。
                status = ReadAmdPackageTemp(pio, mutex, m->hStopEvent, smnTimeout, packageTemp);
                if (status == 0) packageTemp += tctlOffset;        // 应用 Zen 1/1+ Tctl offset
                EnterCriticalSection(&m->cs);
                if (status == 0) m->sharedTemps[0] = static_cast<int>(packageTemp * 1000.0 + 0.5);
                else             m->sharedTemps[0] = -1;
                LeaveCriticalSection(&m->cs);
            } else {                                               // 多 CCD：批量读取所有 CCD（单次互斥体）
                std::vector<int> ccdTemps;
                bool anyCcdValid = ReadAllCcdTemps(pio, mutex, m->hStopEvent, m->running,
                                                   ccdBase, detectedCcdCount, isTurin,
                                                   smnTimeout, ccdTemps);
                if (!m->running) break;                            // 停止信号已到，不再读剩余路径

                EnterCriticalSection(&m->cs);
                if (anyCcdValid) {
                    for (int i = 0; i < detectedCcdCount; ++i) {
                        m->sharedTemps[i] = ccdTemps[i];
                    }
                    status = 0;
                } else {                                           // CCD 全部失败，回退到封装温度
                    if (!m->running) {
                        LeaveCriticalSection(&m->cs);
                        break;                                     // 回退前再检查一次 running，避免在停止过程中多做一次互斥体等待
                    }
                    LeaveCriticalSection(&m->cs);                  // 先离开，再获取互斥体读封装温度
                    status = ReadAmdPackageTemp(pio, mutex, m->hStopEvent, smnTimeout, packageTemp);
                    if (status == 0) packageTemp += tctlOffset;    // 回退到封装温度时同样应用 Tctl offset
                    if (!m->running) break;                        // 回退读取完成后，如果已在停止中，直接退出
                    EnterCriticalSection(&m->cs);
                    if (status == 0) {                             // 用封装温度填充所有槽位，避免单个 CCD 失败时显示 0
                        for (int i = 0; i < detectedCcdCount; ++i) {
                            m->sharedTemps[i] = static_cast<int>(packageTemp * 1000.0 + 0.5);
                        }
                    } else {
                        for (int i = 0; i < detectedCcdCount; ++i) m->sharedTemps[i] = -1;
                    }
                }
                LeaveCriticalSection(&m->cs);
            }

            // 错误码状态机：互斥体问题才写，且成功时复位
            if (status == 1) {
                m->errorCode.store(ERR_PCI_MUTEX);
            } else if (status == 0) {
                m->errorCode.store(ERR_NONE);
                m->lastReportedError.store(ERR_NONE);                                    // 让下一次同类错误可以重新上报
            }

            if (interval < MIN_PER_CORE_MS) interval = MIN_PER_CORE_MS;
            if (WaitForSingleObject(m->hStopEvent, interval) == WAIT_OBJECT_0) break;    // 停止信号一到立即退出
        }

        mutex.Destroy();
        return;
    }

    // AMD Family 10h-16h 路径
    if (m->vendor == CpuVendor::AmdFamily10) {
        auto coreMasks = GetPhysicalCoreMasks();
        if (coreMasks.empty()) {
            m->errorCode.store(ERR_ENUM_CORES);
            return;
        }
        const int coreCount = static_cast<int>(coreMasks.size());

        EnterCriticalSection(&m->cs);
        m->amdHasSmuTemp = (m->cpuFamily == 0x15 &&    // 判断是否走 SMU 温度路径（Family 15h Model 60h/70h）
                            ((m->cpuModel & 0xF0) == 0x60 || (m->cpuModel & 0xF0) == 0x70));
        m->amdLegacyOffset = 0.0f;                     // Family 10h-16h 的 Tctl offset 在 LibreHardwareMonitor 中可由用户自行设置，默认 0；
        m->sharedTemps.assign(1, -1);                  // 单传感器，只用一个槽位
        m->physicalCoreCount = coreCount;
        m->ccdCount = 1;                               // 视为单 CCD，Update 走单传感器路径
        LeaveCriticalSection(&m->cs);

        PciMutex mutex;
        if (!mutex.EnsureCreated()) {
            m->errorCode.store(ERR_PCI_MUTEX);         // 互斥体创建失败
            return;
        }

        while (m->running) {
            int interval = m->collectIntervalMs.load();
            DWORD timeout = static_cast<DWORD>(interval);
            if (timeout < 100) timeout = 100;          // 互斥体等待超时不低于 100ms
            if (timeout > SMN_MUTEX_MAX_TIMEOUT_MS) timeout = SMN_MUTEX_MAX_TIMEOUT_MS;

            int milliC = ReadAmd10TempMilliC(pio, mutex, m->hStopEvent, m, timeout);

            EnterCriticalSection(&m->cs);
            m->sharedTemps[0] = milliC;                // -1 表示本次读取失败
            LeaveCriticalSection(&m->cs);

            if (milliC >= 0) {                         // 仅当本次读取成功时才复位错误状态，防止反复写日志
                m->errorCode.store(ERR_NONE);
                m->lastReportedError.store(ERR_NONE);
            }

            if (interval < MIN_PER_CORE_MS) interval = MIN_PER_CORE_MS;
            if (WaitForSingleObject(m->hStopEvent, interval) == WAIT_OBJECT_0) break;
        }

        mutex.Destroy();
        return;
    }

    // AMD Family 0Fh 路径
    if (m->vendor == CpuVendor::AmdFamily0F) {
        auto coreMasks = GetPhysicalCoreMasks();
        if (coreMasks.empty()) {
            m->errorCode.store(ERR_ENUM_CORES);
            return;
        }
        const int coreCount = static_cast<int>(coreMasks.size());

        EnterCriticalSection(&m->cs);
        float offset = -49.0f;    // 基准偏移 -49℃
        uint32_t model = m->cpuModel;
        if (model >= 0x69 && model != 0xC1 && model != 0x6C && model != 0x7C) {
            offset += 21.0f;      // AM2+ 65nm 型号偏移条件来自 LibreHardwareMonitor：model >= 0x69 且不是 0xC1 / 0x6C / 0x7C
        }
        m->amdLegacyOffset = offset;
        m->sharedTemps.assign(1, -1);
        m->physicalCoreCount = coreCount;
        m->ccdCount = 1;          // 目前视为单 CCD，见下方注释：

        /*
        已知 K8 的 Thermtrip Status 是一个共享寄存器，通过内部核心选择位暴露 socket 内不同核心的状态，且选择位的极性因 model 而异。当前
        固定读取 socket 0 的默认核心，未实现基于 model 的核心选择逻辑，因此在双核 K8 上 CoreIndex 无法区分 Core 0 / Core 1 的温度，两
        者返回同一值。此外有报道称，AMDFamily0F 模块的 Thermtrip 功能在部分平台上可能尚未实装，届时 ReadAmd0FTempMilliC 会持续返回 -1。
        */

        LeaveCriticalSection(&m->cs);

        PciMutex mutex;
        if (!mutex.EnsureCreated()) {
            m->errorCode.store(ERR_PCI_MUTEX);
            return;
        }

        while (m->running) {
            int interval = m->collectIntervalMs.load();
            DWORD timeout = static_cast<DWORD>(interval);
            if (timeout < 100) timeout = 100;
            if (timeout > SMN_MUTEX_MAX_TIMEOUT_MS) timeout = SMN_MUTEX_MAX_TIMEOUT_MS;

            int milliC = ReadAmd0FTempMilliC(pio, mutex, m->hStopEvent, m, timeout);

            EnterCriticalSection(&m->cs);
            m->sharedTemps[0] = milliC;
            LeaveCriticalSection(&m->cs);

            if (milliC >= 0) {
                m->errorCode.store(ERR_NONE);
                m->lastReportedError.store(ERR_NONE);
            }

            if (interval < MIN_PER_CORE_MS) interval = MIN_PER_CORE_MS;
            if (WaitForSingleObject(m->hStopEvent, interval) == WAIT_OBJECT_0) break;
        }

        mutex.Destroy();
        return;
    }
}

// ==================== 错误日志上报 ====================
static void ReportErrorIfAny(MeasureData* m) {
    const int err = m->errorCode.load();
    if (err == ERR_NONE) return;
    if (err == m->lastReportedError.load()) return;    // 避免每帧 Update 都往日志刷同一句错误

    m->lastReportedError.store(err);

    // 根据错误码选中文文案
    const wchar_t* msg = L"";
    switch (err) {
    case ERR_OPEN_DEVICE:
        msg = L"[PawnTemp] 无法打开 PawnIO 设备。请确认已安装 PawnIO 驱动，"
              L"且 Rainmeter 以管理员身份运行。";
        break;
    case ERR_LOAD_MODULE:
        msg = L"[PawnTemp] 无法加载二进制模块。请联系开发者确认模块"
              L"是否与当前 PawnIO 版本兼容。";
        break;
    case ERR_UNSUPPORTED_CPU_FAMILY:
        msg = L"[PawnTemp] 不支持的 CPU 家族。本插件支持 Intel 及 "
              L"AMD 0Fh~1Ah。";
        break;
    case ERR_ENUM_CORES:
        msg = L"[PawnTemp] 无法枚举物理核心。";
        break;
    case ERR_PCI_MUTEX:
        msg = L"[PawnTemp] 无法获取 PCI 互斥体。"
              L"可能有其他硬件监控软件正在占用 SMN 总线。";
        break;
    case ERR_READ_TJMAX:
        msg = L"[PawnTemp] 无法读取 TjMax (MSR 0x1A2)。"
              L"CPU 过新或 MSR 读取被拒绝。";
        break;
    default:
        msg = L"[PawnTemp] 未知错误。";
        break;
    }
    RmLog(m->rm, LOG_ERROR, msg);    // Rainmeter 日志接口
}

// ==================== 插件主线程 ====================
PLUGIN_EXPORT void Initialize(void** data, void* rm) {
    auto* m = new MeasureData();
    InitializeCriticalSection(&m->cs);                              // 必须调，cs{} 不算真正初始化
    m->hStopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);    // 创建手动重置停止事件：初始无信号，Finalize 中 SetEvent 唤醒采集线程
    m->rm = rm;
    *data = m;                                                      // 插件私有数据槽位
}

PLUGIN_EXPORT void Reload(void* data, void* rm, double* maxValue) {
    auto* m = static_cast<MeasureData*>(data);

    // 只在第一次 Reload 时识别 CPU，并把厂商 / 家族信息写入 Rainmeter 日志
    if (!m->cpuDetected) {
        m->cpuDetected = true;
        CpuIdentity cpu = DetectCpu();
        m->vendor    = cpu.vendor;
        m->cpuFamily = cpu.family;
        m->cpuModel  = cpu.model;
        memcpy(m->cpuName, cpu.name, sizeof(m->cpuName));
        m->cpuName[48] = '\0';

        const wchar_t* vendorStr =
            (cpu.vendor == CpuVendor::Intel)       ? L"Intel" :
            (cpu.vendor == CpuVendor::AmdFamily17) ? L"AMD" :
            (cpu.vendor == CpuVendor::AmdFamily10) ? L"AMD" :
            (cpu.vendor == CpuVendor::AmdFamily0F) ? L"AMD" :
                                                     L"Unsupported";
        wchar_t logBuf[512];
        swprintf_s(logBuf, L"[PawnTemp] CPU: %s  Family: 0x%X  Model: 0x%X",
                   vendorStr, cpu.family, cpu.model);
        RmLog(rm, LOG_NOTICE, logBuf);

        if (cpu.vendor == CpuVendor::Unsupported) {
            m->errorCode.store(ERR_UNSUPPORTED_CPU_FAMILY);               // 家族不支持（错误码 3）
        }
    }

    int interval = RmReadInt(rm, L"CollectInterval", 100);                // 读 ini 里的 CollectInterval，默认 100 ms
    if (interval < MIN_PER_CORE_MS) interval = MIN_PER_CORE_MS;           // 此处下限仅防止 0 或负数，真正的限制由采集线程保证
    m->collectIntervalMs.store(interval);

    m->requestedCore.store(RmReadInt(rm, L"CoreIndex", -1));              // 读 ini 里的 CoreIndex，默认取 -1 最大值

    ReportErrorIfAny(m);

    *maxValue = (m->vendor == CpuVendor::AmdFamily17 ||
                 m->vendor == CpuVendor::AmdFamily10 ||
                 m->vendor == CpuVendor::AmdFamily0F) ? 120.0 : 110.0;    // 告诉 Rainmeter 该 Measure 上限

    if (m->vendor != CpuVendor::Unsupported && !m->running) {
        m->running = true;
        if (m->hStopEvent) ResetEvent(m->hStopEvent);                     // 启动线程前重置停止事件：若上一次 Finalize 已 SetEvent，必须 Reset 才能复用
        m->collector = std::thread(CollectorThread, m);                   // 第一次 Reload 时启动线程，后续 Reload 复用同一个线程
    }
}

PLUGIN_EXPORT double Update(void* data) {
    auto* m = static_cast<MeasureData*>(data);

    ReportErrorIfAny(m);    // 如果采集线程出错了，这里报一次

    double result = 0.0;
    EnterCriticalSection(&m->cs);

    const int index = m->requestedCore.load();

    // Intel
    if (m->vendor == CpuVendor::Intel) {
        const size_t n = m->sharedTemps.size();
        if (index == -1) {                                          // 模式：所有核最大值
            int mx = -1;
            for (int t : m->sharedTemps) if (t > mx) mx = t;
            result = (mx >= 0) ? static_cast<double>(mx) : 0.0;
        }
        else if (index == -2) {                                     // 模式：所有核平均值
            int sum = 0, count = 0;
            for (int t : m->sharedTemps) {
                if (t >= 0) { sum += t; ++count; }
            }
            result = (count > 0) ? static_cast<double>(sum) / count : 0.0;
        }
        else if (index >= 0 && static_cast<size_t>(index) < n) {    // 模式：指定核
            int t = m->sharedTemps[index];
            result = (t >= 0) ? static_cast<double>(t) : 0.0;
        }
        else {
            result = 0.0;                                           // 越界或非法值
        }
    }
    // AMD 17h+
    else if (m->vendor == CpuVendor::AmdFamily17) {
        if (m->ccdCount > 1) {                                      // 多 CCD：CoreIndex 通过 coreToCcd 映射到对应 CCD
            if (index == -1) {                                      // 模式：所有 CCD 最大值
                int mx = -1;
                for (int t : m->sharedTemps) if (t > mx) mx = t;
                result = (mx >= 0) ? (mx / 1000.0) : 0.0;
            } else if (index == -2) {                               // 模式：所有 CCD 平均值
                int sum = 0, count = 0;
                for (int t : m->sharedTemps) {
                    if (t >= 0) { sum += t; ++count; }
                }
                result = (count > 0) ? (static_cast<double>(sum) / count / 1000.0) : 0.0;
            } else if (index >= 0 && index < m->physicalCoreCount &&
                       index < static_cast<int>(m->coreToCcd.size())) {
                int ccd = m->coreToCcd[index];                      // 查表得到该物理核心所属 CCD
                if (ccd >= 0 && ccd < static_cast<int>(m->sharedTemps.size())) {
                    int t = m->sharedTemps[ccd];
                    result = (t >= 0) ? (t / 1000.0) : 0.0;
                }
            } else {
                result = 0.0;
            }
        } else {                                                    // 单 CCD：CoreIndex 直接映射物理核心索引
                                                                    // 索引 0 .. physicalCoreCount-1 均有效，统一返回封装温度（sharedTemps[0]）。
            const bool validIndex = (index == -1 || index == -2 ||
                                     (index >= 0 && index < m->physicalCoreCount));
            if (validIndex && !m->sharedTemps.empty() && m->sharedTemps[0] >= 0) {
                result = m->sharedTemps[0] / 1000.0;                // 还原为 ℃，保留 3 位小数
            } else {
                result = 0.0;
            }
        }
    }
    // AMD 0Fh-16h
    else if (m->vendor == CpuVendor::AmdFamily10 || m->vendor == CpuVendor::AmdFamily0F) {
        const bool validIndex = (index == -1 || index == -2 ||
                                 (index >= 0 && index < m->physicalCoreCount));    // 只有一个核心温度传感器，-1 / -2 / 0..physicalCoreCount-1 均返回同一值
        if (validIndex && !m->sharedTemps.empty() && m->sharedTemps[0] >= 0) {
            result = m->sharedTemps[0] / 1000.0;
        } else {
            result = 0.0;
        }
    }

    LeaveCriticalSection(&m->cs);
    return result;
}

PLUGIN_EXPORT void Finalize(void* data) {
    auto* m = static_cast<MeasureData*>(data);
    m->running = false;                                  // 通知采集线程退出
    if (m->hStopEvent) SetEvent(m->hStopEvent);          // 先 SetEvent 使采集线程从 sleep 或互斥体等待中醒来并退出
    if (m->collector.joinable()) m->collector.join();    // 等待线程真正结束
    if (m->hStopEvent) {
        CloseHandle(m->hStopEvent);
        m->hStopEvent = nullptr;
    }
    DeleteCriticalSection(&m->cs);
    delete m;
}
