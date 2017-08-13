--
-- Created by IntelliJ IDEA.
-- User: AppleTree
-- Date: 16/11/26
-- Time: 下午7:53
-- To change this template use File | Settings | File Templates.
--

local type = type
local pairs = pairs
local string = string
local table = table
local tostring = tostring
local print = print

local string_format = string.format
local table_insert = table.insert
local table_concat = table.concat

-- dump打印数据，这里做特殊处理，凡是带有 _ 开头或者super的，都不会导出
local serialise_table_map = {}
local serialise_level
local serialise_table_dump
local serialise_key_dump
local serialise_value_dump
serialise_key_dump = function(value, indent, depth)
	if indent == nil then indent = "" end
	if depth == nil then depth = 0 end

	if type(value) == "string" then
		return value
	elseif type(value) == "nil" or type(value) == "number" or
			type(value) == "boolean" then
		return "[" ..tostring(value).. "]"
	elseif type(value) == "table" then
		return "[" .. serialise_table_dump(value, indent, depth) .. "]"
	else
		return type(value)
	end
end
serialise_table_dump = function(value, indent, depth)
	local spacing, spacing2, indent2
	if indent then
		spacing = "\n" .. indent
		spacing2 = spacing .. "  "
		indent2 = indent .. "  "
	else
		spacing, spacing2, indent2 = " ", " ", false
	end
	depth = depth + 1
	if serialise_level ~= nil and depth > serialise_level then
		return "\"<table>\""
	end
	if serialise_table_map[value] ~= nil then
		return serialise_table_map[value]
	end
	serialise_table_map[value] = "\"<loop table>\""
	--	if depth > 50 then
	--		return "Cannot serialise any further: too many nested tables"
	--	end

	local comma = false
	local fragment = { "{" .. spacing2 }
	for k, v in pairs(value) do
		if comma then
			table_insert(fragment, ";" .. spacing2)
		end
		table_insert(fragment,
			string_format("%s = %s", serialise_key_dump(k, indent2, depth),
				serialise_value_dump(v, indent2, depth)))
		comma = true
	end
	if comma then
		table_insert(fragment, ";")
	end
	table_insert(fragment, spacing .. "}")

	return table_concat(fragment)
end
serialise_value_dump =function(value, indent, depth)
	if indent == nil then indent = "" end
	if depth == nil then depth = 0 end

	if type(value) == "string" then
		return string_format("%q", value)
	elseif type(value) == "nil" or type(value) == "number" or
			type(value) == "boolean" then
		return tostring(value)
	elseif type(value) == "table" then
		return serialise_table_dump(value, indent, depth)
	else
		return "\"<" .. type(value) .. ">\""
	end
end
local function dump(value, level, disableoutput, output_func)
	serialise_table_map = {}
	serialise_level = level
	local ret = serialise_value_dump(value)
	serialise_table_map = nil
	if not disableoutput then
		if output_func then
			return output_func(ret)
		else
			return print(ret)
		end
	else
		return ret
	end
end

return dump
