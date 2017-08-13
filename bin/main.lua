--
-- Created by IntelliJ IDEA.
-- User: AppleTree
-- Date: 2017/5/20
-- Time: 下午1:55
-- To change this template use File | Settings | File Templates.
--

--[[
class Handler
{
	unsigned int getService(void) const;
	unsigned int getType(void) const;
	unsigned int getIndex(void) const;
	unsigned int getNode(void) const;
	unsigned int getHandle(void) const;											// 获取当前的句柄

	void identifyHive(uint32 connectHandle);
	void openClient(uint32 callbackID, const char* ip, uint16 port, bool isNeedEncrypt, bool isNeedDecrypt, uint8 acceptIndex);
	void openHttpListener(uint32 callbackID, const char* ip, uint16 port);
	void openHttpsListener(uint32 callbackID, const char* ip, uint16 port);
	void openSocketListener(uint32 callbackID, const char* ip, uint16 port, bool isNeedEncrypt, bool isNeedDecrypt, uint8 acceptIndex);
	void closeListener(uint32 callbackID, uint32 listenerHandle);
	void closeConnect(uint32 callbackID, uint32 connectHandle);
	void bindAccept(uint32 acceptHandle, uint32 bindHandle);
	void sendToDestination(uint32 command, uint32 destination, const char* ptr, uint32 length);

    uint32 startTimer(uint32 callbackID, int64 timeCount);			        // 开始一个计时器
    void removeTimer(uint32 handle);								        // 移除计时器
    void changeTimer(uint32 handle, int64 timeCount);				        // 更改计时器的时间
    int64 leftTimer(uint32 handle);									        // 获取计时器剩余时间
};// end class Handler
-- ]]

local tostring = tostring
local debug = debug
local error_function = print
local print_error = function(msg, traceMsg)
	error_function("----------------------------------------")
	error_function("LUA ERROR: " .. tostring(msg) .. "\n")
	error_function(traceMsg)
	error_function("----------------------------------------")
end
local register_error
function set_register_error_function(callback)
	register_error = callback
end

-- traceback
function __G__TRACKBACK__(msg)
	local traceMsg = debug.traceback()
	print_error(msg, traceMsg)
	if register_error ~= nil then
		return register_error(msg, traceMsg)
	end
end
local __G__TRACKBACK__ = __G__TRACKBACK__
local xpcall = xpcall

local rpc

local initEnvironment = function()
	local config = require("config")
	local package = package
	package.path = "./?.lua;"
	package.cpath = "./?.so;"
	for _,p in pairs(config.path) do
		package.path = package.path .. "./"..p.."/?.lua;"
		package.cpath = package.cpath .. "./"..p.."/?.so;"
	end
	for _,p in pairs(config.path) do
		local rp = p..".init"
		require(rp)
	end
	local log = require("log")
	log.level = config.log_level or "trace"
	error_function = log_error
	rpc = require("rpc")
end

function onInitialize(pHandler, param)
	return xpcall(function()
		-- 打开Lua的日志文件
		local config = require("config")
		local nodeID = pHandler:getNode()
		local moduleType = pHandler:getModuleType()
		local moduleIndex = pHandler:getModuleIndex()
		local handle = pHandler:getHandle()
		local arr = {nodeID, moduleType, moduleIndex, handle }
		local str = table.concat(arr, "_")
		local moduleName = config.moduleName or "gamma"
		local log = require("log")
		log.open(moduleName.."_"..str..".log")

		return rpc:onInitialize(pHandler, param)
	end, __G__TRACKBACK__)
end
function onDestroy()
	return xpcall(function() return rpc:onDestroy() end, __G__TRACKBACK__)
end
function onTimerUpdate(callbackID)
	return xpcall(function() return rpc:onTimerUpdate(callbackID) end, __G__TRACKBACK__)
end
function onReceivePacket(command, destination, buffer)
	return xpcall(function() return rpc:onReceivePacket(command, destination, buffer) end, __G__TRACKBACK__)
end
function onCurlResponse(isRequestOK, callbackID, buffer)
	return xpcall(function() return rpc:onCurlResponse(isRequestOK, callbackID, buffer) end, __G__TRACKBACK__)
end
function onOpenClientOK(clientHandle)
	return xpcall(function() return rpc:onOpenClientOK(clientHandle) end, __G__TRACKBACK__)
end
function onOpenClient(callbackID, clientHandle)
	return xpcall(function() return rpc:onOpenClient(callbackID, clientHandle) end, __G__TRACKBACK__)
end
function onOpenSocketListener(callbackID, listenerHandle)
	return xpcall(function() return rpc:onOpenSocketListener(callbackID, listenerHandle) end, __G__TRACKBACK__)
end
function onOpenHttpListener(callbackID, listenerHandle)
	return xpcall(function() return rpc:onOpenHttpListener(callbackID, listenerHandle) end, __G__TRACKBACK__)
end
function onOpenHttpsListener(callbackID, listenerHandle)
	return xpcall(function() return rpc:onOpenHttpsListener(callbackID, listenerHandle) end, __G__TRACKBACK__)
end
function onCloseListener(callbackID, connectHandle)
	return xpcall(function() return rpc:onCloseListener(callbackID, connectHandle) end, __G__TRACKBACK__)
end
function onCloseConnect(callbackID, connectHandle)
	return xpcall(function() return rpc:onCloseConnect(callbackID, connectHandle) end, __G__TRACKBACK__)
end

-- 初始化环境变量配置
initEnvironment()



