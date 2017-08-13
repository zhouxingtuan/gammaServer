--
-- Created by IntelliJ IDEA.
-- User: AppleTree
-- Date: 2017/6/8
-- Time: 下午11:33
-- To change this template use File | Settings | File Templates.
--

local rpc = require("rpc")

local user = class("user")

rpc:addListener(user)

function user:onInitialize(pHandler, param)

end
function user:onDestroy()

end

-- 对外接口
-- 手机使用uuid登录，如果不存在则注册一个账号
function user:login(request, response, rpcReq, connectHandle)
	-- 检查是不是session登录，查找session内容，转发该登录请求到对应的handler

	-- 不是sessio登录就向login服务请求

	-- login服务返回，判断是否能登录成功

	-- 登录成功将结果抛给对应uid的handler处理

	--
end


return user
