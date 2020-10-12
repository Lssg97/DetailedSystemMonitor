详细系统监控 v 3.3
========

[![LICENSE](https://img.shields.io/badge/license-CC%20BY--NC--SA%203.0-orange.svg?logo=creative-commons&logoColor=white)](LICENSE.md)

**详细系统监控**（DetailedSystemMonitor，以下均简称为“该皮肤”）是一款全面、直观、高信息密度的，用于监视所在计算机的网络、CPU、GPU、内存、硬盘等相关信息的 [Rainmeter](https://www.rainmeter.net/) 皮肤，旨在将这些系统信息清晰明了地展示在你的桌面上。能兼容市面上绝大多数使用Windows系统的计算机或工作站，并自动适配系统版本及硬件以只展示可以获取的信息。

![详细系统监控 v3.0](https://raw.githubusercontent.com/Lssg97/DetailedSystemMonitor/master/Images/0.png)

该皮肤目前最新版本为 **3.3**。该版本新增了对 Intel CPU 温度的监测；优化了部分内容监测方式和UI界面，网速监测的默认方式由“带宽”变更为“速度”；修复一些已知问题。详细内容请查看 [**更新笔记**](https://github.com/Lssg97/DetailedSystemMonitor/blob/master/Documents/History.md)。


### 运行要求

*  **系统要求：** **Microsoft Windows 7 SP1** 或更高版本系统，若要获得皮肤的全部功能，则需要 **Microsoft Windows 10 1709** 或更高版本系统；

*  **环境要求：** [**.NET Framework 4.5**](https://www.microsoft.com/zh-cn/download/details.aspx?id=30653) 或更高版本；

*  **软件要求：** [**Rainmeter 4.3 r3266**](https://www.rainmeter.net/) 或更高版本；

*  **其它要求：** 对于 **Intel平台计算机** 用户若想在皮肤中正确获取 **CPU温度**，Rainmeter 必须 **以管理员身份运行**。仅有AMD核心显卡的计算机若无法正常显示 GPU 温度，还需要后台运行 [**CoreTemp**](https://www.alcpu.com/CoreTemp/) 应用。


### 安装方法

本皮肤的官方发行版会使用 Rainmeter 自带皮肤打包器进行打包，安装包扩展名为 `.rmskin`。已经拥有 Rainmeter 软件的用户可在此仓库直接 [**下载**](https://github.com/Lssg97/DetailedSystemMonitor/releases/download/v3.2/DetailedSystemMonitor_v_3.2.rmskin) 最新的版本安装包到您的计算机，直接双击文件安装。

*  安装皮肤时，请注意选择适合您计算机显卡设备的插件：若您的计算机存在 **NVIDIA** 显卡，请在插件栏中只勾选 `NvidiaGPU.dll`（如下图所示）；**AMD** 显卡同理。若您的计算机并非 **Intel** 平台，请取消勾选 `CPUTemp.dll`。

<p align="center"><img src="https://raw.githubusercontent.com/Lssg97/DetailedSystemMonitor/master/Images/3.png"></p>

*  使用上述方法在默认情况下，新版本将自动覆盖旧版本的皮肤。若您想保留旧版本皮肤，请手动修改旧版本皮肤的文件名或皮肤文件所在目录名称，亦或者使用第三方压缩软件来解压新版本皮肤安装包，手动提取新版本皮肤文件（不推荐）。


### 更新历史

您可以在 [**更新笔记**](https://github.com/Lssg97/DetailedSystemMonitor/blob/master/Documents/History.md) 中查看历史更新记录。如果您想实时了解我最近的更新内容，可以查看本仓库的 [**develop**](https://github.com/Lssg97/DetailedSystemMonitor/tree/develop) 分支。


### 遇到问题

如果您在使用该皮肤中遇到任何功能性问题，请优先参考 [**问题解答**](https://github.com/Lssg97/DetailedSystemMonitor/blob/master/Documents/Q&A.md) 文档。这里回答了一些可能最常见的问题。如果您的问题不在此列，可以在该皮肤所在的 [**中国雨滴美化社区**](https://bbs.rainmeter.cn/thread-290033-1-1.html) 或 [rainmeter百度贴吧](https://tieba.baidu.com/p/6868066630)回帖反馈，亦或直接在本仓库发出 [**Issue**](https://github.com/Lssg97/DetailedSystemMonitor/issues/new) 进行提问。


### 参与维护

如果您对本皮肤项目感兴趣，有关于本皮肤的更好意见或想法且愿意将你的想法分享给其他人，那么欢迎您加入本皮肤的维护过程。您可以根据自身情况在以下方式中选择一种参与维护：

* 如果您只是一名很普通的 Rainmeter 用户，不太了解 Rainmeter 皮肤的工作方式和文件构成，只是单纯的在使用上有更好的建议或者需求，那么可以在该皮肤所在的 [**中国雨滴美化社区**](https://bbs.rainmeter.cn/thread-290033-1-1.html) 回帖或直接在本仓库发出 [**Issue**](https://github.com/Lssg97/DetailedSystemMonitor/issues/new) 提出。本人会视情况将您的建议或需求添加到下一次的版本更新中。

* 如果您已经了解该皮肤的工作方式和文件构成，已经修改了该皮肤的副本，但是不太了解 GitHub 的团队工作方式，可以通过下面的 [**联系作者**](https://github.com/Lssg97/DetailedSystemMonitor#%E8%81%94%E7%B3%BB%E4%BD%9C%E8%80%85) 发送你的皮肤修改版本并附加修改内容说明。

* 如果您已经了解该皮肤的工作方式和文件构成，同时还熟悉 Git 和 GitHub 的团队工作方式，可以选择直接 **Fork** 本项目仓库，建立您自己的项目仓库并进行修改。通过 [**Pull requests**](https://github.com/Lssg97/DetailedSystemMonitor/compare) 来参与到该皮肤的维护当中。

* 如果您并无问题需要反馈或者有功能上的建议，但愿意支持我们，也可以考虑点击仓库页面左上角的 **❤Sponsor** 适当捐助，十分感谢您的慷慨。

### 相关协议

该皮肤的 **本体文件**（.ini）、**安装包**（.rmskin）以及该皮肤项目所在本 [**GitHub仓库**](https://github.com/Lssg97/DetailedSystemMonitor) 的所有文本内容除另有声明外，均在 [**知识共享 (Creative Commons) 署名-非商业性使用-相同方式共享 3.0**](https://github.com/Lssg97/DetailedSystemMonitor/blob/master/LICENSE.md) 协议下提供，附加条款亦可能应用。

本仓库内容归 **全体编辑者** 共同所有，在本仓库里发布内容即表示您允许将您编辑的内容 **无偿且自由** 地分享给其他编辑者。


### 其他声明

目前该皮肤及后续更新的官方发布渠道仅为本 [GitHub仓库](https://github.com/Lssg97/DetailedSystemMonitor)；相关消息的官方发布渠道包括：[中国雨滴美化社区](https://bbs.rainmeter.cn/thread-290033-1-1.html) 和 [rainmeter百度贴吧](https://tieba.baidu.com/p/6868066630)。

该皮肤的初始版本源自互联网，作者、创作时间等信息均已无从考证。本人于2010年获得了该皮肤初始版本的副本，自2015年开始致力于维护该皮肤，并在 [中国雨滴美化社区](https://bbs.rainmeter.cn/) 发布过七次独立的版本更新（<https://bbs.rainmeter.cn/thread-141896-1-1.html>）。维护至今，该皮肤在构思上已经和早期版本没有任何关联，属于原创作品。不过为了致敬早期版本创作者，在名称与版本上将继续延续早期版本。

该皮肤使用了三个个其他作者制作的第三方 Rainmeter 插件：AMDTemperature.dll（[z100s](https://tieba.baidu.com/p/2221203992)）、NvidiaGPU.dll（[IZZZZZI](https://tieba.baidu.com/p/3242119308)）和 CPUTemp.dll（[DeadUniverse](https://tieba.baidu.com/p/6855002141)）；GPU Bar 的配色方案来自 [mzflh](https://tieba.baidu.com/home/main/?un=mzflh)。感谢支持和帮助。


### 联系作者

QQ：[760265690](https://wpa.qq.com/msgrd?v=3&uin=760265690)

邮箱：lssg97@163.com

