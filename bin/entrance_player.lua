--
-- Created by IntelliJ IDEA.
-- User: AppleTree
-- Date: 2017/6/8
-- Time: 下午11:40
-- To change this template use File | Settings | File Templates.
--

local path = require("path")

local config = {
	log_level = "debug";	-- { "trace", "debug", "info", "warn", "error", "fatal" }
	path = path;
	groups = {

	};
	moduleName = "player";
}
package.loaded["config"] = config

require("main")
require("player")
