--
-- Created by IntelliJ IDEA.
-- User: AppleTree
-- Date: 2017/6/4
-- Time: 上午12:10
-- To change this template use File | Settings | File Templates.
--

local pairs = pairs
local eproto = require("eproto")
local rpc = require("rpc")
local blockRecord = require("blockRecord")

local log_debug = log_debug
local log_error = log_error

local account = class("account")

rpc:addListener(account)

function account:onInitialize(pHandler, param)
	-- 创建数据库
	self.m_accountDB = blockRecord.new()
	local config = require("config")
	local moduleName = config.moduleName
	local result = self.m_accountDB:open(moduleName, 1000000, "epaccount")
	if not result then
		log_error("open account db failed")
	end
	-- 注册msgID对应的处理函数
	rpc:register("account:set", 		self, self.set)
	rpc:register("account:get", 		self, self.get)
	rpc:register("account:update", 		self, self.update)
end
function account:onDestroy()
	-- 销毁数据库
	local m_accountDB = self.m_accountDB
	if m_accountDB ~= nil then
		m_accountDB:close()
		self.m_accountDB = nil
	end
end

function account:set(request, response, rpcReq, connectHandle)
	response.result = self.m_accountDB:setRecordData(request.key, request.value)
	log_debug("account:set", request, response, rpcReq)
end
function account:get(request, response, rpcReq, connectHandle)
	local data = self.m_accountDB:getRecordData(request.key)
	if data then
		for k,v in pairs(data) do
			response[k] = v
		end
	end
	log_debug("account:get", request, response, rpcReq)
end
function account:update(request, response, rpcReq, connectHandle)
	response.result = self.m_accountDB:updateRecord(request.key, request.value)
	log_debug("account:update", request, response, rpcReq)
end

return account
