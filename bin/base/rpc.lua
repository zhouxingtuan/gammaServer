--
-- Created by IntelliJ IDEA.
-- User: AppleTree
-- Date: 2017/5/25
-- Time: 下午10:00
-- To change this template use File | Settings | File Templates.
--

local coroutine = coroutine
local pairs = pairs
local log_debug = log_debug
local log_error = log_error

local class = require("class")
local eproto = require("eproto")

local rpc = class("rpc")

-- C++调用函数 Begin-------------------

function rpc:onInitialize(pHandler, param)
	log_debug("rpc:onInitialize called", pHandler:getHandle(), param)
	return self:initRpc(pHandler, param)
end
function rpc:onDestroy()
	log_debug("rpc:onDestroy called")
	return self:destroyRpc()
end
function rpc:onTimerUpdate(callbackID)
	log_debug("rpc:onTimerUpdate called", callbackID)
	return self:doingTimeout(callbackID)
end
function rpc:onReceivePacket(command, destination, buffer)
	log_debug("rpc:onReceivePacket called", command, destination, #buffer)
	if command <= 249 then
		return self:doing(destination, buffer)
	else
		if command == 252 then
			-- COMMAND_RESPONSE 连接验证成功
			log_debug("rpc:onReceivePacket command identify hive OK", command, destination)
			return self:checkIdentifyOKListener(destination)
		else
			log_debug("rpc:onReceivePacket command not handle", command, destination)
		end
	end
end
function rpc:onCurlResponse(isRequestOK, callbackID, buffer)
	log_debug("rpc:onCurlResponse called", isRequestOK, callbackID, #buffer)
	return self:doingHttp(isRequestOK, callbackID, buffer)
end
function rpc:onOpenClientOK(clientHandle)
	log_debug("rpc:onOpenClientOK called", clientHandle)
	-- 这里是连接建立成功，可以不处理
	return self:checkClientOKListener(clientHandle)
end
function rpc:onOpenClient(callbackID, clientHandle)
	log_debug("rpc:onOpenClient called", callbackID, clientHandle)
	return self:doingWithCO(callbackID, clientHandle)
end
function rpc:onOpenSocketListener(callbackID, listenerHandle)
	log_debug("rpc:onOpenSocketListener called", callbackID, listenerHandle)
	return self:doingWithCO(callbackID, listenerHandle)
end
function rpc:onOpenHttpListener(callbackID, listenerHandle)
	log_debug("rpc:onOpenHttpListener called", callbackID, listenerHandle)
	return self:doingWithCO(callbackID, listenerHandle)
end
function rpc:onOpenHttpsListener(callbackID, listenerHandle)
	log_debug("rpc:onOpenHttpsListener called", callbackID, listenerHandle)
	return self:doingWithCO(callbackID, listenerHandle)
end
function rpc:onCloseListener(callbackID, connectHandle)
	log_debug("rpc:onCloseListener called", callbackID, connectHandle)
	return self:doingWithCO(callbackID, connectHandle)
end
function rpc:onCloseConnect(callbackID, connectHandle)
	log_debug("rpc:onCloseConnect called", callbackID, connectHandle)
	if callbackID > 0 then
		-- 上层主动调用 closeConnect 返回结果
		self:checkConnectListener(connectHandle)
		return self:doingWithCO(callbackID, connectHandle)
	else
		-- 处理连接自动断开的行为，比如说从group里面移除连接
		return self:checkConnectListener(connectHandle)
	end
end

-- C++调用函数 End-------------------

-- 对外接口，注册一个消息的处理函数和eproto协议
function rpc:register(msgName, obj, objFunc)
	local msgInfo = self.m_msgNameMap[msgName]
	if msgInfo == nil then
		log_error("rpc:register failed msgInfo == nil", msgName)
		return
	end
	msgInfo.obj = obj
	msgInfo.objFunc = objFunc
end
-- 对外接口，执行一个携程函数
function rpc:execute(callback, needMinID)
	-- 创建携程
	local co = coroutine.create(callback)
	-- 注册当前的携程
	local callbackID,coInfo = self:registerCoroutine(co, needMinID)
	if not self:resumeCoroutine(callbackID, coInfo, coInfo) then
		log_error("rpc:execute checkCoroutine failed")
		return false
	end
	return true,callbackID,coInfo
end
-- 对外接口，请求一个服务组内部服务
function rpc:requestRandom(moduleType, functionName, request, response, timeout)
	local connectHandle = self.m_pHandler:randModuleHandle(moduleType)
	return self:requestService(connectHandle, connectHandle, functionName, request, response, timeout)
end
function rpc:request(moduleType, moduleIndex, functionName, request, response, timeout)
	local connectHandle = self.m_pHandler:getModuleHandle(moduleType, moduleIndex)
	return self:requestService(connectHandle, connectHandle, functionName, request, response, timeout)
end
-- 对外接口，请求一个服务组内部服务
function rpc:requestService(connectHandle, destination, functionName, request, response, timeout)
	local buffer = self:encodeRequest(functionName, request, response, false)
	if buffer == nil then
		log_error("rpc:request for encodeRequest buffer == nil", destination, functionName)
		return false
	end
	local result = self.m_pHandler:sendToDestination(connectHandle, destination, buffer, #buffer)
	if not result then
		log_debug("rpc:request for sendToDestination failed", destination, functionName, request)
		return false
	end
	if response ~= nil then
		local isOK,responseValue = coroutine.yield(timeout)
		if not isOK then
			log_error("rpc:request for timeout", destination, functionName, request, responseValue)
			return false
		end
		for k,v in pairs(responseValue) do
			response[k] = v
		end
	end
	return true
end
-- 对外接口，请求另一个服务组的服务
function rpc:requestGroup(groupName, moduleName, moduleIndex, functionName, request, response, timeout)
	local buffer = self:encodeRequest(functionName, request, response, true)
	if buffer == nil then
		log_error("rpc:requestGroup for encodeRequest buffer == nil", groupName, moduleName, functionName)
		return false
	end
	local result = self.m_pHandler:sendToGroup(groupName, moduleName, moduleIndex, buffer, #buffer)
	if not result then
		log_debug("rpc:requestGroup for sendToGroup failed", groupName, moduleName, functionName, request)
		return false
	end
	if response ~= nil then
		local isOK,responseValue = coroutine.yield(timeout)
		if not isOK then
			log_error("rpc:requestGroup for timeout", groupName, moduleName, functionName, request, responseValue)
			return false
		end
		for k,v in pairs(responseValue) do
			response[k] = v
		end
	end
	return true
end
-- 对外接口，发起一个curl的http请求
function rpc:requestHttp(method, url, headers, body, response, timeout)
	method = method:upper()		-- C++ 只认大写
	local co = coroutine.running()
--	log_debug("requestHttp current running coroutine", co)
	if co == nil then
		log_error("requestHttp should have a coroutine running")
		return false
	end
	local coInfo = self:getInfoByCO(co)
	if coInfo == nil then
		log_error("requestHttp can not find coroutine info")
		return false
	end
	local callbackID = coInfo.callbackID
	local result = self.m_pHandler:sendCurlRequest(method, url, headers, body, callbackID)
	if not result then
		log_debug("rpc:requestHttp for sendCurlRequest failed", method, url, headers, callbackID)
		return false
	end
	local isOK,responseValue = coroutine.yield(timeout)
	if not isOK then
		log_error("rpc:requestHttp for timeout", method, url, headers, callbackID)
		return false
	end
	for k,v in pairs(responseValue) do
		response[k] = v
	end
	return true
end
-- 对外接口，设置一个计时器，单位毫秒
function rpc:schedule(callback, timeout, loop)
	local coCallback = function(coInfo)
		self:scheduleTimeout(coInfo, timeout)
--		log_debug("rpc:schedule begin", coInfo.callbackID, coInfo.timerHandle, timeout)
		coroutine.yield(timeout)
--		log_debug("rpc:schedule callback begin", coInfo.callbackID, coInfo.timerHandle, timeout)
		if loop then
			while true do
--				log_debug("rpc:schedule callback loop", coInfo.callbackID, coInfo.timerHandle, timeout)
				callback(coInfo)
				self:changeCoroutineCallbackID(coInfo)
				coroutine.yield(timeout)
			end
		else
			callback(coInfo)
		end
--		log_debug("rpc:schedule callback end", coInfo.callbackID, coInfo.timerHandle, timeout)
	end
	return self:execute(coCallback, false)
end

-- 对外接口，其它
-- 让一个client发起Hive的内部验证
function rpc:identifyHive(connectHandle)
	self.m_pHandler:identifyHive(connectHandle)
end
function rpc:openClient(ip, port, isNeedEncrypt, isNeedDecrypt, acceptIndex)
	return self:processWithCO("openClient", ip, port, isNeedEncrypt, isNeedDecrypt, acceptIndex)
end
function rpc:openHttpListener(ip, port)
	return self:processWithCO("openHttpListener", ip, port)
end
function rpc:openHttpsListener(ip, port)
	return self:processWithCO("openHttpsListener", ip, port)
end
function rpc:openSocketListener(ip, port, isNeedEncrypt, isNeedDecrypt, acceptIndex)
	return self:processWithCO("openSocketListener", ip, port, isNeedEncrypt, isNeedDecrypt, acceptIndex)
end
function rpc:closeListener(listenerHandle)
	return self:processWithCO("closeListener", listenerHandle)
end
function rpc:closeConnect(connectHandle)
	return self:processWithCO("closeConnect", connectHandle)
end
function rpc:bindAccept(acceptHandle, bindHandle)
	self.m_pHandler:bindAccept(acceptHandle, bindHandle)
end
function rpc:addGroup(groupName)
	self.m_pHandler:addGroup(groupName)
end
function rpc:removeGroup(groupName)
	self.m_pHandler:removeGroup(groupName)
end
function rpc:getGroupRoute(groupName, moduleName)
	return self.m_pHandler:getGroupRoute(groupName, moduleName)
--	return result,connectHandle,moduleType
end
function rpc:setGroupRoute(groupName, connectHandle)
	self.m_pHandler:setGroupRoute(groupName, connectHandle)
end
function rpc:removeGroupRoute(groupName, connectHandle)
	self.m_pHandler:removeGroupRoute(groupName, connectHandle)
end
function rpc:appendGroupModule(groupName, moduleName, moduleType)
	self.m_pHandler:appendGroupModule(groupName, moduleName, moduleType)
end
function rpc:removeGroupModule(groupName, moduleName)
	self.m_pHandler:removeGroupModule(groupName, moduleName)
end
function rpc:addConnectListener(connectHandle, callback)
	self.m_connectListener[connectHandle] = callback
end
function rpc:addClientOKListener(connectHandle, callback)
	self.m_clientOKListener[connectHandle] = callback
end
function rpc:addIdentifyOKListener(connectHandle, callback)
	self.m_identifyOKListener[connectHandle] = callback
end
function rpc:addListener(obj)
	local m_listener = self.m_listener
	if m_listener  == nil then
		m_listener = {}
		self.m_listener = m_listener
	end
	table.insert(m_listener, obj)
end

-- 内部接口 -------------------------------------------
function rpc:initRpc(pHandler, param)
	self.m_pHandler = pHandler
	self.m_param = param
	self.m_msgMap = {}
	self.m_msgNameMap = {}
	self.m_cbMap = {}
	self.m_coMap = {}
	self.m_minLoop = 1	-- (1,65535)
	self.m_id = 65536	-- (65536,2147483647)
	self.m_connectListener = {}
	self.m_clientOKListener = {}
	self.m_identifyOKListener = {}

	-- 初始化config的配置，建立服务器组间的连接
	if pHandler ~= nil then
		-- Handler 是由主线程创建的，初始化操作需要等到后面执行，这里使用Timer延迟执行
		self:schedule(function()
			return self:delayInit()
		end, 1000)
	end
end
function rpc:getHandler()
	return self.m_pHandler
end
function rpc:delayInit()
	self:initRpcProto()
	self:buildGroupConnection()
	local pHandler = self.m_pHandler
	local param = self.m_param
	local m_listener = self.m_listener
	if m_listener then
		for _,obj in ipairs(m_listener) do
			local onInitialize = obj.onInitialize
			if onInitialize then
				onInitialize(obj, pHandler, param)
			else
				log_debug("Object do not provide onInitialize function", obj.__cname)
			end
		end
	else
		log_debug("delayInit m_listener == nil")
	end
end
function rpc:initRpcProto()
	package.loaded["rpcproto"] = nil
	local rpcproto = require("rpcproto")
	local m_msgMap = self.m_msgMap
	local m_msgNameMap = self.m_msgNameMap
	for _,info in ipairs(rpcproto) do
		local msgID = info[1]
		local functionName = info[2]
		local requestName = info[3]
		local responseName = info[4]
		local msgInfo = { msgID=msgID, functionName=functionName, requestName=requestName, responseName=responseName }
		m_msgMap[msgID] = msgInfo
		m_msgNameMap[functionName] = msgInfo
	end
end
function rpc:buildGroupConnection()
	log_debug("buildGroupConnection called begin")
	local pHandler = self.m_pHandler
	local config = require("config")
	local groups = config.groups
	for groupName,groupInfo in pairs(groups) do
		-- 添加组
		pHandler:addGroup(groupName)
		-- 添加组内模块
		for moduleName,moduleType in pairs(groupInfo.modules) do
			pHandler:appendGroupModule(groupName, moduleName, moduleType)
		end
		-- 创建组内连接
		for _,connInfo in pairs(groupInfo.connects) do
			self:startGroupConnection(groupName, connInfo)
		end
	end
	log_debug("buildGroupConnection called end")
end
function rpc:startGroupConnection(groupName, connInfo)
	local callback = function()
		local pHandler = self.m_pHandler
		local ip, port, isNeedEncrypt, isNeedDecrypt, acceptIndex = connInfo.ip, connInfo.port, connInfo.isNeedEncrypt, connInfo.isNeedDecrypt, connInfo.acceptIndex
		log_debug("startGroupConnection callback", ip, port, isNeedEncrypt, isNeedDecrypt, acceptIndex)
		local isOK,resultHandle = self:openClient(ip, port, isNeedEncrypt, isNeedDecrypt, acceptIndex)
		if not isOK then
			-- 创建连接失败，3秒后重新连接
			log_debug("startGroupConnection openClient failed")
			self:schedule(function()
				return self:startGroupConnection(groupName, connInfo)
			end, 3000)
			return
		end
		log_debug("startGroupConnection callback openClient OK", ip, port, isNeedEncrypt, isNeedDecrypt, acceptIndex)
		-- 设置连接断开回调函数
		self:addConnectListener(resultHandle, function()
			log_debug("startGroupConnection client disconnect", resultHandle)
			pHandler:removeGroupRoute(groupName, resultHandle)
			-- 连接断开，3秒后重新尝试创建连接
			self:schedule(function()
				return self:startGroupConnection(groupName, connInfo)
			end, 3000)
		end)
		-- 设置连接成功的回调函数
		self:addClientOKListener(resultHandle, function()
			log_debug("startGroupConnection identifyHive", resultHandle)
			return self:identifyHive(resultHandle)
		end)
		-- 设置验证成功回调函数
		self:addIdentifyOKListener(resultHandle, function()
			log_debug("startGroupConnection setGroupRoute", groupName, resultHandle)
			-- 设置当前的连接到group中
			pHandler:setGroupRoute(groupName, resultHandle)
		end)
	end
	return self:execute(callback, false)
end
function rpc:destroyRpc()
	log_debug("rpc:destroyRpc called")
	local pHandler = self.m_pHandler
	for _,coInfo in pairs(self.m_cbMap) do
		local timerHandle = coInfo.timerHandle
		if timerHandle then
			pHandler:removeTimer(timerHandle)
		end
	end
	local m_listener = self.m_listener
	if m_listener then
		for _,obj in ipairs(m_listener) do
			local onDestroy = obj.onDestroy
			if onDestroy then
				onDestroy(obj)
			else
				log_debug("Object do not provide onDestroy function", obj.__cname)
			end
		end
	else
		log_debug("destroyRpc m_listener == nil")
	end
	self:initRpc(nil)
end
function rpc:checkConnectListener(connectHandle)
	self.m_clientOKListener[connectHandle] = nil
	self.m_identifyOKListener[connectHandle] = nil
	local callback = self.m_connectListener[connectHandle]
	if callback ~= nil then
		self.m_connectListener[connectHandle] = nil
		return callback()
	end
end
function rpc:checkClientOKListener(connectHandle)
	local callback = self.m_clientOKListener[connectHandle]
	if callback ~= nil then
		self.m_clientOKListener[connectHandle] = nil
		return callback()
	end
end
function rpc:checkIdentifyOKListener(connectHandle)
	local callback = self.m_identifyOKListener[connectHandle]
	if callback ~= nil then
		self.m_identifyOKListener[connectHandle] = nil
		return callback()
	end
end
function rpc:registerCoroutine(co, needMinID)
	local callbackID = self:nextID(needMinID)
	local coInfo = { callbackID=callbackID, co=co }
	self.m_cbMap[callbackID] = coInfo
	self.m_coMap[co] = coInfo
	return callbackID,coInfo
end
function rpc:changeCoroutineCallbackID(coInfo)
	-- 注释掉老的callbackID
	local callbackID = coInfo.callbackID
	self.m_cbMap[callbackID] = nil
	-- 生成新的callbackID
	callbackID = self:nextID(false)
	coInfo.callbackID = callbackID
	self.m_cbMap[callbackID] = coInfo
	return callbackID,coInfo
end
function rpc:getInfoByID(callbackID)
	return self.m_cbMap[callbackID]
end
function rpc:getInfoByCO(co)
	return self.m_coMap[co]
end
function rpc:getTimerLeft(coInfo)
	local timerHandle = coInfo.timerHandle
	if timerHandle ~= nil then
		return self.m_pHandler:leftTimer(timerHandle)
	end
	return 0
end
function rpc:remove(coInfo)
	return self:removeByID(coInfo.callbackID)
end
function rpc:removeByID(callbackID)
	log_debug("rpc:removeByID", callbackID)
	local coInfo = self.m_cbMap[callbackID]
	if coInfo ~= nil then
		self.m_cbMap[callbackID] = nil
		self.m_coMap[coInfo.co] = nil
		local timerHandle = coInfo.timerHandle
		log_debug("rpc:removeByID", timerHandle, callbackID)
		if timerHandle then
			log_debug("rpc:removeByID removeTimer", timerHandle, callbackID)
			self.m_pHandler:removeTimer(timerHandle)
		end
	else
		log_error("rpc:removeByID not found coInfo", callbackID)
	end
end
-- 获取小循环的ID，长度会被控制在2个字节以内
function rpc:minLoop()
	local id = self.m_minLoop
	local minLoop = id + 1
	if minLoop > 65535 then
		self.m_minLoop = 1
	else
		self.m_minLoop = minLoop
	end
	if self.m_cbMap[id] == nil then
		return id
	end
end
function rpc:nextID(needMinID)
	local id
	if needMinID then
		id = self:minLoop()
	end
	while id == nil do
		id = self.m_id
		local nextID = id + 1
		if nextID > 2147483647 then
			self.m_id = 65536
		else
			self.m_id = nextID
		end
		if self.m_cbMap[id] ~= nil then
			id = nil
		end
	end
	return id
end
function rpc:encodeRequest(functionName, request, response, isGroup)
	local info = self.m_msgNameMap[functionName]
	if info == nil then
		log_error("encodeRequest failed info == nil", functionName, request)
		return
	end
	local buffer = eproto.encode(info.requestName, request)
	local rpcReq = {
		msgID = info.msgID;
		buffer = buffer;
	}
	if response ~= nil then
		-- with response
		if isGroup then
			rpcReq.msgType = 2	-- for group
		else
			rpcReq.msgType = 1	-- for node inner message
		end
		rpcReq.destination = self.m_pHandler:getHandle()
		local co = coroutine.running()
--		log_debug("encodeRequest current running coroutine", co)
		if co ~= nil then
			local coInfo = self:getInfoByCO(co)
			if coInfo then
				rpcReq.callbackID = coInfo.callbackID
			else
				log_error("encodeRequest but no co info found", functionName)
			end
		else
			log_error("encodeRequest but no running co found", functionName)
		end
	else
		rpcReq.msgType = 0		-- without response
	end
	log_debug("encodeRequest with callbackID", rpcReq.callbackID, functionName)
	local rpcBuffer = eproto.encode("eprpc", rpcReq)
	return rpcBuffer
end
function rpc:encodeResponse(msgType, msgID, callbackID, responseName, response)
	local buffer = eproto.encode(responseName, response)
	local rpcRes = {
		msgType = msgType;
		msgID = msgID;
		destination = self.m_pHandler:getHandle();
		callbackID = callbackID;
		buffer = buffer;
	}
	local rpcBuffer = eproto.encode("eprpc", rpcRes)
	return rpcBuffer
end
function rpc:decodeRpc(rpcBuffer)
	local rpcInfo,len = eproto.decode("eprpc", rpcBuffer)
	if len ~= #rpcBuffer then
		log_error("decodeRpc failed len ~= #rpcBuffer", #rpcBuffer, len)
		return
	end
	local msgID = rpcInfo.msgID
	local info = self.m_msgMap[msgID]
	if info == nil then
		log_error("decodeRpc failed info == nil", msgID, rpcInfo.callbackID)
		return
	end
	local buffer = info.buffer
	if buffer == nil then
		log_error("decodeRpc failed buffer == nil")
		return
	end
	local protoName
	if rpcInfo.msgType == 3 then
		protoName = info.responseName
	else
		protoName = info.requestName
	end
	local rpcData,len = eproto.decode(protoName, buffer)
	if len ~= #buffer then
		log_error("decodeRpc failed len ~= #buffer", msgID, rpcInfo.callbackID)
		return
	end
	return rpcData,rpcInfo
end
function rpc:doingHttp(isOK, callbackID, buffer)
	log_debug("rpc:doingHttp with callbackID", isOK, callbackID)
	local coInfo = self:getInfoByID(callbackID)
	if coInfo == nil then
		log_error("rpc:doingHttp can not find coInfo", callbackID)
		return
	end
	local co = coInfo.co
	-- 执行携程函数
	local response = {
		isOK = isOK;
		buffer = buffer;
	}
	if not self:resumeCoroutine(callbackID, coInfo, true, response) then
		log_error("rpc:doingHttp checkCoroutine failed")
		return
	end
end
function rpc:doing(connectHandle, rpcBuffer)
	local request,rpcReq = self:decodeRpc(rpcBuffer)
	if request == nil then
		log_error("rpc:doing for decodeRequest failed")
		return
	end
	local msgType = rpcReq.msgType
	if msgType == 0 then
		return self:doingRequest(request, rpcReq)
	elseif msgType == 1 then
		return self:doingRequest(request, rpcReq, "responseDestination", connectHandle, 3)
	elseif msgType == 2 then
		return self:doingRequest(request, rpcReq, "responseGroup", connectHandle, 3)
	elseif msgType == 3 then
		return self:response(request, rpcReq)
	else
		log_error("rpc:doing unknown msgType", msgType)
	end
end
function rpc:doingRequest(request, rpcReq, responseFunctionName, connectHandle, msgType)
	-- 查找消息的注册信息
	local msgID = rpcReq.msgID
	local info = self.m_msgMap[msgID]
	local obj = info.obj
	local objFunc = info.objFunc
	if obj == nil or objFunc == nil then
		log_error("rpc:doingRequest for obj == nil or objFunc == nil", msgID)
		return
	end
	local response = {}
	-- 创建一个携程处理当前的请求
	local callback = function()
		log_debug("rpc:doingRequest call obj function begin", msgID)
		objFunc(obj, request, response, rpcReq, connectHandle)
		log_debug("rpc:doingRequest call obj function end", msgID)
		-- 发送消息，返回给请求方
		if responseFunctionName ~= nil then
			local buffer = self:encodeResponse(msgType, msgID, rpcReq.callbackID, info.responseName, response)
			local pHandler = self.m_pHandler
			local responseFunction = pHandler[responseFunctionName]
			local result = responseFunction(pHandler, connectHandle, rpcReq.destination, buffer, #buffer)
			if not result then
				log_error("rpc:doingRequest for response failed", msgID, rpcReq.callbackID, connectHandle, rpcReq.destination)
			else
				log_debug("rpc:doingRequest for response OK", msgID, rpcReq.callbackID, connectHandle, rpcReq.destination)
			end
		else
			log_debug("rpc:doingRequest for no response", msgID, rpcReq.callbackID, connectHandle, rpcReq.destination)
		end
	end
	if self:execute(callback, true) then
		log_error("rpc:doingRequest execute error", msgID)
		return
	end
	log_debug("rpc:doingRequest execute OK", msgID)
end
function rpc:response(response, rpcReq)
	local callbackID = rpcReq.callbackID
	log_debug("rpc:response with callbackID", callbackID, rpcReq.msgID)
	local coInfo = self:getInfoByID(callbackID)
	if coInfo == nil then
		log_error("rpc:response can not find coInfo", callbackID)
		return
	end
	local co = coInfo.co
	if not self:resumeCoroutine(callbackID, coInfo, true, response) then
		log_error("rpc:response checkCoroutine failed")
		return
	end
end
function rpc:scheduleTimeout(coInfo, timeout)
	local timerHandle = coInfo.timerHandle
	local callbackID = coInfo.callbackID
	log_debug("rpc:scheduleTimeout", callbackID, timerHandle)
	if timerHandle ~= nil then
		log_debug("scheduleTimeout changeTimer", timerHandle, callbackID)
		self.m_pHandler:changeTimer(timerHandle, timeout)
	else
		timerHandle = self.m_pHandler:startTimer(callbackID, timeout)
		coInfo.timerHandle = timerHandle
		log_debug("scheduleTimeout startTimer", timerHandle, callbackID)
	end
end
function rpc:doingTimeout(callbackID)
	log_debug("rpc:doingTimeout with callbackID", callbackID)
	local coInfo = self:getInfoByID(callbackID)
	if coInfo == nil then
		log_error("rpc:doingTimeout can not find coInfo", callbackID)
		return
	end
	-- 定时器超时的时候，不需要再执行后续的删除timer操作
	coInfo.timerHandle = nil
	if not self:resumeCoroutine(callbackID, coInfo, false) then
		log_error("rpc:doingTimeout checkCoroutine failed")
		return
	end
end
function rpc:processWithCO(functionName, ...)
	local co = coroutine.running()
--	log_debug("processWithCO current running coroutine", functionName, co)
	if co == nil then
		log_error("processWithCO should have a coroutine running", functionName)
		return false
	end
	local coInfo = self:getInfoByCO(co)
	if coInfo == nil then
		log_error("processWithCO can not find coroutine info", functionName)
		return false
	end
	local callbackID = coInfo.callbackID
	local pHandler = self.m_pHandler
	local func = pHandler[functionName]
	log_debug("processWithCO before call", functionName)
	func(pHandler, callbackID, ...)
	log_debug("processWithCO after call", functionName)
	local isOK,responseValue = coroutine.yield(10000)	-- 最多等待10秒返回结果
	log_debug("processWithCO after yield", functionName)
	return isOK,responseValue
end
function rpc:doingWithCO(callbackID, resultHandle)
	log_debug("rpc:doingWithCO with callbackID", callbackID, resultHandle)
	local coInfo = self:getInfoByID(callbackID)
	if coInfo == nil then
		log_error("rpc:doingWithCO can not find coInfo", callbackID)
		return
	end
	local co = coInfo.co
	local result = (resultHandle > 0)
	if not self:resumeCoroutine(callbackID, coInfo, result, resultHandle) then
		log_error("rpc:doingWithCO checkCoroutine failed")
		return
	end
end
function rpc:resumeCoroutine(callbackID, coInfo, ...)
	local co = coInfo.co
	-- 执行携程函数
	local result,returnValue = coroutine.resume(co, ...)
	if not result then
		log_error("rpc:checkCoroutine failed", returnValue)
		self:removeByID(callbackID)
		return false
	end
	-- 当前的请求被挂起了，需要处理挂起操作
	local co_status = coroutine.status(co)
	log_debug("rpc:resumeCoroutine co_status", co_status, type(co_status), callbackID)
	if co_status == "suspended" then
		-- 设定超时移除的计时器 returnValue 为 timeout
		self:scheduleTimeout(coInfo, returnValue)
	else
		-- 执行结束，直接移除当前注册的携程
		self:removeByID(callbackID)
	end
	return true
end


return rpc
