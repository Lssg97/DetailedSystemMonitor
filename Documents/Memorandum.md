## 对 GPU 引擎占用情况的检测逻辑

1. 基于一般 GPU 占用大的程序 CPU 占用都大这个逻辑，先用 `[MeasureCPUProcess1]` 到 `[MeasureCPUProcess20]` 获取 CPU 占用最大的 20 个进程，然后用 `[MeasurePID1]` 到 `[MeasurePID20]` 获取到它们的 PID，以便后续 GPU 监测部分中获得的 PID 和进程名称能匹配起来；

2. 接下来使用第三方插件 **DeviceInfo.dll** 从设备管理器中获取所有 GPU 的名称 `[MeasureGPUName1]` 到 `[MeasureGPUName3]`，并根据包含的关键字判断电脑使用核显还是独显亦或都有，以及哪个显卡厂商。因为这对后面显卡 LUID 的匹配和逻辑判断（如启用哪款插件？是否监测显存？）至关重要。这里干了两件事：一是将关键词输入 *IntegratedGPUName*、*DiscreteGPUName* 变量并决定是否启用后面的相关 Measure；二是将独显对应的“DeviceDesc”编号传递给步骤 10 中的 `[MeasureGPUDriveKey]` 用于获取独显在注册表中的信息。使用三个 Measure 是因为防止这里可能会出一些类似远程控制软件虚拟显卡或华为虚拟屏之类的东西挤掉正常的显卡信息。`[MeasureGPUNameRevise]` 主要用于 AMD 显卡识别的修正逻辑，防止核显信息误填入独显变量。`[MeasureGPUAlto]` 用于最终决策电脑是不是 Win10 系统以上？是 NVIDIA 显卡还是 AMD 卡还是核显以决定启用对应的 Measure 和插件；

3. 性能监视器中对程序的 GPU 各个引擎的占用都是类似 `pid_10628_luid_0x00000000_0x0000E14D_phys_0_eng_0_engtype_3D` 的格式。因此想要获取每个 GPU 的每个引擎占用情况就要把它们都统计进来。首先各 GPU 在这里有一个 luid，每次重启电脑都会变。先用 `[MeasureGPUAdapter0]` 到 `[MeasureGPUAdapter3]` 将独显和核显的 luid 识别出来：其中 `[MeasureGPUAdapter0]` 专用于检测独立显卡并将文本值赋值给 *DiscreteGPUAdapter* 变量，因为只有独立显卡的“Non Local Usage”数值不为 0，且一般家用电脑只有一个独立显卡。`[MeasureGPUAdapter1]` 到 `[MeasureGPUAdapter3]` 用于匹配核显。这里必定会检测到至少两个 luid，核显独显都有情况下可能会有三个以上。其中一个 luid 的“Local Usage”刚开机时数值必定为 8192，不知道为什么可能是系统保留，而剩下的要么是一个核显的 luid，要么是两个分别是核显和独显的。如果还有其他 luid 可能是前面提到的虚拟显卡但不重要（目前不知道 NPU 是不是在这里）。`[MeasureGPUAdapter1]` 和 `[MeasureGPUAdapter2]` 各自会分两步走判断获取到的文本值是否和独显 luid 匹配，不匹配先将文本值写入中间变量 *VariableGPUAdapter1*、*VariableGPUAdapter2*  再判断数值是否是 8192，不是再将文本值从中间变量转移给 *IntegratedGPUAdapter*。最后 `[MeasureGPUAdapter3]` 是保底前两个 Measure 分别为独显和核显时将系统保留的那个 luid 转移给 *??GPUAdapter* 变量，用于 `[IntegratedGPUAdapterRevise]` 修正没有核显情况下将系统保留 luid 传递给核显变量的情况；

4. 接下来的 `[MeasureProcess1]` 到 `[MeasureProcess20]` 是个大力出奇迹的产物，用于识别电脑上占用量最大的 20 个 GPU 引擎信息，它们会按照占用百分比从大到小的顺序自己排序，格式就如同上面的 `pid_10628_luid_0x00000000_0x0000E14D_phys_0_eng_0_engtype_3D`。这里每个 Measure 都做了 22 个 IfMatch 匹配，匹配它们归属的 GPU luid 和引擎名称，并将文本值和数值分别写入对应变量。因为 GPU 资源有限且倾向于独占模式，现实里不太可能开启多个程序占用 GPU 做高负载运算，因此这里只给了 20 个 Measure 检测，如果有多余的 Measure 没匹配到引擎信息，会将对应的文本值变量赋值“N/A”，数值变量赋值 0。这些引擎名称是我走访多家电脑商店，查看多款电脑的任务管理器所获取的，肯定不全但尽力了，后续会继续补充；

5. 做加法，把独显、核显对应的每个引擎数值变量相加，总数超过 100 就修正到 100，对同 GPU 的所有引擎数值再相加变成该 GPU 总使用量。因为绘制各个 GPU 引擎的 bar 是相互叠加在一起显示，且需要百分比数值，因此再做一些除法和递进加法来体现最后的重叠和多彩效果；

6. 接下来是统计最大 GPU 引擎占用的进程 PID：`[MeasureNA]` 也是个大力出奇迹：前半部分用 460 个 IfMatch 通过倒序查找方式对每个之前写入的 GPU 引擎文本值变量做匹配，当查到不是“N/A”值时候就将该变量名传递给后续的进程名匹配 Measure 替换原有变量名，这样从 20 到 1 查一遍，最后 Measure 中得到的变量名就是该引擎占用最高的那条。`[MeasureNA]` 后半部分还有 25 个 IfCondition 用于识别当前面统计的某个 GPU 引擎使用率为 0 时关闭对应进程名匹配 Measure 以减少计算量；

7. GPU 引擎最大占用 PID 匹配进程名：这些 Measure 从传递过来的变量中存储的文本值，通过正则获取到该引擎占用最大的进程 PID，再用 20 个 IfMatch 依次匹配步骤 1 中 `[MeasurePID1]` 到 `[MeasurePID20]` 获取到的 PID，匹配成功则将其进程名传递给其修正 Measure；

8. 中间穿插了从驱动获取独显 GPU 总使用率、绘制 bar 和信息弹窗、获取屏幕分辨率、显卡温度、显卡名称等内容；

9. 所有 GPU 引擎占用率最高的进程名单拎出来显示：把步骤 4 中 `[MeasureProcess1]` 统计的排第一的那个单拎出来按照步骤 7 的逻辑再匹配一遍进程名，并修正（去掉多线程进程名后面的序号）；

10. 独显显存信息，这里唯一值得说的是总显存大小的获取：该值在注册表中可获取到，位置在 `HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Control\Class\{4d36e966-e325-11ce-bfc1-08002be10318}` 的某个子项中，具体哪个子项对应独显信息？是 `[MeasureGPUDriveKey]` 通过第三方插件 **DeviceInfo.dll** 利用步骤 2 传递来的信息获取的。