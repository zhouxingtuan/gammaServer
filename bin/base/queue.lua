--
-- Created by IntelliJ IDEA.
-- User: AppleTree
-- Date: 16/12/4
-- Time: 下午1:00
-- To change this template use File | Settings | File Templates.
--

local table = table
local class = require("class")

local queue = class("queue")

function queue:ctor()
	self.data = {}
	self.startIndex = 1
	self.endIndex = 1
end
function queue:clear()
	self.data = {}
	self.startIndex = 1
	self.endIndex = 1
end
-- 压入队列尾部
function queue:push(d)
	if d ~= nil then
		local end_index = self.endIndex
		self.data[end_index] = d
		self.endIndex = end_index + 1
	end
end
-- 从队列头部弹出来
function queue:pop()
	local data = self.data
	local start_index = self.startIndex
	local ret = data[start_index]
	if ret ~= nil then
		data[start_index] = nil
		self.startIndex = start_index + 1
	end
	return ret
end
-- 获取第一个元素
function queue:front()
	return self.data[self.startIndex]
end
-- 获取最后一个元素
function queue:back()
	return self.data[self.endIndex-1]
end
-- 获取 index
function queue:getEndIndex()
	return self.endIndex
end
function queue:getStartIndex()
	return self.startIndex
end
function queue:getFirstIndex()
	return self.startIndex
end
function queue:getLastIndex()
	return self.endIndex - 1
end
-- 判断队列是否为空
function queue:isEmpty()
	return self.endIndex == self.startIndex
end
-- 获取队列中元素的数量
function queue:count()
	return self.endIndex - self.startIndex
end
-- 删除队列中的某个元素
function queue:erase(d)
	local data = self.data
	local eraseIndex, v
	for k=self.startIndex,self.endIndex do
		v = data[k]
		if eraseIndex == nil then
			if v == d then
				eraseIndex = k
			end
		else    -- 将eraseIndex队列后面的元素前移
			data[k-1] = data[k]
		end
	end
	-- 如果发生了删除操作，将endIndex后移
	if eraseIndex ~= nil then
		self.endIndex = self.endIndex - 1
	end
end
-- 查找符合条件的某个元素；从队列头部开始查找，找到就停止
function queue:find(match)
	local data = self.data
	local v
	for k=self.startIndex,self.endIndex-1 do
		v = data[k]
		if match(v) then
			return k,v
		end
	end
end
-- 查找符合条件的某个元素；从队列尾部开始查找，找到就停止
function queue:rfind(match)
	local data = self.data
	local v
	for k=self.endIndex-1,self.startIndex,-1 do
		v = data[k]
		if match(v) then
			return k,v
		end
	end
end
function queue:rfindFrom(match, restart_index)
	restart_index = restart_index or self.endIndex-1
	local data = self.data
	local v
	for k=restart_index,self.startIndex,-1 do
		v = data[k]
		if match(v) then
			return k,v
		end
	end
end
function queue:findFrom(match, restart_index)
	restart_index = restart_index or self.startIndex
	local data = self.data
	local v
	for k=restart_index,self.endIndex-1 do
		v = data[k]
		if match(v) then
			return k,v
		end
	end
end
-- 从头部开始弹出元素，直到某个key==k
function queue:popFront(k)
	local data = self.data
	local start_index = self.startIndex
	for key=start_index,k do
		local ret = data[key]
		if ret ~= nil then
			data[key] = nil
			start_index = start_index + 1
		else
			break
		end
	end
	self.startIndex = start_index
end
function queue:popBack(k)
	local data = self.data
	local end_index = self.endIndex
	for key=end_index-1,k,-1 do
		local ret = data[key]
		if ret ~= nil then
			data[key] = nil
			end_index = end_index - 1
		else
			break
		end
	end
	self.endIndex = end_index
end
-- 遍历获取元素
function queue:copyBack(number)
	local arr = {}
	local data = self.data
	local end_index = self.endIndex
	local to_index = end_index - number
	for key=end_index-1,to_index,-1 do
		local ret = data[key]
		if ret ~= nil then
			table.insert(arr, ret)
		else
			break
		end
	end
	return arr
end
function queue:copyFront(number)
	local arr = {}
	local data = self.data
	local start_index = self.startIndex
	local to_index = start_index + number - 1
	for key=start_index,to_index do
		local ret = data[key]
		if ret ~= nil then
			table.insert(arr, ret)
		else
			break
		end
	end
	return arr
end
-- 排序方法
function queue:sort(sortFunc)
	local data = self.data
	local temp = {}
	for k=self.startIndex,self.endIndex-1 do
		table.insert(temp, data[k])
	end
	table.sort(temp, sortFunc)
	local index = 1
	for k=self.startIndex,self.endIndex-1 do
		data[k] = temp[index]
		index = index + 1
	end
end
-- 复制data的引用
function queue:copyRef()
	local data = self.data
	local ret = {}
	for k=self.startIndex,self.endIndex-1 do
		ret[k] = data[k]
	end
	return ret
end
-- 复制数据的数组
function queue:copyArr()
	local data = self.data
	local ret = {}
	for k=self.startIndex,self.endIndex-1 do
		table.insert(ret, data[k])
	end
	return ret
end

return queue
