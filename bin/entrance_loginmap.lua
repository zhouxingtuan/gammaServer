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

	};
	moduleName = "loginmap";
}
package.loaded["config"] = config

require("main")
require("loginmap")

