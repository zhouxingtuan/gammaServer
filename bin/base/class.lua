--
-- Created by IntelliJ IDEA.
-- User: AppleTree
-- Date: 15/4/6
-- Time: 下午9:43
-- To change this template use File | Settings | File Templates.
--

---- global module
local _G = _G
local type = type
local pairs = pairs
local error = error
local tostring = tostring
local setmetatable = setmetatable
local getmetatable = getmetatable
local rawset = rawset
-----------------------------------------------------------------------------
-- class define
-----------------------------------------------------------------------------
-- 记录创建过的类；下面的操作避免重复加载的时候也可以不清掉以前的数据
local classes = _G.classes or {}
rawset(_G, "classes", classes)

local function clone(object)
	local lookup_table = {}
	local function _copy(object)
		if type(object) ~= "table" then
			return object
		elseif lookup_table[object] then
			return lookup_table[object]
		end
		local new_table = {}
		lookup_table[object] = new_table
		for key, value in pairs(object) do
			new_table[_copy(key)] = _copy(value)
		end
		return setmetatable(new_table, getmetatable(object))
	end
	return _copy(object)
end
-- 这里将会修改class的定义，原始定义在extern.lua中；这里会记录创建过的类的类型
--Create an class.
local function class(classname, super)
	-- 查找是否已经有创建过的类，将会直接返回
	if classes[classname] ~= nil then
		return classes[classname]
	end

	local superType = type(super)
	local cls

	-- 这里做一个处理，让外部可以通过字符串来实现继承，前提是另外一个类已经被加载?（感觉也没必要）
	if type(classname) ~= "string" then
		error("classname mast be string type : "..tostring(classname))
	end
	-- 这里需要报错，提醒外面传进来的类型是错误类型
	if superType ~= "function" and superType ~= "table" then
		if superType ~= "nil" then
			error("the super of class are not function,table or nil : "..tostring(classname).."-->"..tostring(superType))
		end
		superType = nil
		super = nil
	end
	if superType == "function" or (super and super.__ctype == 1) then
		-- inherited from native C++ Object
		cls = {}
		if superType == "table" then
			-- copy fields from super
			for k,v in pairs(super) do cls[k] = v end
			cls.__create = super.__create
			cls.super    = super
		else
			cls.__create = super
		end
		cls.ctor    = function() end
		cls.__cname = classname
		cls.__ctype = 1
		function cls.new(...)
			-- 传入的初始化参数是个个类的初始化数据
			local instance = cls.__create(...)
			-- copy fields from class to native object
			for k,v in pairs(cls) do instance[k] = v end
			instance.class = cls
			instance:ctor(...)

			return instance
		end
	else
		-- inherited from Lua Object
		if super then
			cls = clone(super)
			cls.super = super
		else
			cls = {ctor = function() end}
		end

		cls.__cname = classname
		cls.__ctype = 2 -- lua
		cls.__index = cls

		function cls.new(...)
			local instance = setmetatable({}, cls)
			instance.class = cls
			instance:ctor(...)

			return instance
		end
	end

	-- 记录创建的类数据
	classes[classname] = cls
	-- 添加继承函数subclass
	function cls:subclass(name)
		return class(name, cls)
	end

	return cls
end
rawset(_G, "class", class)

-----------------------------------------------------------------------------
-- global variable control
-- 全局变量（_G表中的值）在这里之后将会被控制存取
-----------------------------------------------------------------------------
local gm = {}
-- 凡是通过class注册的类，或者模块，都可以通过_G查询到
gm.__index = function(g, key)
	local c = classes[key]
	if c == nil then
		error("you can't __index global variable(for right value): xxx="..tostring(key))
	end
	return c
end
-- 禁止程序员使用_G表，全局变量全部禁止使用；程序员通过subclass或者module可以声明一个模块
gm.__newindex = function(g, key, value)
	if classes[key] then
		return
	end
	error("you can't __newindex global variable(for left value): "..tostring(key).."="..tostring(value))
end
setmetatable(_G, gm)

return class


