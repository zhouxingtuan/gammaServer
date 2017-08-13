--
-- Created by IntelliJ IDEA.
-- User: AppleTree
-- Date: 2017/5/28
-- Time: 下午10:43
-- To change this template use File | Settings | File Templates.
--

local require = require
local pairs = pairs

local eproto = require("eproto")

local _M = {}

_M.eprpc 					= require("eprpc")

_M.epreqset 				= require("epreqset")
_M.epreqget 				= require("epreqget")
_M.epreqreplace 			= require("epreqreplace")
_M.eprequpdate 				= require("eprequpdate")
_M.epreqlogin 				= require("epreqlogin")
_M.epreqregister 			= require("epreqregister")
_M.epreqaccountset 			= require("epreqaccountset")
_M.epreqgettable 			= require("epreqgettable")
_M.epreqgetrecord 			= require("epreqgetrecord")

_M.epresbool 				= require("epresbool")
_M.epresvalue 				= require("epresvalue")
_M.epreslogin 				= require("epreslogin")

_M.epaccount 				= require("epaccount")
_M.epsession 				= require("epsession")

-- register all the ui protos
for name,proto in pairs(_M) do
	eproto.register(name, proto)
end

return _M
