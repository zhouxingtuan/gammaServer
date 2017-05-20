--
-- Created by IntelliJ IDEA.
-- User: AppleTree
-- Date: 2017/5/20
-- Time: 下午1:55
-- To change this template use File | Settings | File Templates.
--

--[[
class Handler
{
	unsigned int getService(void) const;
	unsigned int getType(void) const;
	unsigned int getIndex(void) const;
	unsigned int getNode(void) const;
	unsigned int getHandle(void) const;											// 获取当前的句柄

	void identifyHive(uint32 connectHandle);
	void openClient(uint32 callbackID, const char* ip, uint16 port, bool isNeedEncrypt, bool isNeedDecrypt, uint8 acceptIndex);
	void openHttpListener(uint32 callbackID, const char* ip, uint16 port);
	void openHttpsListener(uint32 callbackID, const char* ip, uint16 port);
	void openSocketListener(uint32 callbackID, const char* ip, uint16 port, bool isNeedEncrypt, bool isNeedDecrypt, uint8 acceptIndex);
	void closeListener(uint32 callbackID, uint32 listenerHandle);
	void closeConnect(uint32 callbackID, uint32 connectHandle);
	void bindAccept(uint32 acceptHandle, uint32 bindHandle);
	void sendToDestination(uint32 command, uint32 destination, const char* ptr, uint32 length);

    uint32 startTimer(uint32 callbackID, int64 timeCount);			        // 开始一个计时器
    void removeTimer(uint32 handle);								        // 移除计时器
    void changeTimer(uint32 handle, int64 timeCount);				        // 更改计时器的时间
    int64 leftTimer(uint32 handle);									        // 获取计时器剩余时间
};// end class Handler
-- ]]

local print = print
local m_pHandler

print("main loaded")

function onInitialize(pHandler, param)
	print("onInitialize called", pHandler:getHandle(), param)
	m_pHandler = pHandler

end
function onDestroy()
	print("onDestroy called")

end
function onTimerUpdate(callbackID)
	print("onTimerUpdate called", callbackID)

end
function onReceivePacket(command, destination, buffer)
	print("onReceivePacket called", command, destination, #buffer)

end
function onCurlResponse(isRequestOK, callbackID, buffer)
	print("onCurlResponse called", isRequestOK, callbackID, #buffer)

end
function onOpenClientOK(clientHandle)
	print("onOpenClientOK called", clientHandle)

end
function onOpenClient(callbackID, clientHandle)
	print("onOpenClient called", callbackID, clientHandle)

end
function onOpenSocketListener(callbackID, listenerHandle)
	print("onOpenSocketListener called", callbackID, listenerHandle)

end
function onOpenHttpListener(callbackID, listenerHandle)
	print("onOpenHttpListener called", callbackID, listenerHandle)

end
function onOpenHttpsListener(callbackID, listenerHandle)
	print("onOpenHttpsListener called", callbackID, listenerHandle)

end
function onCloseListener(callbackID, connectHandle)
	print("onCloseListener called", callbackID, connectHandle)

end






