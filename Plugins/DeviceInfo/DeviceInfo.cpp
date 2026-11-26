// DeviceInfo.cpp

#include <windows.h>                            // Windows 核心 API 头文件：DWORD / BOOL / MAX_PATH / LUID 等类型与 Win32 声明
#include <algorithm>                            // C++ 标准算法：std::sort / std::remove / std::replace
#include <vector>                               // C++ 标准容器：std::vector，用于存放设备列表
#include <string>                               // C++ 标准字符串：std::wstring
#include <memory>                               // C++ 智能指针：std::unique_ptr（异常安全）
#include <utility>                              // C++ 键值对：std::pair / std::make_pair
#include <cstring>                              // C 标准字符串函数：memset
#include <cstdio>                               // C 标准输入输出：swscanf_s / swprintf_s
#include <cwchar>                               // C 宽字符函数：wcscmp / _wcsicmp
#include <setupapi.h>                           // 设备安装 API 头文件：SetupDiXxx 系列函数
#include <dxgi.h>                               // DXGI API 头文件：枚举显卡适配器、读取 AdapterLuid / VendorId / DeviceId / Flags

#include "../API/Rainmeter/RainmeterAPI.h"      // Rainmeter 插件 API 头文件：RmXxx 函数、PLUGIN_EXPORT 宏

#pragma comment(lib, "setupapi.lib")            // 链接 setupapi.lib，SetupDiXxx 函数需要它
#pragma comment(lib, "dxgi.lib")                // 链接 dxgi.lib，CreateDXGIFactory1 等 DXGI 函数需要它

static const ULONGLONG REFRESH_MIN_INTERVAL_MS = 1000;   // Bang Update 两次刷新之间的最小间隔（毫秒，每皮肤节流）
static const DWORD MAX_PROP_BYTES = 64 * 1024;           // 单个设备属性允许的最大字节数（防御异常大值）

enum HardwareIdMask : DWORD                     // 硬件ID 字段有效性掩码（用于宽松匹配）
{
	MASK_VEN = 1u << 0,                         // 厂商ID 有效（VEN_XXXX）
	MASK_DEV = 1u << 1,                         // 设备ID 有效（DEV_XXXX）
	MASK_SUBSYS = 1u << 2,                      // 子系统ID 有效（SUBSYS_XXXXXXXX）
	MASK_REV = 1u << 3                          // 修订号 有效（REV_XX）
};

struct DxgiAdapterKey                           // 用于在硬件ID 与 DXGI 适配器之间比对的键
{
	DWORD VendorId;                             // 厂商ID（如 0x10DE = NVIDIA）
	DWORD DeviceId;                             // 设备ID（如 0x2484 = RTX 3070）
	DWORD SubSysId;                             // 子系统ID（同型号不同厂商/版本区分）
	DWORD Revision;                             // 修订号
	DWORD mask;                                 // 哪些字段有效（位组合，见 HardwareIdMask）
	DxgiAdapterKey() :                          // 构造函数
		VendorId(0), DeviceId(0), SubSysId(0), Revision(0), mask(0) {}   // 全部清零
};

struct DevInfoSet                               // RAII 封装：设备信息集句柄，析构时自动释放
{
	HDEVINFO h;                                 // 底层设备信息集句柄
	DevInfoSet() :                              // 构造函数
		h(SetupDiGetClassDevsW(NULL, 0, 0, DIGCF_PRESENT | DIGCF_ALLCLASSES)) {}   // 取所有“当前存在”的设备
	~DevInfoSet()                               // 析构函数
	{
		if (h != INVALID_HANDLE_VALUE)          // 句柄有效
			SetupDiDestroyDeviceInfoList(h);    // 释放设备信息集
	}
	DevInfoSet(const DevInfoSet&) = delete;     // 禁止拷贝（避免 double free）
	DevInfoSet& operator=(const DevInfoSet&) = delete;   // 禁止赋值
	bool valid() const                          // 判断句柄是否有效
	{
		return h != INVALID_HANDLE_VALUE;       // 有效返回 true
	}
};

struct DeviceInfo                               // 单个设备的信息结构
{
	std::wstring DescName;                      // 设备描述名（如 "NVIDIA GeForce RTX 3070"）
	std::wstring Drkey;                         // 驱动注册表键（SPDRP_DRIVER）
	LUID AdapterLuid;                           // 显卡适配器 LUID（仅 Display 类设备会被填充）
	bool HasLuid;                               // 是否成功从 DXGI 匹配到 LUID
	DeviceInfo() :                              // 构造函数
		DescName(),                             // 初始化 DescName 为空
		Drkey(),                                // 初始化 Drkey 为空
		AdapterLuid{ 0, 0 },                    // 初始化 LUID 为零值
		HasLuid(false) {}                       // 默认未匹配
};

struct DeviceClass                              // 设备类别：聚合同一 Class 的所有设备
{
	std::wstring Class;                         // 类别名（如 "Display"、"Net"、"USB"）
	std::vector<DeviceInfo> Desc;               // 该类下所有设备
	std::wstring num;                           // 该类下设备数量（转成字符串方便返回）
	DeviceClass() :                             // 构造函数
		Class(),                                // 初始化 Class 为空
		Desc(),                                 // 初始化 Desc 为空
		num() {}                                // 初始化 num 为空
};

enum ChildType                                  // 子测量返回类型
{
	Name,                                       // 返回设备描述名
	DriveKey,                                   // 返回驱动注册表键
	AdapterLuid                                 // 返回显卡适配器 LUID（格式 luid_0xHHHHHHHH_0xLLLLLLLL）
};

struct ChildMeasure;                            // 前置声明（ParentMeasure 需要用到指针）

struct ParentMeasure                            // 父测量：每个皮肤共享一份设备数据
{
	void* skin;                                 // Rainmeter 皮肤句柄（RmGetSkin 返回）
	ULONGLONG lastRefreshTick;                  // 本 parent 上次全量枚举设备的时刻（每皮肤节流）
	int refCount;                               // 引用计数：同皮肤下所有子测量的数量
	std::vector<DeviceClass> Devices;           // 缓存设备分类列表

	ParentMeasure() :                           // 构造函数
		skin(nullptr),                          // 初始化 skin
		lastRefreshTick(0),                     // 初始化节流时间戳
		refCount(0),                            // 初始化引用计数
		Devices()                               // 初始化 Devices 为空 vector
	{}
};

struct ChildMeasure                             // 子测量：每个 Measure=Plugin 实例一份
{
	ParentMeasure* parent;                      // 指向所属父测量
	void* rm;                                   // 本 Measure 的句柄（供 RmLog 使用，生命周期由本 Measure 保证）
	std::wstring Class;                         // INI 中配置的 Class 值（如 "Display"）
	int DeviceDesc;                             // 0 基设备索引；-1 表示无效
	ChildType type;                             // 返回类型：Name / DriveKey / AdapterLuid
	bool warnedInvalidBang;                     // 本 Measure 是否已经提示过无效 Bang（每子测量独立，避免刷屏）
	std::wstring stringCache;                   // GetString 的返回值缓冲
	std::wstring classNameCache;                // ClassName 的返回值缓冲
	std::wstring classCountCache;               // ClassCount / ClassDesc 的返回值缓冲

	ChildMeasure() :                            // 构造函数
		parent(nullptr),                        // 初始化 parent
		rm(nullptr),                            // 初始化 rm
		Class(),                                // 默认 Class 为空串
		DeviceDesc(-1),                         // 默认 DeviceDesc 无效
		type(Name),                             // 默认返回 Name
		warnedInvalidBang(false),               // 默认未提示过
		stringCache(),                          // 初始化返回值缓冲
		classNameCache(),                       // 初始化返回值缓冲
		classCountCache() {}                    // 初始化返回值缓冲
};

std::vector<ParentMeasure*> g_ParentMeasures;   // 全局父测量表（按 skin 句柄匹配，跨 Measure 共享）

std::vector<DeviceClass> getDevicesInfo(void* rmForLog);   // 前置声明：枚举设备并分类（rmForLog 用于中途出错时写日志，可为 nullptr）

static bool readDeviceProperty(HDEVINFO hDevInfo, SP_DEVINFO_DATA* data, DWORD prop, std::wstring& out, bool allowMultiSz = false)   // 读取设备注册表属性（动态分配缓冲；allowMultiSz 允许 REG_MULTI_SZ 取首段）
{
	out.clear();                                // 清空输出

	DWORD type = 0;                             // 属性值类型（REG_SZ / REG_MULTI_SZ 等）
	DWORD req = 0;                              // 属性值所需字节数
	bool firstOk = SetupDiGetDeviceRegistryPropertyW(hDevInfo, data, prop, &type, NULL, 0, &req);   // 先查询所需缓冲大小
	if (firstOk || GetLastError() != ERROR_INSUFFICIENT_BUFFER || req < sizeof(wchar_t))   // 查询成功（属性为空）、属性不存在、大小异常
		return false;                           // 读取失败
	if (req > MAX_PROP_BYTES)                   // 需求字节数异常大（系统状态异常）
		return false;                           // 拒绝，避免超大内存分配
	if (type != REG_SZ && !(allowMultiSz && type == REG_MULTI_SZ))   // 类型不匹配
		return false;                           // 视为失败

	std::vector<wchar_t> buf(req / sizeof(wchar_t) + 1, L'\0');   // 按所需大小分配宽字符缓冲（多 1 个保证结尾 L'\0'）
	if (!SetupDiGetDeviceRegistryPropertyW(hDevInfo, data, prop, &type, (PBYTE)buf.data(), (DWORD)(buf.size() * sizeof(wchar_t)), NULL))   // 读取属性值
		return false;                           // 读取失败

	size_t len = 0;                             // 实际字符长度
	while (len < buf.size() && buf[len] != L'\0')   // 找第一个字符串结尾（MULTI_SZ 也只需首段）
		len++;                                  // 继续向后扫描
	out.assign(buf.data(), len);                // 写入输出（不含结尾 L'\0'）
	return true;                                // 读取成功
}

static bool ParsePciHardwareId(const std::wstring& hwId, DxgiAdapterKey& key)   // 从 PCI 硬件ID 字符串（如 "PCI\VEN_10DE&DEV_2484&SUBSYS_...&REV_A1"）解析字段
{
	key = DxgiAdapterKey();                     // 清零结果

	unsigned int ven = 0, dev = 0, subsys = 0, rev = 0;   // 解析出的各字段（%x 需要 unsigned int*）

	int n = swscanf_s(hwId.c_str(), L"PCI\\VEN_%x&DEV_%x&SUBSYS_%x&REV_%x", &ven, &dev, &subsys, &rev);   // 尝试完整格式（4 个字段）
	if (n == 4)                                 // 全部解析成功
	{
		key.VendorId = ven;                     // 保存厂商ID
		key.DeviceId = dev;                     // 保存设备ID
		key.SubSysId = subsys;                  // 保存子系统ID
		key.Revision = rev;                     // 保存修订号
		key.mask = MASK_VEN | MASK_DEV | MASK_SUBSYS | MASK_REV;   // 全部字段有效
		return true;                            // 成功
	}

	n = swscanf_s(hwId.c_str(), L"PCI\\VEN_%x&DEV_%x&SUBSYS_%x", &ven, &dev, &subsys);   // 尝试无 REV 格式（3 个字段）
	if (n == 3)                                 // 解析成功
	{
		key.VendorId = ven;                     // 保存厂商ID
		key.DeviceId = dev;                     // 保存设备ID
		key.SubSysId = subsys;                  // 保存子系统ID
		key.mask = MASK_VEN | MASK_DEV | MASK_SUBSYS;   // 无 REV 字段
		return true;                            // 成功
	}

	n = swscanf_s(hwId.c_str(), L"PCI\\VEN_%x&DEV_%x", &ven, &dev);   // 尝试仅 VEN+DEV 格式（2 个字段）
	if (n == 2)                                 // 解析成功
	{
		key.VendorId = ven;                     // 保存厂商ID
		key.DeviceId = dev;                     // 保存设备ID
		key.mask = MASK_VEN | MASK_DEV;         // 仅 VEN+DEV 有效
		return true;                            // 成功
	}

	return false;                               // 无法识别的硬件ID
}

static std::vector<std::pair<DxgiAdapterKey, LUID>> GetDxgiAdapterInfoMap()   // 枚举所有 DXGI 适配器，构建 (硬件ID组合 → LUID) 映射表
{
	std::vector<std::pair<DxgiAdapterKey, LUID>> map;   // 结果容器

	IDXGIFactory1* factory = nullptr;           // DXGI 工厂对象（Win7 起支持）
	if (FAILED(CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void**)&factory)))   // 创建工厂失败
		return map;                             // 返回空表

	IDXGIAdapter1* adapter = nullptr;           // 当前遍历到的适配器
	for (UINT i = 0; ; ++i)                     // 逐个枚举适配器
	{
		HRESULT hr = factory->EnumAdapters1(i, &adapter);   // 枚举第 i 个适配器
		if (hr == DXGI_ERROR_NOT_FOUND)         // 枚举结束
			break;                              // 退出循环
		if (FAILED(hr))                         // 其它错误
			break;                              // 退出循环

		DXGI_ADAPTER_DESC1 desc = {};           // 适配器描述结构
		if (SUCCEEDED(adapter->GetDesc1(&desc))   // 成功获取描述
			&& !(desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE))   // 且不是软件适配器（如 Microsoft Basic Render Driver）
		{
			DxgiAdapterKey key;                 // 构造用于匹配的键
			key.VendorId = desc.VendorId;       // 厂商ID
			key.DeviceId = desc.DeviceId;       // 设备ID
			key.SubSysId = desc.SubSysId;       // 子系统ID
			key.Revision = desc.Revision;       // 修订号
			key.mask = MASK_VEN | MASK_DEV | MASK_SUBSYS | MASK_REV;   // DXGI 侧始终提供全部字段
			map.push_back(std::make_pair(key, desc.AdapterLuid));   // 记录映射：硬件ID 组合 → LUID
		}
		adapter->Release();                     // 释放适配器对象（无论是否加入映射表都要释放）
		adapter = nullptr;                      // 重置指针
	}
	factory->Release();                         // 释放工厂对象
	return map;                                 // 返回映射表
}

static bool FindAdapterLuidByHardwareId(const std::vector<std::pair<DxgiAdapterKey, LUID>>& map,   // 在映射表中按硬件ID 组合查找 LUID
                                        const DxgiAdapterKey& hwKey, LUID& outLuid)   // 输入设备侧的硬件ID，输出匹配到的 LUID
{
	if ((hwKey.mask & (MASK_VEN | MASK_DEV)) != (MASK_VEN | MASK_DEV))   // 至少需要厂商ID + 设备ID 才能可靠匹配
		return false;                           // 信息不足，拒绝匹配

	for (size_t i = 0; i < map.size(); ++i)     // 遍历 DXGI 适配器映射表
	{
		const DxgiAdapterKey& dxgiKey = map[i].first;   // 当前候选 DXGI 键
		if ((hwKey.mask & MASK_VEN) && hwKey.VendorId != dxgiKey.VendorId) continue;   // 厂商ID 不一致则跳过
		if ((hwKey.mask & MASK_DEV) && hwKey.DeviceId != dxgiKey.DeviceId) continue;   // 设备ID 不一致则跳过
		if ((hwKey.mask & MASK_SUBSYS) && hwKey.SubSysId != dxgiKey.SubSysId) continue;   // 子系统ID 不一致则跳过
		if ((hwKey.mask & MASK_REV) && hwKey.Revision != dxgiKey.Revision) continue;   // 修订号不一致则跳过
		outLuid = map[i].second;                // 写入匹配到的 LUID
		return true;                            // 返回成功
	}
	return false;                               // 未找到匹配
}

PLUGIN_EXPORT void Initialize(void** data, void* rm)   // 插件初始化：每个 Measure 首次创建时调用
{
	*data = nullptr;                                    // 先置空，异常时 Rainmeter 拿到的是空指针
	try                                                 // 捕获 C++ 异常，避免穿透到 Rainmeter 崩溃进程
	{
		std::unique_ptr<ChildMeasure> child(new ChildMeasure);   // 用 unique_ptr 管理，异常时自动释放
		child->rm = rm;                                 // 记录本 Measure 的句柄，供 RmLog 使用

		void* skin = RmGetSkin(rm);                     // 取当前皮肤句柄
		ParentMeasure* parent = nullptr;                // 查找可复用的父测量
		// 用皮肤句柄在全局表中查找已有的父测量，确保同皮肤下所有 Measure 共享同一份设备数据
		std::vector<ParentMeasure*>::const_iterator iter = g_ParentMeasures.begin();   // 遍历全局父测量表
		for (; iter != g_ParentMeasures.end(); ++iter)  // 逐个查找
		{
			if ((*iter)->skin == skin)                  // 同一皮肤下已有父测量
			{
				parent = (*iter);                       // 复用已有父测量
				break;                                  // 结束查找
			}
		}
		if (!parent)                                    // 该皮肤首个测量
		{
			std::unique_ptr<ParentMeasure> np(new ParentMeasure);   // 新建父测量（unique_ptr 管理）
			np->skin = skin;                            // 记录皮肤句柄
			np->lastRefreshTick = GetTickCount64();     // 记录刷新时刻，避免刚初始化就响应 Bang Update
			try
			{
				np->Devices = getDevicesInfo(rm);       // 首次枚举设备并缓存结果（传入 rm 供中途出错时写日志）
			}
			catch (...)                                 // 枚举失败（内存不足等）
			{
				RmLog(rm, LOG_ERROR, L"DeviceInfo: enumerate devices failed");   // 写日志
				np->Devices.clear();                    // 保证 Devices 为空而不是半成品
			}
			g_ParentMeasures.push_back(np.get());       // 加入全局列表（若抛异常，np 自动释放）
			parent = np.release();                      // 释放所有权，交给全局表管理
		}
		child->parent = parent;                         // 绑定 parent
		parent->refCount++;                             // 引用计数加一
		*data = child.release();                        // 移交所有权给 Rainmeter
	}
	catch (...)                                         // 捕获所有剩余异常
	{
		RmLog(rm, LOG_ERROR, L"DeviceInfo: initialize failed");   // 写日志（*data 保持 nullptr）
	}
}

PLUGIN_EXPORT void Reload(void* data, void* rm, double* maxValue)   // 每次刷新 INI 时调用
{
	if (!data) return;                          // Initialize 失败时 data 为 nullptr，直接返回
	ChildMeasure* child = (ChildMeasure*)data;  // 取子测量
	child->rm = rm;                             // 更新为当前有效的 Measure 句柄

	child->Class = RmReadString(rm, L"Class", L"");   // 读 Class 参数（默认空串）

	int desc = RmReadInt(rm, L"DeviceDesc", 1) - 1;   // 读 DeviceDesc（默认 1），转为 0 基索引
	child->DeviceDesc = (desc >= 0) ? desc : -1;      // 负数一律视为无效

	child->type = Name;                         // 每次 Reload 先重置为默认值，避免沿用旧配置
	LPCWSTR type = RmReadString(rm, L"type", L"");    // 读 type 参数（默认空）
	if (type && _wcsicmp(type, L"DriveKey") == 0)     // type == DriveKey
		child->type = DriveKey;                 // 返回驱动键
	else if (type && _wcsicmp(type, L"AdapterLuid") == 0)   // type == AdapterLuid
		child->type = AdapterLuid;              // 返回显卡适配器 LUID
	// 其他值（含 DescName / 空 / 未知）均保持 Name

	child->warnedInvalidBang = false;           // 重新加载 INI 后允许再次提示无效 Bang
}

PLUGIN_EXPORT double Update(void* data)         // 每次刷新时调用，返回值作为数值
{
	if (!data) return 0.0;                      // Initialize 失败时 data 为 nullptr，返回 0
	ChildMeasure* child = (ChildMeasure*)data;  // 取子测量
	ParentMeasure* parent = child->parent;      // 取父测量
	// 注意：Update 返回的是“所匹配类下所有设备的总数”，与 DeviceDesc 参数无关
	if (!parent || child->Class.empty())        // 父测量不存在或 Class 未配置
		return 0.0;                             // 返回 0

	for (size_t i = 0; i < parent->Devices.size(); i++)   // 遍历设备类
	{
		if (_wcsicmp(child->Class.c_str(), parent->Devices[i].Class.c_str()) == 0)   // 找到匹配类
			return (double)parent->Devices[i].Desc.size();   // 返回该类下设备数量
	}
	return 0.0;                                 // 没找到则 0
}

PLUGIN_EXPORT LPCWSTR GetString(void* data)     // 取字符串值时调用
{
	if (!data) return L"";                      // Initialize 失败时 data 为 nullptr，返回空串
	ChildMeasure* child = (ChildMeasure*)data;  // 取子测量
	ParentMeasure* parent = child->parent;      // 取父测量

	child->stringCache.clear();                 // 清空返回值缓冲

	if (!parent || child->Class.empty() || child->DeviceDesc < 0)   // 父测量不存在、Class 未配置、索引无效
		return child->stringCache.c_str();      // 返回空串

	for (size_t i = 0; i < parent->Devices.size(); i++)   // 遍历设备类
	{
		if (_wcsicmp(child->Class.c_str(), parent->Devices[i].Class.c_str()) == 0)   // 找到匹配类
		{
			if ((size_t)child->DeviceDesc < parent->Devices[i].Desc.size())   // 索引在范围内
			{
				const DeviceInfo& info = parent->Devices[i].Desc[child->DeviceDesc];   // 取当前设备
				switch (child->type)            // 按类型返回
				{
				case Name:                      // 设备描述名
					child->stringCache = info.DescName;   // 复制到缓冲
					break;                      // 结束
				case DriveKey:                  // 驱动键
					child->stringCache = info.Drkey;      // 复制到缓冲
					break;                      // 结束
				case AdapterLuid:               // 显卡适配器 LUID
					if (info.HasLuid)           // 成功匹配到 LUID
					{
						wchar_t luidBuf[64] = { 0 };   // 格式化缓冲
						swprintf_s(luidBuf, L"luid_0x%08X_0x%08X",   // 高 8 位 + 低 8 位分开显示
							(unsigned int)info.AdapterLuid.HighPart,  // HighPart（32 位）
							(unsigned int)info.AdapterLuid.LowPart);  // LowPart（32 位）
						child->stringCache = luidBuf;   // 写入返回值缓冲
					}
					// 未匹配到 LUID 时保持空串
					break;                      // 结束
				}
			}
			break;                              // 找到匹配类后停止遍历
		}
	}
	return child->stringCache.c_str();          // 返回缓冲指针（生命周期由子测量保证）
}

PLUGIN_EXPORT void ExecuteBang(void* data, LPCWSTR args)   // 处理 Bang 命令
{
	if (!data) return;                          // Initialize 失败时 data 为 nullptr，直接返回
	ChildMeasure* child = (ChildMeasure*)data;  // 取子测量
	ParentMeasure* parent = child->parent;      // 取父测量
	if (!parent)                                // 父测量不存在
		return;                                 // 直接返回

	if (_wcsicmp(args, L"Update") == 0)         // 命令 == Update
	{
		ULONGLONG now = GetTickCount64();       // 当前时刻
		if (now - parent->lastRefreshTick < REFRESH_MIN_INTERVAL_MS)   // 本皮肤距上次刷新太近
		{
			RmLog(child->rm, LOG_WARNING, L"DeviceInfo: Update ignored (refreshed too recently)");   // 写日志提示
			return;                             // 跳过本次刷新
		}
		parent->lastRefreshTick = now;          // 记录刷新时刻
		try
		{
			parent->Devices = getDevicesInfo(child->rm);   // 重新枚举全部设备（传入 rm 供中途出错时写日志）
		}
		catch (...)                             // 枚举失败（内存不足等）
		{
			RmLog(child->rm, LOG_ERROR, L"DeviceInfo: update failed");   // 写日志，保留旧数据
		}
		// 警告：刷新后设备顺序可能变化，DeviceDesc 索引可能已指向另一台设备，需要用户重新确认
	}
	else                                        // 未知命令
	{
		if (!child->warnedInvalidBang)          // 本子测量尚未提示过
		{
			RmLog(child->rm, LOG_ERROR, L"Invalid \"ExecuteBang\" (only logged once per measure)");   // 写日志
			child->warnedInvalidBang = true;    // 标记已提示（Reload 时重置）
		}
	}
}

PLUGIN_EXPORT void Finalize(void* data)         // 插件销毁（Measure 卸载）
{
	if (!data) return;                          // Initialize 失败时 data 为 nullptr，无需释放
	ChildMeasure* child = (ChildMeasure*)data;  // 取子测量
	ParentMeasure* parent = child->parent;      // 取父测量

	if (parent)                                 // 父测量存在
	{
		parent->refCount--;                     // 引用计数减一
		if (parent->refCount <= 0)              // 最后一个子测量
		{
			g_ParentMeasures.erase(             // 从全局表移除
				std::remove(g_ParentMeasures.begin(), g_ParentMeasures.end(), parent),   // 找到要删项
				g_ParentMeasures.end());        // 删除其后所有
			delete parent;                      // 释放父测量
		}
	}
	delete child;                               // 释放子测量
}

PLUGIN_EXPORT LPCWSTR ClassName(void* data, const int argc, const WCHAR* argv[])   // 自定义函数：返回第 N 个类的类名
{
	if (!data) return L"";                      // Initialize 失败时 data 为 nullptr，返回空串
	ChildMeasure* child = (ChildMeasure*)data;  // 取子测量
	ParentMeasure* parent = child->parent;      // 取父测量

	child->classNameCache.clear();              // 清空返回值缓冲（无效参数时也返回空串）

	if (parent && argc == 1)                    // 父测量存在且参数个数正确
	{
		int idx = -1;                           // 存放解析出的索引
		swscanf_s(argv[0], L"%d", &idx);        // 从参数解析整数（1 基）
		if (idx > 0 && (size_t)idx <= parent->Devices.size())   // 索引在范围内
			child->classNameCache = parent->Devices[idx - 1].Class;   // 复制类名到缓冲
	}
	return child->classNameCache.c_str();       // 返回缓冲指针（无效参数时为空串）
}

PLUGIN_EXPORT LPCWSTR ClassCount(void* data, const int argc, const WCHAR* argv[])   // 自定义函数：返回第 N 个类的设备数量
{
	if (!data) return L"";                      // Initialize 失败时 data 为 nullptr，返回空串
	ChildMeasure* child = (ChildMeasure*)data;  // 取子测量
	ParentMeasure* parent = child->parent;      // 取父测量

	child->classCountCache.clear();             // 清空返回值缓冲（无效参数时返回空串，与 ClassName 保持一致）

	if (parent && argc == 1)                    // 父测量存在且参数个数正确
	{
		int idx = -1;                           // 存放解析出的索引
		swscanf_s(argv[0], L"%d", &idx);        // 从参数解析整数（1 基）
		if (idx > 0 && (size_t)idx <= parent->Devices.size())   // 索引在范围内
			child->classCountCache = parent->Devices[idx - 1].num;   // 复制数量到缓冲
	}
	return child->classCountCache.c_str();      // 返回缓冲指针（无效参数时为空串）
}

PLUGIN_EXPORT LPCWSTR ClassDesc(void* data, const int argc, const WCHAR* argv[])   // 旧名函数（向后兼容，转调 ClassCount）
{
	if (!data) return L"";                      // Initialize 失败时 data 为 nullptr，返回空串
	return ClassCount(data, argc, argv);        // 直接转调新实现
}

std::vector<DeviceClass> getDevicesInfo(void* rmForLog)   // 枚举当前系统所有设备并按类分组；rmForLog 用于中途出错时写日志（可为 nullptr）
{
	std::vector<DeviceClass> Devices;           // 结果容器

	DevInfoSet devSet;                          // 设备信息集（RAII，出函数自动释放）
	if (!devSet.valid())                        // 获取失败
		return Devices;                         // 返回空列表

	std::vector<std::pair<DxgiAdapterKey, LUID>> dxgiMap;   // DXGI 适配器映射表（仅在遇到 Display 类时才用到）
	bool dxgiMapReady = false;                  // 映射表是否已构建（懒加载，避免无显卡场景下无谓枚举）

	SP_DEVINFO_DATA data = {};                  // 清零初始化，避免未初始化字段
	data.cbSize = sizeof(SP_DEVINFO_DATA);      // 设置结构大小

	for (DWORD i = 0; ; i++)                    // 手动控制循环，以区分正常结束与中途出错
	{
		if (!SetupDiEnumDeviceInfo(devSet.h, i, &data))   // 枚举返回 FALSE：正常结束或出错
		{
			DWORD err = GetLastError();         // 取错误码以区分两种情形
			if (err != ERROR_NO_MORE_ITEMS && rmForLog)   // 不是“没有更多项”= 中途出错，且有日志句柄
				RmLog(rmForLog, LOG_WARNING, L"DeviceInfo: device enumeration stopped unexpectedly");   // 写警告
			break;                              // 无论哪种情况都结束循环（已收集数据保留）
		}

		std::wstring Class;                     // 当前设备类名
		if (!readDeviceProperty(devSet.h, &data, SPDRP_CLASS, Class) || Class.empty())   // 读取类名失败或为空
			Class = L"Unknown";                 // 兜底为 "Unknown"（不丢弃设备）

		DeviceInfo Desc;                        // 当前设备描述
		if (!readDeviceProperty(devSet.h, &data, SPDRP_DEVICEDESC, Desc.DescName) || Desc.DescName.empty())   // 描述名为空
		{
			if (!readDeviceProperty(devSet.h, &data, SPDRP_FRIENDLYNAME, Desc.DescName) || Desc.DescName.empty())   // 尝试友好名
				Desc.DescName = L"(Unknown)";   // 兜底占位
		}
		readDeviceProperty(devSet.h, &data, SPDRP_DRIVER, Desc.Drkey);   // 读取驱动键（部分虚拟设备无此属性，允许为空）

		if (_wcsicmp(Class.c_str(), L"Display") == 0)   // 仅对显示类设备尝试匹配 DXGI LUID
		{
			if (!dxgiMapReady)                  // 首次遇到 Display 类时才构建映射表（懒加载，减少开销）
			{
				dxgiMap = GetDxgiAdapterInfoMap();   // 枚举所有 DXGI 适配器，构建 (硬件ID组合 → LUID) 映射
				dxgiMapReady = true;            // 标记已构建
			}
			std::wstring hardwareId;            // 存放当前设备的首个硬件ID 字符串
			if (readDeviceProperty(devSet.h, &data, SPDRP_HARDWAREID, hardwareId, true))   // 读取硬件ID（REG_MULTI_SZ，取首段）
			{
				DxgiAdapterKey hwKey;           // 从硬件ID 解析出的键
				if (ParsePciHardwareId(hardwareId, hwKey))   // 解析成功
					Desc.HasLuid = FindAdapterLuidByHardwareId(dxgiMap, hwKey, Desc.AdapterLuid);   // 在映射表中查找对应 LUID
			}
		}

		bool found = false;                     // 是否找到同类
		for (size_t j = 0; j < Devices.size(); j++)   // 查找已存在的同类
		{
			if (_wcsicmp(Class.c_str(), Devices[j].Class.c_str()) == 0)   // 类名匹配
			{
				Devices[j].Desc.push_back(Desc);   // 加入对应类
				found = true;                   // 标记找到
				break;                          // 结束查找
			}
		}
		if (!found)                             // 未找到
		{
			DeviceClass DClass;                 // 新建类
			DClass.Class = Class;               // 设置类名
			DClass.Desc.push_back(Desc);        // 加入首个设备
			Devices.push_back(DClass);          // 压入结果容器
		}
	}

	for (size_t j = 0; j < Devices.size(); j++) // 设置每个类的设备数量
	{
		Devices[j].num = std::to_wstring(Devices[j].Desc.size());   // 数量转字符串
		std::sort(Devices[j].Desc.begin(), Devices[j].Desc.end(),    // 对类内设备按名称排序
			[](const DeviceInfo& p1, const DeviceInfo& p2) {        // 比较两个设备
				return wcscmp(p1.DescName.c_str(), p2.DescName.c_str()) < 0;   // 按宽字符字典序
			});
	}
	std::sort(Devices.begin(), Devices.end(),                        // 对所有类按类名排序
		[](const DeviceClass& p1, const DeviceClass& p2) {           // 比较两个类
			return wcscmp(p1.Class.c_str(), p2.Class.c_str()) < 0;   // 按宽字符字典序
		});
	return Devices;                             // 返回分类后的设备列表
}
