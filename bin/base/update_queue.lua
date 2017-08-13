--
-- Created by IntelliJ IDEA.
-- User: AppleTree
-- Date: 15/11/20
-- Time: 下午12:39
-- To change this template use File | Settings | File Templates.
--

local queue = require("queue")
local update_queue = class("update_queue")

function update_queue:ctor(m_cb)
    -- 初始化
    self.m_cb = m_cb
    self.m_q = queue.new()
    self.m_m = {}
end
function update_queue:push(d)
    local m_m = self.m_m
    if m_m[d] ~= nil then
        return
    end
    m_m[d] = true
    return self.m_q:push(d)
end
function update_queue:pop()
    local d = self.m_q:pop()
    self.m_m[d] = nil
    return d
end
function update_queue:isHave(d)
    return (self.m_m[d] ~= nil)
end
function update_queue:update(n)
    local m_q = self.m_q
    if m_q:isEmpty() then
        return
    end
    local e = m_q:getEndIndex()
    local s = m_q:getStartIndex()
    local m_m = self.m_m
    local m_cb = self.m_cb
    local c = 0
    local d
    while s < e and c < n do
        c = c + 1
        d = m_q:pop()
        s = m_q:getStartIndex()    -- 检查下一个
        -- 如果外面返回true则从队列中移除；false则重新放回队列的尾部
        if not m_cb(d) then
            m_q:push(d)
        else
            m_m[d] = nil
        end
    end
end

return update_queue
