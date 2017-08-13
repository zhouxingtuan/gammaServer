--
-- Created by IntelliJ IDEA.
-- User: AppleTree
-- Date: 2017/6/3
-- Time: 下午2:51
-- To change this template use File | Settings | File Templates.
--

local rpcproto = {
	-- 注册账号名到uid的映射服务
	{01,	"loginmap:set",			"epreqset", 		"epresbool"};
	{02,	"loginmap:setn",		"epreqset", 		"epresbool"};
	{03,	"loginmap:get",			"epreqget", 		"epresvalue"};
	{04,	"loginmap:del", 		"epreqget", 		"epresvalue"};
	{05,	"loginmap:replace", 	"epreqreplace", 	"epresbool"};
	{06,	"loginmap:incrby", 		"epreqset", 		"epresvalue"};
	-- 登录服务
	{11,	"login:register", 		"epreqregister", 	"epreslogin"};
	{12,	"login:login", 			"epreqlogin", 		"epreslogin"};
	{13,	"login:update", 		"eprequpdate", 		"epresbool"};
	-- 账号数据服务
	{21,	"account:set", 			"epreqaccountset", 	"epresbool"};
	{22,	"account:get", 			"epreqget", 		"epaccount"};
	{23,	"account:update", 		"eprequpdate", 		"epresbool"};
	-- player数据服务
	{31,	"player:set", 			"epreqset", 		"epresbool"};
	{32,	"player:get", 			"epreqget", 		"epresvalue"};
	{33,	"player:update", 		"eprequpdate", 		"epresbool"};
	{34,	"player:getTable", 		"epreqgettable", 	"epresvalue"};
	{35,	"player:getRecord",		"epreqgetrecord", 	"epresvalue"};
	-- user服务

	-- map地图服务

	-- charge充值数据服务

	-- chat聊天服务

}

return rpcproto
