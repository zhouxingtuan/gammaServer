--
-- Created by IntelliJ IDEA.
-- User: AppleTree
-- Date: 16/12/4
-- Time: 下午1:00
-- To change this template use File | Settings | File Templates.
--

local table = table
local class = require("class")

local stack = class("stack")
function stack:ctor()
	self.data = {}
end
function stack:clear()
	self.data = {}
end
-- 从顶部压入一个元素
function stack:push(d)
	if d ~= nil then
		table.insert(self.data, d)
	end
end
-- 从栈顶退出一个或者多个元素
function stack:pop()
	local data = self.data
	local total = #data
	if total < 0 then
		return
	end
	local ret = data[total]
	data[total] = nil  -- 置空最后一个元素
	return ret
end
-- 获取栈顶的元素
function stack:top()
	local data = self.data
	return data[#data]
end
-- 获取栈底的元素
function stack:bottom()
	return self.data[1]
end
-- 通过索引index获取某一个元素
function stack:at(index)
	return self.data[index]
end
-- 判断堆栈是否为空
function stack:isEmpty()
	return #self.data == 0
end
-- 获取元素的数量
function stack:count()
	return #self.data
end
-- 插入某个元素；如果index超出当前stack的范围，则插入到末尾
function stack:insert(d, index)
	local data = self.data
	local total = #data
	if index > total then
		return self:push(d)
	end
	if d == nil then
		return
	end
	if index < 1 then
		index = 1
	end
	-- 将index往后的元素向后移动
	for k=total,index,-1 do
		data[k+1] = data[k]
	end
	data[index] = d
end
-- 删除某个元素；但是不清空栈顶的元素
function stack:erase(index)
	if index == nil then
		return
	end
	local data = self.data
	local total = #data
	if index >= total then
		return self:pop()
	end
	if index < 1 then
		index = 1
	end
	-- 将index往后的元素向前移动
	local ret = data[index]
	for k=index,total-1 do
		data[k] = data[k+1]
	end
	data[total] = nil  -- 置空最后一个元素
	return ret
end
-- 替换某一个元素，并返回被替换的元素；超出范围则替换栈顶或者栈底的元素
function stack:replace(d, index)
	local data = self.data
	local total = #data
	if total < 0 then
		return
	end
	index = index or total
	if index > total then
		index = total
	end
	if index < 1 then
		index = 1
	end
	local ret = data[index]
	data[index] = d
	return ret,index
end
-- 查找符合条件的某个元素；从栈顶往下开始寻找，找到就停止
function stack:find(match)
	local data = self.data
	local v
	for k=#data,1,-1 do
		v = data[k]
		if match(v) then
			return k,v
		end
	end
end
-- 查找符合条件的某个元素；从栈底往上开始查找，找到就停止
function stack:rfind(match)
	local data = self.data
	local v
	for k=1,#data do
		v = data[k]
		if match(v) then
			return k,v
		end
	end
end
-- 排序方法
function stack:sort(sortFunc)
	table.sort(self.data, sortFunc)
end
-- 复制data的引用
function stack:copyRef()
	local data = self.data
	local ret = {}
	for k=1,#data do
		ret[k] = data[k]
	end
	return ret
end
-- 复制数据的数组
function stack:copyArr()
	local data = self.data
	local ret = {}
	for k=1,#data do
		table.insert(ret, data[k])
	end
	return ret
end

return stack
