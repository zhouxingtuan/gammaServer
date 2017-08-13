--
-- Created by IntelliJ IDEA.
-- User: AppleTree
-- Date: 2017/6/1
-- Time: 上午12:12
-- To change this template use File | Settings | File Templates.
--

local math = math
local os = os
local require = require
local eproto = require("eproto")
local rpc = require("rpc")

local log_debug = log_debug
local log_error = log_error

local loginmap_moduleType = 2

local regtype_name = 1
local regtype_uuid = 2
local regtype_email = 3
local regtype_phone = 4

local login_err_regtype_unknown = 1
local login_err_loginmap_setn_failed = 2
local login_err_loginmap_get_failed = 3
local login_err_account_set_failed = 4
local login_err_not_register = 5
local login_err_loginmap_del_failed = 6
local login_err_account_get_failed = 7
local login_err_account_not_found_failed = 8
local login_err_password_failed = 9

local login = class("login")

rpc:addListener(login)

function login:onInitialize(pHandler, param)
	self.m_pHandler = pHandler
	-- 创建数据库

	-- 注册msgID对应的处理函数
	rpc:register("login:register", 		self, self.register)
	rpc:register("login:login", 		self, self.login)
	rpc:register("login:update", 		self, self.update)
end
function login:onDestroy()
	-- 销毁数据库

end

-- 对外接口，command函数
function login:register(request, response, rpcReq, connectHandle)

end
function login:login(request, response, rpcReq, connectHandle)

end
function login:update(request, response, rpcReq, connectHandle)

end

return login
