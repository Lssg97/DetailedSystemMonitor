# 问题解答


#### 1. 为什么在我的电脑上，皮肤看起来很小？

　　皮肤默认尺寸为 200 x 780 像素，不同电脑显示器的像素密度不同，在小尺寸但高分辨率屏幕上显示，看起来就会很小。若您使用的 Rainmeter 版本高于 5.0，请使用 Rainmeter 自带的缩放功能调整皮肤大小；否则请打开皮肤目录下的“@Resources\Configs\Settings.inc”文件调整皮肤的缩放比例参数（如下图），此方法可能随时在后续皮肤版本中停止支持。特别注意修改后**刷新皮肤前再三检查小数点在正确位置！！**

<p align="center">
  <img src="../Images/1.webp" alt="皮肤缩放调节" width="900"/>
</p>


#### 2. 为什么我的皮肤和图片展示的不一样，感觉缺东西？

　　如果皮肤自 SYS 监测模块就出现了大片缺失，且您使用的是 Windows 7 系统，则是系统无法正确识别皮肤中包含的“Noto Sans CJK.ttc”字体造成的。遇到这种情况，请下载字体修复补丁 [DetailedSystemMonitor_v_FixFont.rmskin](../../../releases/download/v3.3/DetailedSystemMonitor_v_FixFont.rmskin)，并直接安装即可解决问题（百度网盘内也有提供下载）。

　　除了上述情况外，差别主要体现在 GPU 监控相关的内容上。皮肤会根据系统环境和硬件环境自动适应，隐藏当前环境下无法显示的内容。如：受到系统环境影响，32 位系统不能显示 CPU 温度，Windows 10 1709 以前的系统无法显示非 NVIDIA 显卡的使用率，没有独立显卡的电脑无法显示显存使用情况，等等。如果您实在觉得皮肤在您的电脑上缺少某些内容属于功能性问题，可以按照 [项目首页](../../../) 的说明发出反馈。


#### 3. 为什么使用本皮肤后，我的电脑上其它Rainmeter皮肤的动画每秒钟都会卡顿一次？

　　请确保本皮肤组成文件完整，尤其检查皮肤目录下的“@Resources\Scripts\GPUAggregator.lua”文件是否存在。


#### 4. 为什么“显示器分辨率”监控和系统设置的不一样？

　　这是因为您电脑开启了 Windows 的**显示缩放**功能，旧版本 Rainmeter 识别分辨率错误造成的。若您的系统不能安装 Rainmeter 5.0 版本，请参考下图来进行设置：打开 Rainmeter 的安装目录，鼠标右键点击 `Rainmeter.exe`,属性，兼容性，更改高 DIP 设置，勾选“替代高 DIP 缩放行为”，确定。完成操作后重启 Rainmeter 软件。

<p align="center">
  <img src="../Images/2.webp" alt="显示缩放调节" width="900"/>
</p>


#### 5. 为什么“显卡温度”显示为“Nun”或者“N/A”？

　　这是一个比较复杂的问题。最开始请先确保您的电脑正确安装了显卡驱动程序。在此基础上，请您根据自己的硬件情况，按照下方列出条件进行诊断：
 * 如果显卡温度显示为“N/A”，且您电脑只拥有 AMD 独立显卡，请检查是否安装了皮肤安装包内携带的 `AMDPlugin.dll` 插件；
 * 如果显卡温度显示为“N/A”，且您的电脑只拥有 NVIDIA 独立显卡，请检查是否安装了皮肤安装包内携带的 `NvidiaGPU.dll` 插件；
 * 如果显卡温度显示为“N/A”，且您的电脑同时拥有核心显卡和独立显卡，请确保独立显卡已经工作（一般双显卡笔记本中，只有高图形运算要求情况，如玩游戏时，才会令独立显卡工作，平时独立显卡处于关闭状态）；
 * 如果显卡温度显示为“N/A”，且您的电脑只有核心显卡，此时大概率您的 CPU 温度也没有，请看下一条问题；

如果依据上面条件诊断后您的问题依旧，或者显示为“Nun”，有小概率可能是皮肤适配性问题。请按照 [项目首页](../../../) 的说明发出反馈。


#### 6. 为什么“CPU温度”显示为“N/A”？

　　首先确保您安装皮肤时勾选了 `PawnTemp.dll` 插件，然后请检查 Rainmeter 日志确认插件报告的错误信息。**显示 CPU 温度需要安装 PawnIO 驱动且 Rainmeter 以管理员身份运行。**

　　若您能确定一定以及肯定驱动已安装且 Rainmeter 已经是以管理员身份运行了，请检查 Rainmeter 日志查看插件报告的错误信息，并按照 [项目首页](../../../) 的说明发出反馈。

　　此外，如果您使用低于 5.0 版本的 Rainmeter 且电脑用户不是系统内置的“**Administrator**”，正常情况下 Rainmeter 是无法做到以管理员身份无感开机自启动运行的。所以需要采用“**计划任务**”进行设置。设置方法：

 * 首先在电脑资源管理器地址栏输入 `shell:startup`，删除启动文件夹中的 Rainmeter 快捷方式来取消原有启动方式；
 * 确保 **Rainmeter** 正处于运行状态，使用 **`Win + R`** 调用“**运行**”窗口，输入 `PowerShell Start-Process PowerShell -Verb runAs` 调出“**管理员: Windows PowerShell**”窗口；
 * 复制下方脚本代码全文，粘贴至 **PowerShell** 窗口；

```powershell
if ( -not ([Security.Principal.WindowsPrincipal][Security.Principal.WindowsIdentity]::GetCurrent()).IsInRole([Security.Principal.WindowsBuiltInRole] "Administrator") ) { "`nPlease run `"PowerShell`" as Administrator. Please `"ENTER`" to exit." ; Read-Host | Out-Null ; Exit }
if ( $process = Get-Process -Name "Rainmeter" -ErrorAction SilentlyContinue | Select Path ) {
    $path = $process -replace '(^@\{Path=)|(}$)',''
    if ( $path -match '^$' ) { $path = "$env:ProgramFiles\Rainmeter\Rainmeter.exe" }
} else { $path = "$env:ProgramFiles\Rainmeter\Rainmeter.exe" }
if ( -not (Test-Path "$path") ) { "`n`"Rainmeter.exe`" could not be found, Please `"ENTER`" to exit." ; Read-Host | Out-Null ; Exit }
$principal = New-ScheduledTaskPrincipal -GroupId "BUILTIN\Administrators" -RunLevel Highest
$trigger = New-ScheduledTaskTrigger -AtLogon
$action = New-ScheduledTaskAction -Execute $path
$settings = New-ScheduledTaskSettingsSet -DontStopOnIdleEnd -AllowStartIfOnBatteries -DontStopIfGoingOnBatteries -ExecutionTimeLimit (New-TimeSpan -Hours 0)
$task = New-ScheduledTask -Action $action -Trigger $trigger -Settings $settings -Principal $principal -Description "Run Rainmeter as Administrator"
$null = Register-ScheduledTask -TaskName "Rainmeter" -InputObject $task
Write-Output "`nScheduled task created successfully! Please `"ENTER`" to exit." ; Read-Host | Out-Null ; Exit

```

 * 当 **PowerShell** 窗口内出现 `Scheduled task created successfully! Please "ENTER" to exit.` 提示，则创建计划任务成功（若代码未执行，请敲击 **`Enter`** 键运行）；
 * 重启电脑试验效果。

当然，若想要取消这种自启动方式，只需使用 **`Win + R`** 调用“**运行**”窗口，输入 `taskschd.msc` 调出“**任务计划程序**”窗口，在“**任务计划程序库**”中查找并删除“**Rainmeter**”计划任务即可。

*注：若 **PowerShell** 窗口内出现 `Please run "PowerShell" as Administrator. Please "ENTER" to exit.` 提示，说明 **PowerShell** 未以管理员身份运行；若 **PowerShell** 窗口内出现 `"Rainmeter.exe" could not be found, Please "ENTER" to exit.` 提示，说明 **Rainmeter** 未运行。*


#### 7. 为什么任何“显卡使用率”都显示为“0”？

　　首先确保您的系统版本在 Windows 10 1709 及以上，然后确保您的电脑显卡驱动程序模型支持 **WDDM 2.4** 及以上。请使用 **Win + R** 调用“运行”，输入 `dxdiag` 来调出“**DirectX 诊断工具**”进行查看。

<p align="center">
  <img src="../Images/6.webp" alt="DirectX"  width="700"/>
</p>


如果上述条件均满足但依旧存在问题，请确认您是否还在 Rainmeter 中加载了其它和“显卡监控”有关的皮肤。若有，尝试关闭相关皮肤，再刷新该皮肤确认情况。若问题没有解决，请按照 [项目首页](../../../) 的说明发出反馈。


#### 8. 为什么“DNS服务器”展示的地址是“网关（路由器）”的地址？

　　当您的电脑网络的“DNS 服务器”设置为了 `自动` 可能会导致这样。现在市面上绝大部分家用路由器会默认接管其局域网下设备的 DNS 设置。


#### 9. 为什么电脑刚开机，但是“开机时长”已经好几天了？

　　这是 Windows 系统的“快速启动”机制导致的。


#### 10. 为什么网络正常但无法显示“广域网IP”或者“归属地”？

　　由于“广域网 IP”或者“归属地”信息从互联网相关网站进行捕获，若网站进行改版，可能会导致皮肤无法正确获取信息。如果您不了解 Rainmeter 皮肤配置文件的语法，遇到这种情况，除了等待下次皮肤更新外没有其他办法（我们会尽可能在第一时间进行修复性更新）。

　　还有一种情况就是您可能在短时间内过于频繁的刷新了皮肤，刷新皮肤会使皮肤重新向网站服务器发送 get 请求，短时间内的多次请求会被服务器判定为恶意攻击，从而遭到屏蔽。等待一段时间后便会恢复正常。

