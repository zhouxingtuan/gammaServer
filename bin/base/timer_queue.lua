--
-- Created by IntelliJ IDEA.
-- User: AppleTree
-- Date: 15/11/20
-- Time: 下午12:39
-- To change this template use File | Settings | File Templates.
--

local os = os
local queue = require("queue")
local timer_queue = class("timer_queue")

function timer_queue:ctor(m_t, m_cb, m_max)
    -- 初始化
    self.m_t = m_t                      -- 检查的间隔时间
    self.m_cb = m_cb                    -- 移除对象时调用的回掉函数
    self.m_max = m_max or 2147483647    -- 最大缓存的数量
    self.m_q = queue.new()
    self.m_m = {}
end
function timer_queue:push(d)
    local t = os.time()
    local m_m = self.m_m
    local info = m_m[d]
    if info ~= nil then
        info.a = t              -- 更新最后一次激活时间
        return
    end
    m_m[d] = { a=t, t=t }
    local m_q = self.m_q
    if m_q:count() >= self.m_max then
        local s = m_q:pop()
        m_m[s] = nil
        self.m_cb(s)
    end
    return m_q:push(d)
end
function timer_queue:pop()
    local d = self.m_q:pop()
    self.m_m[d] = nil
    self.m_cb(d)
    return d
end
function timer_queue:isHave(d)
	return (self.m_m[d] ~= nil)
end
function timer_queue:count()
    return self.m_q:count()
end
function timer_queue:update()
    local m_q = self.m_q
    if m_q:isEmpty() then
        return
    end
    local e = m_q:getEndIndex()
    local s = m_q:getStartIndex()
    local m_m = self.m_m
    local m_cb = self.m_cb
    local d, info, it, at
    local t = os.time()
    local dl = t - self.m_t     -- 截止时间点
    while s < e do
        d = m_q:front()
        info = m_m[d]
        it = info.t
        if dl > it then
            -- 从队列的头部移除这个数据
            m_q:pop()
            s = m_q:getStartIndex()    -- 检查下一个
            -- 检查是否需要重新进入队列
            at = info.a
            if at > it then
                info.t = at
                m_q:push(d)
            else
                m_cb(d)     -- 调用外部的操作进行处理
            end
        else
            break   -- 不需要再往下检查了，后面的时间都更久
        end
    end
end

return timer_queue
