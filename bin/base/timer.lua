--
-- Created by IntelliJ IDEA.
-- User: AppleTree
-- Date: 2017/5/30
-- Time: 下午12:58
-- To change this template use File | Settings | File Templates.
--

local os = os
local rpc = require("rpc")
local log = require("log")
local log_debug = log_debug

local timer = class("timer")

rpc:addListener(timer)

function timer:onInitialize(pHandler, param)
	log_debug("timer:onInitialize", pHandler:getHandle(), param)
end

function timer:getTimerLeft(coInfo)
	return rpc:getTimerLeft(coInfo)
end
-- 计时器的单位是毫秒
function timer:schedule(callback, timeout, loop)
	local result,callbackID,coInfo = rpc:schedule(callback, timeout, loop)
	if not result then
		return
	end
	return coInfo
end
function timer:scheduleByTime(callback, year, month, day, hour, min, sec)
	local info = { year=year, month=month, day=day, hour=hour, min=min, sec=sec }
	local active_time = os.time(info)
	local current_time = os.time()
	if active_time <= current_time then
		return 0
	end
	-- 计时器的单位是毫秒
	local timeout = 1000*(active_time - current_time)
	return self:schedule(callback, timeout, false)
end
function timer:scheduleByMonth(callback, day, hour, min, sec, loop)
	local current_time = os.time()
	local active_time = self:findNextMonthActiveTime(current_time, day, hour, min, sec)
	local timeout = 1000*(active_time - current_time)
	return self:schedule(callback, timeout, loop)
end
function timer:scheduleByDay(callback, hour, min, sec, loop)
	local current_time = os.time()
	local active_time = self:findNextDayActiveTime(current_time, hour, min, sec)
	local timeout = 1000*(active_time - current_time)
	return self:schedule(callback, timeout, loop)
end
function timer:unschedule(coInfo)
	return rpc:remove(coInfo)
end
function timer:findNextDayActiveTime(current_time, hour, min, sec)
	-- 查看当天是否已经过时
	local t = os.date("*t", current_time)
	local new_t = { year=t.year, month=t.month, day=t.day, hour=hour, min=min, sec=sec }
	local active_time = os.time(new_t)
	if current_time < active_time then
		return active_time
	else
		return active_time + 24*3600
	end
end
function timer:findNextMonthActiveTime(current_time, day, hour, min, sec)
	-- 查看当月是否已经过时
	local t = os.date("*t", current_time)
	local new_t = { year=t.year, month=t.month, day=day, hour=hour, min=min, sec=sec }
	local active_time = os.time(new_t)
	if current_time < active_time then
		return active_time
	else
		local year = t.year
		local month = t.month + 1
		if month > 12 then
			year = year + 1
			month = 1
		end
		local new_t = { year=year, month=month, day=day, hour=hour, min=min, sec=sec }
		return os.time(new_t)
	end
end

return timer
