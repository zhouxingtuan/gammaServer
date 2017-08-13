--
-- Created by IntelliJ IDEA.
-- User: AppleTree
-- Date: 2017/6/1
-- Time: 上午12:15
-- To change this template use File | Settings | File Templates.
--

local log_debug = log_debug
local log_error = log_error
local rpc = require("rpc")
local map = require("map")

local loginmap = class("loginmap")

rpc:addListener(loginmap)

function loginmap:onInitialize(pHandler, param)
	log_debug("loginmap:onInitialize", pHandler:getHandle())
	self.m_pHandler = pHandler
	-- 创建数据库
	local moduleIndex = pHandler:getModuleIndex()
	local config = require("config")
	local dbName = config.moduleName .. "_" .. moduleIndex
	local m_mapDB = map.new()
	self.m_mapDB = m_mapDB
	local result = m_mapDB:open(dbName)
	if not result then
		log_error("openDB failed", dbName)
	end
	-- 注册对应的处理函数
	rpc:register("loginmap:set", 		self, self.set)
	rpc:register("loginmap:setn", 		self, self.setn)
	rpc:register("loginmap:get", 		self, self.get)
	rpc:register("loginmap:del", 		self, self.del)
	rpc:register("loginmap:replace", 	self, self.replace)
	rpc:register("loginmap:incrby", 	self, self.incrby)
end
function loginmap:onDestroy()
	-- 销毁数据库
	local m_mapDB = self.m_mapDB
	if m_mapDB ~= nil then
		m_mapDB:close()
		self.m_mapDB = nil
	end
end

-- 对外接口，command函数
function loginmap:set(request, response, rpcReq, connectHandle)
	response.result = self.m_mapDB:set(request.key, request.value)
	log_debug("loginmap:set", request, response, rpcReq)
end
function loginmap:setn(request, response, rpcReq, connectHandle)
	response.result = self.m_mapDB:setn(request.key, request.value)
	log_debug("loginmap:setn", request, response, rpcReq)
end
function loginmap:get(request, response, rpcReq, connectHandle)
	response.value = self.m_mapDB:get(request.key)
	log_debug("loginmap:get", request, response, rpcReq)
end
function loginmap:del(request, response, rpcReq, connectHandle)
	response.value = self.m_mapDB:del(request.key)
	log_debug("loginmap:del", request, response, rpcReq)
end
function loginmap:replace(request, response, rpcReq, connectHandle)
	response.result = self.m_mapDB:del(request.key, request.newKey)
	log_debug("loginmap:replace", request, response, rpcReq)
end
function loginmap:incrby(request, response, rpcReq, connectHandle)
	response.value = self.m_mapDB:del(request.key, request.value)
	log_debug("loginmap:incrby", request, response, rpcReq)
end

-- 对内接口，内部使用函数


return loginmap
