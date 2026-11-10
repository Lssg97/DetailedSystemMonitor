-- =============================================================
-- GPU Monitoring Optimizer v6
-- Author: Lssg97
-- Developed and modified using DeepSeek AI on November 9, 2026.
-- =============================================================



-- 定义 GPU 引擎类型白名单
local ENGINES = {
    "3D", "Copy", "VideoEncode", "VideoDecode", "VideoProcessing", "LegacyOverlay", "GDIRender", "Compute", "Security", "Graphics", "VR", "Cuda"
}


-- 去除 "GDI Render" 的空格，匹配为白名单中的标准引擎类型
local function normalizeEngine(raw)
    if not raw then return nil end
    local key = raw:gsub("%s+", "")
    for _, e in ipairs(ENGINES) do
        if e:lower() == key:lower() then return e end
    end
    return nil
end

local function extractEngine(instName)
    return normalizeEngine(instName:match("engtype_(.+)$"))
end


-- 初始化 "Lua_…" 变量信息
local function initVariables()
    local function setv(n, v) SKIN:Bang("!SetVariable", n, v) end
    setv("Lua_Int_Total", "0"); setv("Lua_Dis_Total", "0")
    setv("Lua_TopGPU_Name", "System"); setv("Lua_TopGPU_Percent", "0")
    for _, e in ipairs(ENGINES) do
        for _, k in ipairs({"Revise", "Bar", "BarAdd"}) do
            setv("Lua_Int_" .. e .. "_" .. k, "0")
            setv("Lua_Dis_" .. e .. "_" .. k, "0")
        end
        setv("Lua_Int_" .. e .. "_TopName", "N/A")
        setv("Lua_Dis_" .. e .. "_TopName", "N/A")
    end
end


-- 重绑定 Meter 数据源（对 Meter 输出的 GPU 相关 Measure）为 "Lua_…" 变量
local function rebindCalc(name, var)
    SKIN:Bang("!SetOption", name, "Formula", "#*" .. var .. "*#")
    SKIN:Bang("!SetOption", name, "DynamicVariables", 1)
end

local function rebindString(name, var)
    SKIN:Bang("!SetOption", name, "String", "#*" .. var .. "*#")
    SKIN:Bang("!SetOption", name, "DynamicVariables", 1)
end

local function rebindAll()
    -- 重绑定 Measure…GPUTotalRevise
    rebindCalc("MeasureIntegratedGPUTotalRevise", "Lua_Int_Total")
    rebindCalc("MeasureDiscreteGPUTotalRevise",   "Lua_Dis_Total")
    for i, e in ipairs(ENGINES) do
	    -- 重绑定 Measure…GPU…TotalRevise、Measure…GPU…TotalBar 和 Measure…GPU…TotalBarAdd
        rebindCalc("MeasureIntegratedGPU" .. e .. "TotalRevise", "Lua_Int_" .. e .. "_Revise")
        rebindCalc("MeasureIntegratedGPU" .. e .. "TotalBar",    "Lua_Int_" .. e .. "_Bar")
        if i > 1 then
            rebindCalc("MeasureIntegratedGPU" .. e .. "TotalBarAdd", "Lua_Int_" .. e .. "_BarAdd")
        end
        rebindCalc("MeasureDiscreteGPU" .. e .. "TotalRevise", "Lua_Dis_" .. e .. "_Revise")
        rebindCalc("MeasureDiscreteGPU" .. e .. "TotalBar",    "Lua_Dis_" .. e .. "_Bar")
        if i > 1 then
            rebindCalc("MeasureDiscreteGPU" .. e .. "TotalBarAdd", "Lua_Dis_" .. e .. "_BarAdd")
        end
		-- 重绑定 Measure…GPU…Revise
        rebindString("MeasureIntegratedGPU" .. e .. "Revise", "Lua_Int_" .. e .. "_TopName")
        rebindString("MeasureDiscreteGPU" .. e .. "Revise",   "Lua_Dis_" .. e .. "_TopName")
    end
    -- 重绑定 MeasureGPUTopRevise 和 MeasureGPUTopPercentual
    rebindString("MeasureGPUTopRevise",    "Lua_TopGPU_Name")
    rebindCalc  ("MeasureGPUTopPercentual", "Lua_TopGPU_Percent")
end


-- 计算某显卡各引擎总占用（替代 Measure…GPU…Total），获得一份该显卡的引擎使用率字典
local function aggregate(items, luid, engines)
    local data = {}
    for _, e in ipairs(engines) do data[e] = 0 end
    if not luid or luid == "" then return data end
    for _, item in ipairs(items) do
        if item.name:find(luid, 1, true) then
            local eng = extractEngine(item.name)
            if eng and data[eng] then data[eng] = data[eng] + item.val end
        end
    end
    return data
end


-- 用于绘制 Bar Meter 的其余计算操作
local function writeGPUData(prefix, data, engines)
    -- 根据字典，计算各显卡总占用（替代 Measure…GPUTotal），写入 "Lua_…_Total" 变量
    local total = 0
    for _, e in ipairs(engines) do total = total + data[e] end
    local totalRevise = math.min(total, 100)
    SKIN:Bang("!SetVariable", "Lua_" .. prefix .. "_Total", string.format("%.2f", totalRevise))
	-- 根据字典，将各引擎总占用写入 "Lua_…_…_Revies" 变量
    for _, e in ipairs(engines) do
        SKIN:Bang("!SetVariable", "Lua_" .. prefix .. "_" .. e .. "_Revise",
                  string.format("%.2f", math.min(data[e], 100)))
    end
	-- 计算 Bar Meter 所需数据（替代 Measure…GPU…TotalBar 和 Measure…GPU…TotalBarAdd），写入 "Lua_…_…_Bar" 和 "Lua_…_…_BarAdd" 变量
    if total > 0 then
        local bar, running = {}, 0
        for _, e in ipairs(engines) do bar[e] = totalRevise * data[e] / total end
        for _, e in ipairs(engines) do
            running = running + bar[e]
            SKIN:Bang("!SetVariable", "Lua_" .. prefix .. "_" .. e .. "_Bar",
                      string.format("%.2f", bar[e]))
            SKIN:Bang("!SetVariable", "Lua_" .. prefix .. "_" .. e .. "_BarAdd",
                      string.format("%.2f", running))
        end
    else
        for _, e in ipairs(engines) do
            SKIN:Bang("!SetVariable", "Lua_" .. prefix .. "_" .. e .. "_Bar", "0")
            SKIN:Bang("!SetVariable", "Lua_" .. prefix .. "_" .. e .. "_BarAdd", "0")
        end
    end
end


-- 最大占用进程 PID 的匹配
local function writeTopNamesByEngine(items, luid, engines, prefix, pidToName)
    -- 生成一份某显卡各引擎的最大占用进程 PID 和占用率的空字典
    local topByEngine = {}
    for _, e in ipairs(engines) do topByEngine[e] = { pid = "", val = 0 } end
    if luid and luid ~= "" then
        for _, item in ipairs(items) do
            if item.name:find(luid, 1, true) then
                local pid = item.name:match("pid_(%d+)")
                local eng = extractEngine(item.name)
				-- 若当前 item（来自主函数的 items 列表）占用率大于字典中已存在的，就替换字典中的原 PID 和占用率信息（替代 MeasureNA）
                if pid and eng and topByEngine[eng] and item.val > topByEngine[eng].val then
                    topByEngine[eng] = { pid = pid, val = item.val }
                end
            end
        end
    end
    for _, e in ipairs(engines) do
        local top  = topByEngine[e]
        local name = "N/A"
		-- 若当前引擎存在最大占用进程 PID 且能在 pidToName 字典（主函数中生成）查询到进程名，就写入 "Lua_…_…_TopName" 变量（替代 Measure…GPU…）
        if top.pid ~= "" and pidToName[top.pid] then name = pidToName[top.pid] end
        SKIN:Bang("!SetVariable", "Lua_" .. prefix .. "_" .. e .. "_TopName", name)
    end
end



-- 皮肤加载初始化主函数
function Initialize()
    -- 脚本生成的皮肤变量的初始化
    initVariables()
    -- 定义重型计算 Measure 白名单（需要 Disable 的 GPU 相关 Measure）
    local heavy = { "MeasureNA", "MeasureIntegratedGPUTotal", "MeasureDiscreteGPUTotal", "MeasureGPUTop" }
    local function addAll(prefix)
        for _, e in ipairs(ENGINES) do
            table.insert(heavy, prefix .. e)
            table.insert(heavy, prefix .. e .. "Total")
        end
    end
    addAll("MeasureIntegratedGPU")
    addAll("MeasureDiscreteGPU")
    -- 禁用重型计算 Measure
    for _, name in ipairs(heavy) do
        SKIN:Bang("!DisableMeasure", name)
    end
    -- 禁用 MeasureProcessN 的 IfMatch 正则匹配（属于数据源 Measure，不能直接 Disable）
    for i = 1, 20 do
        local m = "MeasureProcess" .. i
        SKIN:Bang("!SetOption", m, "IfMatch", "")
        SKIN:Bang("!SetOption", m, "IfMatchAction", "")
        for j = 2, 23 do
            SKIN:Bang("!SetOption", m, "IfMatch" .. j, "")
            SKIN:Bang("!SetOption", m, "IfMatchAction" .. j, "")
        end
        SKIN:Bang("!UpdateMeasure", m)
    end
    -- 重绑定 Meter 数据源到脚本生成的皮肤变量
    rebindAll()
end


-- 皮肤更新执行主函数
function Update()
    -- 获取显卡 LUID
    local intLUID = SKIN:GetVariable("IntegratedGPUAdapter")
    local disLUID = SKIN:GetVariable("DiscreteGPUAdapter")
    if (not intLUID or intLUID == "") and (not disLUID or disLUID == "") then return end
    -- 根据 MeasureCPUProcessN 和 MeasurePIDN 生成一份 PID 对应进程名的字典
    local pidToName = {}
    for i = 1, 20 do
        local nameM  = SKIN:GetMeasure("MeasureCPUProcess" .. i)
        local pidM   = SKIN:GetMeasure("MeasurePID" .. i)
        local name   = nameM and nameM:GetStringValue() or ""
        local pidVal = pidM and pidM:GetValue() or 0
        if name ~= "" and pidVal and pidVal > 0 then
		    -- pidVal 整数化
            local pidStr = string.format("%d", math.floor(pidVal))
            pidToName[pidStr] = name:gsub("#%d+$", "")
        end
    end
    -- 根据 MeasureProcessN 生成一份包含 GPU 引擎占用进程的实例名和占用率的对象列表
    local items = {}
    for i = 1, 20 do
        local m = SKIN:GetMeasure("MeasureProcess" .. i)
        local instName = m:GetStringValue()
        local val      = m:GetValue()
        if instName and instName ~= "" and instName ~= "N/A" and instName:find("engtype_") and val and val > 0 then
            table.insert(items, { name = instName, val = val })
        end
    end
    -- 从 items 列表中选出占用率最大进程 PID
    local maxVal, topPID = 0, ""
    for _, item in ipairs(items) do
        if item.val > maxVal then
            maxVal = item.val
            topPID = item.name:match("pid_(%d+)") or ""
        end
    end
	-- 全局 GPU 最大占用进程名匹配，写入 "Lua_TopGPU_Name" 和 "Lua_TopGPU_Percent" 变量
    local topName = "System"
    if topPID ~= "" and pidToName[topPID] then topName = pidToName[topPID] end
    SKIN:Bang("!SetVariable", "Lua_TopGPU_Name", topName)
    SKIN:Bang("!SetVariable", "Lua_TopGPU_Percent", string.format("%.0f", math.min(maxVal, 100)))
	-- Bar Meter 相关计算
    writeGPUData("Int", aggregate(items, intLUID, ENGINES), ENGINES)
    writeGPUData("Dis", aggregate(items, disLUID, ENGINES), ENGINES)
	-- ToolTip 相关匹配
    writeTopNamesByEngine(items, intLUID, ENGINES, "Int", pidToName)
    writeTopNamesByEngine(items, disLUID, ENGINES, "Dis", pidToName)
end

