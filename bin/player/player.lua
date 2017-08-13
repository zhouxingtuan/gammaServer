--
-- Created by IntelliJ IDEA.
-- User: AppleTree
-- Date: 2017/6/8
-- Time: 下午11:30
-- To change this template use File | Settings | File Templates.
--

local rpc = require("rpc")
local blockMap = require("blockMap")

local log_debug = log_debug
local log_error = log_error

local player = class("player")

rpc:addListener(player)

function player:onInitialize(pHandler, param)
	-- 创建数据库
	self.m_dataDB = blockMap.new()
	local config = require("config")
	local dataBaseName = config.moduleName
	local result = self.m_dataDB:open(dataBaseName, 100000)
	if not result then
		log_error("player:onInitialize open data db failed", dataBaseName)
	end
	-- 注册msgID对应的处理函数
	rpc:register("player:set", 			self, self.set)
	rpc:register("player:get", 			self, self.get)
	rpc:register("player:update", 		self, self.update)
	rpc:register("player:getTable", 	self, self.getTable)
	rpc:register("player:getRecord", 	self, self.getRecord)
end
function player:onDestroy()
	-- 销毁数据库
	local m_dataDB = self.m_dataDB
	if m_dataDB ~= nil then
		m_dataDB:close()
		self.m_dataDB = nil
	end
end

-- 对外接口
function player:set(request, response, rpcReq, connectHandle)
	response.result = self.m_dataDB:setRecord(request.key, request.value)
	log_debug("player:set", request, response, rpcReq)
end
function player:get(request, response, rpcReq, connectHandle)
	response.value = self.m_dataDB:getRecord(request.key)
	log_debug("player:get", request, response, rpcReq)
end
function player:update(request, response, rpcReq, connectHandle)
	response.result = self.m_dataDB:updateRecord(request.key, request.value)
	log_debug("player:update", request, response, rpcReq)
end
function player:getTable(request, response, rpcReq, connectHandle)
	response.value = self.m_dataDB:getTable(request.key, request.tableName)
	log_debug("player:getTable", request, response, rpcReq)
end
function player:getRecord(request, response, rpcReq, connectHandle)
	response.value = self.m_dataDB:getTableRecord(request.key, request.tableName, request.id)
	log_debug("player:getRecord", request, response, rpcReq)
end


return player
