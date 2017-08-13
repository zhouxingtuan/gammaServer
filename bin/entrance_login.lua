--
-- Created by IntelliJ IDEA.
-- User: AppleTree
-- Date: 2017/6/1
-- Time: 上午12:03
-- To change this template use File | Settings | File Templates.
--

local path = require("path")

local config = {
	log_level = "debug";	-- { "trace", "debug", "info", "warn", "error", "fatal" }
	path = path;
	groups = {
		account = {
			modules = {
				account = 1;
			};
			connects = {
				{ ip="127.0.0.1", port=10101, isNeedEncrypt=false, isNeedDecrypt=false, acceptIndex=0 };
			};
		};
	};
	moduleName = "login";
	start_block = 100;		-- 从100万开始分配玩家的ID
	max_block = 109;		-- 最大分配到110万

}
package.loaded["config"] = config

require("main")
require("login")

