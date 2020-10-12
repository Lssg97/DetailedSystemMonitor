# 更新笔记


#### 2020年10月5日

![修复](https://img.shields.io/badge/-%E4%BF%AE%E5%A4%8D-yello?style=plastic)：  修复了“显卡详细使用情况”中已关闭进程的残留问题。


#### 2020年9月23日

![修复](https://img.shields.io/badge/-%E4%BF%AE%E5%A4%8D-yello?style=plastic)：  修复了“网卡设备信息”重复显示的错误。

![删除](https://img.shields.io/badge/-%E5%88%A0%E9%99%A4-red?style=plastic)：  删除变量“TempType”。


#### 2020年9月22日

![优化](https://img.shields.io/badge/-%E4%BC%98%E5%8C%96-green?style=plastic)：  针对Intel平台计算机，舍弃使用“CoreTemp”插件，转而使用“CPUTemp”插件。

![删除](https://img.shields.io/badge/-%E5%88%A0%E9%99%A4-red?style=plastic)：  针对Intel平台计算机，删除对“句柄数”的监测。


#### 2020年9月4日

![新增](https://img.shields.io/badge/-%E6%96%B0%E5%A2%9E-blue?style=plastic)：  增加了对“CPU温度”的监测（测试，仅适用于Intel平台计算机）。插件来自<https://tieba.baidu.com/p/6855002141>。感谢插件的作者DeadUniverse。

![优化](https://img.shields.io/badge/-%E4%BC%98%E5%8C%96-green?style=plastic)：  “CPU图标”、“GPU图标”可以展示出相关平台类型。

![优化](https://img.shields.io/badge/-%E4%BC%98%E5%8C%96-green?style=plastic)：  限制了“用户名”的长度。

![优化](https://img.shields.io/badge/-%E4%BC%98%E5%8C%96-green?style=plastic)：  变更了“广域网IP”监测网站。


#### 2020年8月5日

![优化](https://img.shields.io/badge/-%E4%BC%98%E5%8C%96-green?style=plastic)：  使用系统环境变量捕获CPU逻辑核心数。

![优化](https://img.shields.io/badge/-%E4%BC%98%E5%8C%96-green?style=plastic)：  监测磁盘驱动器类型，不再展示非本地磁盘（如网络磁盘）。


#### 2020年8月2日 - Version 3.2
> 基于Rainmeter 4.3 r3266

![修复](https://img.shields.io/badge/-%E4%BF%AE%E5%A4%8D-yello?style=plastic)：  修复了“WiFi图标”无法切换信号强度的错误。


#### 2020年7月31日

![优化](https://img.shields.io/badge/-%E4%BC%98%E5%8C%96-green?style=plastic)：  重绘所有图标。

![优化](https://img.shields.io/badge/-%E4%BC%98%E5%8C%96-green?style=plastic)：  改进部分图标标签名称。

![修复](https://img.shields.io/badge/-%E4%BF%AE%E5%A4%8D-yello?style=plastic)：  修复了变更编码方式后部分字符无法显示的错误。


#### 2020年7月15日

![新增](https://img.shields.io/badge/-%E6%96%B0%E5%A2%9E-blue?style=plastic)：  增加了皮肤更新监测。


#### 2020年7月11日

![优化](https://img.shields.io/badge/-%E4%BC%98%E5%8C%96-green?style=plastic)：  优化了非“Windows 10”系统“N卡”计算机中“GPU监控”的UI间距。

![优化](https://img.shields.io/badge/-%E4%BC%98%E5%8C%96-green?style=plastic)：  当笔记本电脑外接电源且电池满电状态下，“电池监控”中显示“已充满”。


#### 2020年7月9日

![优化](https://img.shields.io/badge/-%E4%BC%98%E5%8C%96-green?style=plastic)：  将“GPU引擎”使用情况展示在“GPU Bar”上，感谢贴吧用户“mzflh”提供的配色方案。

![优化](https://img.shields.io/badge/-%E4%BC%98%E5%8C%96-green?style=plastic)：  皮肤配置文件编码方式改为“GB2312”，压缩配置文件大小。


#### 2020年7月2日

![优化](https://img.shields.io/badge/-%E4%BC%98%E5%8C%96-green?style=plastic)：  尝试性使用“继承节点”管理皮肤信息元素和Bar元素，压缩配置文件大小。


#### 2020年6月27日

![新增](https://img.shields.io/badge/-%E6%96%B0%E5%A2%9E-blue?style=plastic)：  增加了NVIDIA GPU “CUDA”引擎的监测。

![优化](https://img.shields.io/badge/-%E4%BC%98%E5%8C%96-green?style=plastic)：  修改了部分GPU引擎的名称。


#### 2020年6月9日 - Version 3.1
> 基于Rainmeter 4.3 r3266

![优化](https://img.shields.io/badge/-%E4%BC%98%E5%8C%96-green?style=plastic)：  无法获取DNS地址时显示为“N/A”。

![优化](https://img.shields.io/badge/-%E4%BC%98%E5%8C%96-green?style=plastic)：  尝试性使用“Group”管理非“Windows 10”专属皮肤节点，压缩配置文件大小。

![修复](https://img.shields.io/badge/-%E4%BF%AE%E5%A4%8D-yello?style=plastic)：  修复了存在小数缩放倍数时，“硬盘”监控模块整体上移的错误。


#### 2020年5月26日

![优化](https://img.shields.io/badge/-%E4%BC%98%E5%8C%96-green?style=plastic)：  移动了“I/O速度”的位置，微调UI界面。

![优化](https://img.shields.io/badge/-%E4%BC%98%E5%8C%96-green?style=plastic)：  重绘“CPU”，“显存”图标。

![修复](https://img.shields.io/badge/-%E4%BF%AE%E5%A4%8D-yello?style=plastic)：  修复了“网卡信息”在某些情况下突然消失的错误。


#### 2020年5月20日

![新增](https://img.shields.io/badge/-%E6%96%B0%E5%A2%9E-blue?style=plastic)：  增加了“显示时间”设置变量，用户可自主选择是否在皮肤中显示时间。

![优化](https://img.shields.io/badge/-%E4%BC%98%E5%8C%96-green?style=plastic)：  添加了信息标签。

![优化](https://img.shields.io/badge/-%E4%BC%98%E5%8C%96-green?style=plastic)：  缩减“IP地址”最大显示长度。

![优化](https://img.shields.io/badge/-%E4%BC%98%E5%8C%96-green?style=plastic)：  优化“广域网IP”的“归属地”信息刷新方式。

![优化](https://img.shields.io/badge/-%E4%BC%98%E5%8C%96-green?style=plastic)：  重绘“硬盘”，“线程”图标。


#### 2020年5月8日 - Version 3.0.b
> 基于Rainmeter 4.3 r3266

![优化](https://img.shields.io/badge/-%E4%BC%98%E5%8C%96-green?style=plastic)：  调整了“整体缩放”设置变量顺序。


#### 2020年5月7日

![新增](https://img.shields.io/badge/-%E6%96%B0%E5%A2%9E-blue?style=plastic)：  增加了“电池”监测。

![修复](https://img.shields.io/badge/-%E4%BF%AE%E5%A4%8D-yello?style=plastic)：  修复了“广域网IP归属地”监测突然失效的问题。

![修复](https://img.shields.io/badge/-%E4%BF%AE%E5%A4%8D-yello?style=plastic)：  修复了单网卡计算机上，“网卡信息”与“CPU”相关信息间空隙较大的问题。

![修复](https://img.shields.io/badge/-%E4%BF%AE%E5%A4%8D-yello?style=plastic)：  取消了“最大占用进程”中的首字母大写，避免出现“Qq”这种看起来很蠢的情况……


#### 2020年5月6日

![优化](https://img.shields.io/badge/-%E4%BC%98%E5%8C%96-green?style=plastic)：  重做“GPU”UI。

![优化](https://img.shields.io/badge/-%E4%BC%98%E5%8C%96-green?style=plastic)：  只有Intel核显的计算机会默认显示CPU温度。

![删除](https://img.shields.io/badge/-%E5%88%A0%E9%99%A4-red?style=plastic)：  删除了“虚拟内存显示方式”和“硬盘类型”设置变量。


#### 2020年5月4日

![新增](https://img.shields.io/badge/-%E6%96%B0%E5%A2%9E-blue?style=plastic)：  增加了“网卡信息”监测。

![新增](https://img.shields.io/badge/-%E6%96%B0%E5%A2%9E-blue?style=plastic)：  增加了“CPU句柄数”监测。

![优化](https://img.shields.io/badge/-%E4%BC%98%E5%8C%96-green?style=plastic)：  重做“CPU”UI。

![修复](https://img.shields.io/badge/-%E4%BF%AE%E5%A4%8D-yello?style=plastic)：  改进CPU逻辑核心数量的自动监测方式，修复了CPU逻辑核心数量自动监测报警的问题


#### 2020年4月30日

![新增](https://img.shields.io/badge/-%E6%96%B0%E5%A2%9E-blue?style=plastic)：  增加了“DNS服务器”的监测。

![优化](https://img.shields.io/badge/-%E4%BC%98%E5%8C%96-green?style=plastic)：  重做“网络”UI。


#### 2020年4月28日

![新增](https://img.shields.io/badge/-%E6%96%B0%E5%A2%9E-blue?style=plastic)：  增加了系统位数显示。

![新增](https://img.shields.io/badge/-%E6%96%B0%E5%A2%9E-blue?style=plastic)：  重新增加了“登陆时长”中的天数。

![优化](https://img.shields.io/badge/-%E4%BC%98%E5%8C%96-green?style=plastic)：  重做“系统”UI。


#### 2020年4月22日

![优化](https://img.shields.io/badge/-%E4%BC%98%E5%8C%96-green?style=plastic)：  重做“存储”UI。

#### 2020年4月19日

![优化](https://img.shields.io/badge/-%E4%BC%98%E5%8C%96-green?style=plastic)：  重做“内存”UI。

#### 2020年4月16日

![新增](https://img.shields.io/badge/-%E6%96%B0%E5%A2%9E-blue?style=plastic)：  增加了“本地时间”。


#### 2020年4月10日 - Version 3.0.a
> 基于Rainmeter 4.3 r3266

![新增](https://img.shields.io/badge/-%E6%96%B0%E5%A2%9E-blue?style=plastic)：  增加了点击“局域网IP”进入网关。

![优化](https://img.shields.io/badge/-%E4%BC%98%E5%8C%96-green?style=plastic)：  “局域网IP”监测方式交由插件负责。

![优化](https://img.shields.io/badge/-%E4%BC%98%E5%8C%96-green?style=plastic)：  “系统名称”中，“专业工作站版”中文化。

![修复](https://img.shields.io/badge/-%E4%BF%AE%E5%A4%8D-yello?style=plastic)：  修复了“广域网IP”的“归属地”信息与“广域网IP”匹配不同步的问题。

![删除](https://img.shields.io/badge/-%E5%88%A0%E9%99%A4-red?style=plastic)：  不再显示显卡的“luid”信息。


#### 2020年4月8日

![新增](https://img.shields.io/badge/-%E6%96%B0%E5%A2%9E-blue?style=plastic)：  增加了显示“广域网IP”的“归属地”。

![删除](https://img.shields.io/badge/-%E5%88%A0%E9%99%A4-red?style=plastic)：  删除了“登陆时长”中的天数，将天数累加到小时。


#### 2020年3月26日

![优化](https://img.shields.io/badge/-%E4%BC%98%E5%8C%96-green?style=plastic)：  “虚拟内存”为“分页文件”模式时，调整文字“虚拟内存”为“分页文件”。

![删除](https://img.shields.io/badge/-%E5%88%A0%E9%99%A4-red?style=plastic)：  删除了所有颜色配置中的透明度值。


#### 2020年2月22日

![修复](https://img.shields.io/badge/-%E4%BF%AE%E5%A4%8D-yello?style=plastic)：  修复了“广域网IP”监测突然失效的问题。


#### 2020年2月11日

![优化](https://img.shields.io/badge/-%E4%BC%98%E5%8C%96-green?style=plastic)：  将“CPU频率”监测由“基频”更改为“实时频率”。


#### 2020年2月6日

![新增](https://img.shields.io/badge/-%E6%96%B0%E5%A2%9E-blue?style=plastic)：  增加了对“GPU专用显存”的监测。

![优化](https://img.shields.io/badge/-%E4%BC%98%E5%8C%96-green?style=plastic)：  为了长期维护该皮肤，避免字体版权问题，将皮肤字体由“微软雅黑”改为“思源黑体”


> ### 以下为 Version 3.0 以前的公开版本更新笔记


#### 2020年4月7日

![新增](https://img.shields.io/badge/-%E6%96%B0%E5%A2%9E-blue?style=plastic)：  增加了“CPU温度”检测，由配置变量“TempType”决定显示GPU温度还是CPU温度。

![优化](https://img.shields.io/badge/-%E4%BC%98%E5%8C%96-green?style=plastic)：  提高了对“显卡设备”识别的成功度。


#### 2020年2月22日

![修复](https://img.shields.io/badge/-%E4%BF%AE%E5%A4%8D-yello?style=plastic)：  修复了“广域网IP”监测突然失效的问题。


#### 2020年1月3日 - Version 2.99
> 基于Rainmeter 4.3 r3266

![新增](https://img.shields.io/badge/-%E6%96%B0%E5%A2%9E-blue?style=plastic)：  增加了皮肤整体的自动缩放的功能（Zoom）。

![新增](https://img.shields.io/badge/-%E6%96%B0%E5%A2%9E-blue?style=plastic)：  增加了更详细的“GPU引擎使用情况”的报告（鼠标移动至“GPU 1”或者“GPU 2”图标上查看）。

![优化](https://img.shields.io/badge/-%E4%BC%98%E5%8C%96-green?style=plastic)：  加入了对“逻辑6核心”CPU的自动检测。

![优化](https://img.shields.io/badge/-%E4%BC%98%E5%8C%96-green?style=plastic)：  提高了对“显卡设备”识别的成功度。

![优化](https://img.shields.io/badge/-%E4%BC%98%E5%8C%96-green?style=plastic)：  “GPU占用最大进程”由PID改为显示名称。

![优化](https://img.shields.io/badge/-%E4%BC%98%E5%8C%96-green?style=plastic)：  “系统名称”中，“家庭版”中文化。

![优化](https://img.shields.io/badge/-%E4%BC%98%E5%8C%96-green?style=plastic)：  皮肤UI细节调整。

![修复](https://img.shields.io/badge/-%E4%BF%AE%E5%A4%8D-yello?style=plastic)：  修复了部分机型“虚拟内存”在“分页文件”模式下大小显示负数的错误。

![修复](https://img.shields.io/badge/-%E4%BF%AE%E5%A4%8D-yello?style=plastic)：  修复了“核显-独显“双显卡机型，显卡切换后才进行数据刷新的错误。


#### 2019年3月17日 - Version 2.95
> 基于Rainmeter 4.2 r3043

![新增](https://img.shields.io/badge/-%E6%96%B0%E5%A2%9E-blue?style=plastic)：  增加了“虚拟内存显示类型”配置（Default为系统默认方式，Real为分页文件）。

![新增](https://img.shields.io/badge/-%E6%96%B0%E5%A2%9E-blue?style=plastic)：  增加了对“CPU逻辑核心数量”和“显卡类别”的全自动监控。

![优化](https://img.shields.io/badge/-%E4%BC%98%E5%8C%96-green?style=plastic)：  “用户存储”部分可检测的分区名到“Z:”。

![优化](https://img.shields.io/badge/-%E4%BC%98%E5%8C%96-green?style=plastic)：  “系统名称”中，“旗舰版”、“专业版”中文化。

![优化](https://img.shields.io/badge/-%E4%BC%98%E5%8C%96-green?style=plastic)：  “用户名称”中，“管理员”、“访客”中文化。

![删除](https://img.shields.io/badge/-%E5%88%A0%E9%99%A4-red?style=plastic)：  删除了“CPU逻辑核心数量”、“显卡类型”配置。


#### 2019年2月17日 - Version 2.9

> 基于Rainmeter 4.2 r3043

![新增](https://img.shields.io/badge/-%E6%96%B0%E5%A2%9E-blue?style=plastic)：  增加了“网速监测类型”配置（Bandwidth显示带宽bps，Speed显示实时传输速度B/s）。

![新增](https://img.shields.io/badge/-%E6%96%B0%E5%A2%9E-blue?style=plastic)：  增加了“CPU逻辑核心数量”配置（该配置请务必修改为自己设备的CPU逻辑核心数量！如双核四线程就改为“4”。目前有效值只有 1，2，4，8，12，16）。

![新增](https://img.shields.io/badge/-%E6%96%B0%E5%A2%9E-blue?style=plastic)：  增加了“显卡类型”配置（该配置请务必修改为自己电脑拥有的显卡版本，这是能否成功显示GPU温度的关键配置！电脑若有双显卡均要填写，按顺序先集显后独显，用英文逗号分隔。有效值为 Intel，AMD，NVIDIA 。如笔记本电脑有集显和N卡独显，就请改为：GPUType=Intel,NVIDIA）。

![新增](https://img.shields.io/badge/-%E6%96%B0%E5%A2%9E-blue?style=plastic)：  增加了“硬盘类型”配置（决定存储部分显示的图片。有效值为SSD，HDD）。

![新增](https://img.shields.io/badge/-%E6%96%B0%E5%A2%9E-blue?style=plastic)：  增加了GPU使用率监测（正常使用的前提是Windows系统版本必须是Windows 10 1709版本及以上，如果不是Windows 10系统，本部分内容皮肤会自动隐藏不进行监测。详细内容请转https://bbs.rainmeter.cn/thread-274340-1-1.html）。

![新增](https://img.shields.io/badge/-%E6%96%B0%E5%A2%9E-blue?style=plastic)：  增加了“CPU线程数”监测。

![新增](https://img.shields.io/badge/-%E6%96%B0%E5%A2%9E-blue?style=plastic)：  增加了CPU以外其余部件“最大占用进程”监测。

![优化](https://img.shields.io/badge/-%E4%BC%98%E5%8C%96-green?style=plastic)：  整合原来“A卡”、“N卡”两个皮肤为一个文件，实现皮肤半自动化。

![优化](https://img.shields.io/badge/-%E4%BC%98%E5%8C%96-green?style=plastic)：  “CPU频率”监控方式变更为利用“UsageMonitor”插件，并将单位“MHz”改为“GHz”。

![优化](https://img.shields.io/badge/-%E4%BC%98%E5%8C%96-green?style=plastic)：  “CPU逻辑核心”的布局方式更改为双列。缩短皮肤整体长度，适应低分屏。

![优化](https://img.shields.io/badge/-%E4%BC%98%E5%8C%96-green?style=plastic)：  提高“GPU名称”捕获成功性，提高皮肤移植性。

![优化](https://img.shields.io/badge/-%E4%BC%98%E5%8C%96-green?style=plastic)：  将“虚拟内存”的监测由“提交”改为“分页文件”。

![优化](https://img.shields.io/badge/-%E4%BC%98%E5%8C%96-green?style=plastic)：  “存储”的显示方式分割为“系统存储”（C盘）和“用户存储”（D、E、F……）两部分。“用户存储”部分是除C盘外其余所有盘的大小总和，只监测到H盘。

![优化](https://img.shields.io/badge/-%E4%BC%98%E5%8C%96-green?style=plastic)：  皮肤UI位置的细小调整。

![优化](https://img.shields.io/badge/-%E4%BC%98%E5%8C%96-green?style=plastic)：  优化配置文件，使其结构清晰明确。


#### 2018年11月11日 - Version 2.6
> 基于Rainmeter 4.2 r3004

![新增](https://img.shields.io/badge/-%E6%96%B0%E5%A2%9E-blue?style=plastic)：  增加了当前计算机的“登录用户”及“登录时长”的监测。

![新增](https://img.shields.io/badge/-%E6%96%B0%E5%A2%9E-blue?style=plastic)：  “N卡”版本增加了“显卡型号”监测。

![新增](https://img.shields.io/badge/-%E6%96%B0%E5%A2%9E-blue?style=plastic)：  增加了“HDD”硬盘类型标识。

![优化](https://img.shields.io/badge/-%E4%BC%98%E5%8C%96-green?style=plastic)：  重绘所有图片UI。

![修复](https://img.shields.io/badge/-%E4%BF%AE%E5%A4%8D-yello?style=plastic)：  更换了“广域网IP”的监测网址，修复了因原网页改版导致的正则表达匹配失效的问题。


#### 2018年3月3日 - Version 2.0
> 基于Rainmeter 3.3.3 r2744

![新增](https://img.shields.io/badge/-%E6%96%B0%E5%A2%9E-blue?style=plastic)：  根据使用的新插件，将皮肤分为“N卡”版本和“A卡”版本。

![新增](https://img.shields.io/badge/-%E6%96%B0%E5%A2%9E-blue?style=plastic)：  “A卡”版本增加了“显卡型号”监测。

![新增](https://img.shields.io/badge/-%E6%96%B0%E5%A2%9E-blue?style=plastic)：  “N卡”版本增加了“显卡使用率”监测。

![新增](https://img.shields.io/badge/-%E6%96%B0%E5%A2%9E-blue?style=plastic)：  “N卡”版本中将“CPU逻辑核心数”增加到“8核心”。

![新增](https://img.shields.io/badge/-%E6%96%B0%E5%A2%9E-blue?style=plastic)：  增加了对“硬盘读写”和“C盘容量”的监测。

![新增](https://img.shields.io/badge/-%E6%96%B0%E5%A2%9E-blue?style=plastic)：  增加了“SSD”硬盘类型标识。

![优化](https://img.shields.io/badge/-%E4%BC%98%E5%8C%96-green?style=plastic)：  调整各监测项目的排列顺序使其看起来更加合理一些。

![优化](https://img.shields.io/badge/-%E4%BC%98%E5%8C%96-green?style=plastic)：  微调布局，防止UI各元素相互叠加覆盖。

![优化](https://img.shields.io/badge/-%E4%BC%98%E5%8C%96-green?style=plastic)：  调整部分检测不到数据的值从“0”变为“N/A”。

![优化](https://img.shields.io/badge/-%E4%BC%98%E5%8C%96-green?style=plastic)：  “CPU使用率”，“显卡温度”等数值过大时自动变为“红色”加以警示。

![优化](https://img.shields.io/badge/-%E4%BC%98%E5%8C%96-green?style=plastic)：  补充皮肤配置文件注释。

![修复](https://img.shields.io/badge/-%E4%BF%AE%E5%A4%8D-yello?style=plastic)：  修复不能显示GPU温度的问题。对N卡和A卡重选显卡温度监控插件，A卡插件来自<https://tieba.baidu.com/p/2221203992>，N卡插件来自<https://tieba.baidu.com/p/3242119308>。感谢两个插件的作者z100s和IZZZZZI。

![修复](https://img.shields.io/badge/-%E4%BF%AE%E5%A4%8D-yello?style=plastic)：  修复带有无线网卡的双网卡机型单独使用无线局域网络时，“本地IP”显示本地回环“127.0.0.1”的问题。

#### 2017年5月6日 - Version 1.1 ???
> 基于Rainmeter 3.3.3 r2744

完全不记得改动了什么……

