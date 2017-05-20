--
-- Created by IntelliJ IDEA.
-- User: AppleTree
-- Date: 2017/5/20
-- Time: 下午1:55
-- To change this template use File | Settings | File Templates.
--

local print = print

print("main loaded")

function onInitialize(param)
	print("onInitialize called", param)

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






