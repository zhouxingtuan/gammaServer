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

local log_debug = log_debug
local log_error = log_error

local account = class("account")

rpc:addListener(account)

function account:onInitialize(pHandler, param)
	-- 创建数据库

	-- 注册msgID对应的处理函数
	rpc:register("account:set", 		self, self.set)
	rpc:register("account:get", 		self, self.get)
	rpc:register("account:update", 		self, self.update)
end
function account:onDestroy()
	-- 销毁数据库

end

function account:set(request, response, rpcReq, connectHandle)
	log_debug("account:set", request, response, rpcReq)
end
function account:get(request, response, rpcReq, connectHandle)
	log_debug("account:get", request, response, rpcReq)
end
function account:update(request, response, rpcReq, connectHandle)
	log_debug("account:update", request, response, rpcReq)
end

return account
