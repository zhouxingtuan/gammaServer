--
-- Created by IntelliJ IDEA.
-- User: AppleTree
-- Date: 2017/5/29
-- Time: 上午7:34
-- To change this template use File | Settings | File Templates.
--

-- msgType requestWithoutResponse(0) requestWithResponse(1) requestGroupWithResponse(2) response(3)

local eprpc = {
	[1] = "msgType";
	[2] = "msgID";
	[3] = "destination";
	[4] = "callbackID";
	[5] = "buffer";
}

return eprpc
