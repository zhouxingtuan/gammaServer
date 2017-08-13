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
local block = require("block")

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
	local config = require("config")
	local dbName = config.moduleName
	local m_loginuidDB = block.new()
	self.m_loginuidDB = m_loginuidDB
	local result = m_loginuidDB:open(dbName, 0, 10000000)
	if not result then
		log_error("openDB failed", dbName)
	end
	self:loadUID()
	-- 注册msgID对应的处理函数
	rpc:register("login:register", 		self, self.register)
	rpc:register("login:login", 		self, self.login)
	rpc:register("login:update", 		self, self.update)
end
function login:onDestroy()
	-- 销毁数据库
	local m_loginuidDB = self.m_loginuidDB
	if m_loginuidDB ~= nil then
		m_loginuidDB:close()
		self.m_loginuidDB = nil
	end
end

-- 对外接口，command函数
function login:register(request, response, rpcReq, connectHandle)
	-- 临时分配一个uid
	local uid = self:getAvaliableUID()
	-- map分为10组，根据当前的name进行hash求模
	local regtype = request.regtype
	local name = request.name
	local password = request.password or ""
	local regsource = request.regsource
	log_debug("login:register temp uid", uid, regtype, name, password, regsource)
	-- 插入成功则账号可以注册，申请生成一个账号数据，失败则回收uid同时删除map映射
	local isOK,err = self:insertIntoMap(name, uid)
	if not isOK then
		self:releaseOccupy(uid, false)
		response.result = err
		return
	end
	-- 注册账号成功，生成账号数据
	local isOK,err = self:setAccountData(uid, regtype, name, password, regsource)
	if not isOK then
		self:releaseOccupy(uid, false)
		response.result = err
		-- 删除 map 中数据
		self:deleteFromMap(name)
		return
	end
	self:releaseOccupy(uid, true)
	-- 注册账号成功，生成session数据
	local session = self:generateSession(uid, name, password)
	-- 返回登录session数据给注册方
	response.result = 0
	response.session = session
end
function login:login(request, response, rpcReq, connectHandle)
	local regtype = request.regtype
	local regsource = request.regsource
	local name = request.name
	local password = request.password
	local magic = request.magic
	-- 把password解密
	password = self.m_pHandler:decrypt(password, #password, "")
	request.password = password
	log_debug("login:login", regtype, regsource, name, password, magic)
	-- 查找玩家信息的映射uid
	local isOK,uid = self:searchUID(name)
	if not isOK then
		response.result = uid
		return
	end
	-- 找不到：如果允许注册新账号则走注册流程，否则返回账号未注册
	if uid == nil then
		-- login by uuid
		if regtype == regtype_uuid then
			return self:register(request, response, rpcReq, connectHandle)
		end
		response.result = login_err_not_register
		log_error("login:login uid == nil")
		return
	end
	-- 根据玩家的uid查找玩家的账号数据，失败则返回错误
	local result,acc = self:getAccountData(uid)
	if not result then
		response.result = acc
		return
	end
	-- 根据玩家账号数据，检查账号的密码，失败则返回错误
	if password ~= acc.password then
		response.result = login_err_password_failed
		log_error("login:login password ~= acc.password")
		return
	end
	-- 登录成功，生成session数据
	local session = self:generateSession(uid, name, password)
	-- 返回登录session数据给登录方
	response.result = 0
	response.session = session
end
function login:update(request, response, rpcReq, connectHandle)
	-- 更新账号数据并保存到数据库 1000000 为账号数据库的划分
	local moduleIndex = self:getModuleIndex(request.key)
	local result = rpc:requestGroup("account", "account", moduleIndex, "account:update", request, response, 10)
	if not result then
		log_error("login:update", request, response, rpcReq)
	else
		log_debug("login:update", request, response, rpcReq)
	end
end

-- 对内接口，内部使用函数
function login:generateSession(uid, name, password)
	local currentTime = os.time()
	local temp = uid .. name .. password .. currentTime
	local magic = self.m_pHandler:hash(temp, #temp)
	local session = {
		uid = uid;
		magic = magic;
		time = currentTime;
	}
	return session
end
function login:searchUID(name)
	local moduleIndex = self:getRegisterModuleIndex(name)
	local request = {
		key = name;
	}
	local response = {}
	local result = rpc:request(loginmap_moduleType, moduleIndex, "loginmap:get", request, response, 10)
	if not result then
		log_error("rpc:searchUID failed", loginmap_moduleType, moduleIndex, "loginmap:get")
		return false,login_err_loginmap_get_failed
	end
	local uid = response.value
	return true,uid
end
function login:getRegisterModuleIndex(name)
	local moduleIndex = self.m_pHandler:hash(name, #name)
	moduleIndex = math.floor((moduleIndex % 1000000) / 100000) + 1
	return moduleIndex
end
function login:getModuleIndex(uid)
	local moduleIndex = math.floor(uid / 1000000) + 1
	return moduleIndex
end
function login:insertIntoMap(name, uid)
	local request = {
		key = name;
		value = uid;
	}
	local moduleIndex = self:getRegisterModuleIndex(name)
	local response = {}
	local result = rpc:request(loginmap_moduleType, moduleIndex, "loginmap:setn", request, response, 10)
	if not result then
		log_error("rpc:request failed", loginmap_moduleType, moduleIndex, "loginmap:setn")
		return false,login_err_loginmap_setn_failed
	end
	return response.result
end
function login:deleteFromMap(name)
	local request = {
		key = name;
	}
	local moduleIndex = self:getRegisterModuleIndex(name)
	local response = {}
	local result = rpc:request(loginmap_moduleType, moduleIndex, "loginmap:del", request, response, 10)
	if not result then
		log_error("rpc:request failed", loginmap_moduleType, moduleIndex, "loginmap:del")
		return false,login_err_loginmap_del_failed
	end
	return response.result
end
function login:getAccountData(uid)
	local moduleIndex = self:getModuleIndex(uid)
	local request = {
		key = uid;
	}
	local response = {}
	local result = rpc:requestGroup("account", "account", moduleIndex, "account:get", request, response, 10)
	if not result then
		log_error("rpc:requestGroup account:get failed")
		return false,login_err_account_get_failed
	end
	-- 没有取到数据
	if response.uid == nil then
		log_error("login:getAccountData response.uid == nil", uid)
		return false,login_err_account_not_found_failed
	end
	return true,response
end
function login:setAccountData(uid, regtype, name, password, regsource)
	local moduleIndex = self:getModuleIndex(uid)
	-- 查看是否能够插入到map，失败则回收uid
	local acc = {
		id = uid;
		uid = uid;
		regtype = regtype;
		password = password;
		regsource = regsource;
		regtime = os.time();
	}
	if regtype == regtype_name then
		acc.name = name
	elseif regtype == regtype_uuid then
		acc.uuid = name
	elseif regtype == regtype_email then
		acc.email = name
	elseif regtype == regtype_phone then
		acc.phone = name
	else
		log_error("login:setAccountData unknown regtype", regtype)
		return false,login_err_regtype_unknown
	end
	local request = {
		key = uid;
		value = eproto.encode("epaccount", acc);
	}
	local response = {}
	local result = rpc:requestGroup("account", "account", moduleIndex, "account:set", request, response, 10)
	if not result then
		log_error("rpc:requestGroup failed", "account", moduleIndex, "account:set")
		return false,login_err_account_set_failed
	end
	return response.result
end

function login:loadUID()
	self.m_blockMap = {}
	local config = require("config")
	local start_block = config.start_block
	local max_block = config.max_block
	self.m_tryBlockIndex = start_block
	local m_loginuidDB = self.m_loginuidDB
	local m_blockMap = self.m_blockMap
	for index=start_block,max_block do
		local info
		local value = m_loginuidDB:get(index)
		if value == nil then
			local min = index*10000
			local max = (index+1)*10000
			info = { min=min, max=max, cursor=min, occupy=false, }
			self:saveBlockInfo(index, info)
		else
			info = eproto.unpack(value)
			-- 上一次有做过操作，成功失败不知道，直接抛弃这个uid
			if info.occupy then
				info.cursor = info.cursor + 1
				self:saveBlockInfo(index, info)
			end
			info.occupy = false
		end
		m_blockMap[index] = info
	end
end
function login:saveBlockInfo(index, info)
	local value = eproto.pack(info)
	self.m_loginuidDB:set(index, value)
end
function login:getAvaliableUID()
	local config = require("config")
	local start_block = config.start_block
	local max_block = config.max_block
	local m_blockMap = self.m_blockMap
	local tryIndex = self.m_tryBlockIndex
	for index=tryIndex,max_block do
		local info = m_blockMap[index]
		if not info.occupy and info.cursor < info.max then
			info.occupy = true
			self:setTryBlockIndex(index + 1, start_block, max_block)
			log_debug("login:getAvaliableUID", info.cursor)
			self:saveBlockInfo(index, info)
			return info.cursor
		end
	end
	for index=start_block,tryIndex do
		local info = m_blockMap[index]
		if not info.occupy and info.cursor < info.max then
			info.occupy = true
			self:setTryBlockIndex(index + 1, start_block, max_block)
			log_debug("login:getAvaliableUID", info.cursor)
			self:saveBlockInfo(index, info)
			return info.cursor
		end
	end
	-- 目前已经没有可以使用的uid，返回nil
	log_debug("login:getAvaliableUID can not get any uid")
end
function login:setTryBlockIndex(index, start_block, max_block)
	if index > max_block then
		self.m_tryBlockIndex = start_block
	else
		self.m_tryBlockIndex = index
	end
end
function login:releaseOccupy(uid, isOK)
	local index = math.floor(uid / 10000)
	local info = self.m_blockMap[index]
	if info then
		if uid == info.cursor then
			info.occupy = false
			if isOK then
				info.cursor = info.cursor + 1
				self:saveBlockInfo(index, info)
			end
			log_debug("login:releaseOccupy OK", uid, info.cursor)
		else
			log_error("login:releaseOccupy uid not match cursor", uid, info.cursor)
		end
	else
		log_error("login:releaseOccupy can not find info to release", uid)
	end
end

return login
