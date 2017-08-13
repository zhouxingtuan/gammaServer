--
-- Created by IntelliJ IDEA.
-- User: AppleTree
-- Date: 2017/5/25
-- Time: 下午10:50
-- To change this template use File | Settings | File Templates.
--

local ipairs = ipairs
local select = select
local type = type
local table = table
local tostring = tostring
local debug = debug
local string = string
local _G = _G
local rawset = rawset
local io = io
local os = os
local getTimeStringUS = getTimeStringUS

local dump = require("dump")

local log = {}
rawset(_G, "log", log)

log.outfile = nil
log.level = "trace"

local modes = { "trace", "debug", "info", "warn", "error", "fatal" }

function log.open(fileName)
	log.close()
	local outfile = io.open(fileName, "a")
	log.outfile = outfile
end
function log.close()
	local outfile = log.outfile
	if outfile then
		outfile:close()
		log.outfile = nil
	end
end
function log.dispatch(callback)
	log.callback = callback
end

local levels = {}
for i,name in ipairs(modes) do
	levels[name] = i
end

local log_tostring = function(...)
	local t = {}
	for i = 1, select('#', ...) do
		local x = select(i, ...)
		if type(x) == "table" then
			x = dump(x, 10, true, nil)
			x = "\n"..x.."\n"
		end
		t[#t + 1] = tostring(x)
	end
	return table.concat(t, " ")
end

for i,name in ipairs(modes) do
	local nameupper = name:upper()
	log[name] = function(...)
		-- Return early if we're below the log level
		if i < levels[log.level] then
			return
		end

		local msg = log_tostring(...)
		local info = debug.getinfo(2, "Sl")
		local lineinfo = info.short_src .. ":" .. info.currentline
		local str = string.format("[%-6s%s][%s] %s\n", nameupper, getTimeStringUS(), lineinfo, msg)

		-- Output to log file
		local callback = log.callback
		if callback then
			callback(str)
		else
			local outfile = log.outfile
			if outfile then
				outfile:write(str)
				outfile:flush()
			end
		end
	end
	rawset(_G, "log_"..name, log[name])
end

return log
