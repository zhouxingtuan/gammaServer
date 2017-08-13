--
-- Created by IntelliJ IDEA.
-- User: AppleTree
-- Date: 16/11/26
-- Time: 下午10:51
-- To change this template use File | Settings | File Templates.
--

local type = type
local pairs = pairs
local string_find = string.find
local string_sub = string.sub
local table_insert = table.insert

local util = {}

local copy = function(res)
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
		return new_table
	end
	return _copy(res)
end
util.copy = copy

local split = function(value, mark)
	local arr = {}
	local fromn = 1
	local token
	local startn, endn = string_find(value, mark, fromn)
	while startn ~= nil do
		token = string_sub( value, fromn, startn-1 )
		table_insert(arr, token)
		fromn = endn + 1
		startn, endn = string_find(value, mark, fromn)
	end
	token = string_sub( value, fromn, #value )
	table_insert(arr, token)
	return arr
end
util.split = split
local count = function(tab)
	local c = 0
	for _,_ in pairs(tab) do
		c = c + 1
	end
	return c
end
util.count = count
local isEmpty = function(tab)
	for _,_ in pairs(tab) do
		return false
	end
	return true
end
util.isEmpty = isEmpty

-- 该方法可以将中间有nil值的参数完全解开
local function param_unpack(t, s)
	s = s or 1
	local max = 0
	for i,_ in pairs(t) do
		if i > max then
			max = i
		end
	end
	local function up(t, i)
		if i < max then
			return t[i],up(t,i+1)
		else
			return t[i]
		end
	end
	return up(t,s)
end
util.unpack = param_unpack

local merge = function(des, res)
	local lookup_table = {}
	local _merge, _mergeTable
	_mergeTable = function(des_table, res_table)
		for k, v in pairs(res_table) do
			if type(k) == "table" then
				_merge(des_table, copy(k), v)
			else
				_merge(des_table, k, v)
			end
		end
	end
	_merge = function(dest, key, value)
		if type(value) ~= "table" then
			dest[key] = value
			return
		elseif lookup_table[value] then
			dest[key] = lookup_table[value]
			return
		end
		local dest_table = dest[key]
		if dest_table == nil or type(dest_table) ~= "table" then
			dest_table = {}
			dest[key] = dest_table
		end
		lookup_table[value] = dest_table
		_mergeTable(dest_table, value)
	end
	_mergeTable(des, res)
end
util.merge = merge

return util
