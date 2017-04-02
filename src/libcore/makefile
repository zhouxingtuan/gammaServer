
THIRD_CURL_INC := /usr/local/include/curl/

LIB_NAME := libcore.a

INC_PATH := -I$(THIRD_CURL_INC)
LIB_PATH :=
LIBS = -lssl -lcrypto -lz -lrt -lstdc++ -ldl

# 注意，这里的 -ldl 需要放在flag的最后才能生效
CXXFLAGS = -O2 -Wall -pthread -std=c++11 $(INC_PATH) $(LIB_PATH) $(LIBS)
CFLAGS = -O2 -Wall -pthread $(INC_PATH) $(LIB_PATH) $(LIBS)
CC = gcc
AR = ar
ARFLAGS = cr
DEBUG= -g -ggdb

OBJS = common.o util.o systemsignal.o token.o log.o buffer.o destination.o packet.o destinationpool.o destinationgroup.o\
		object.o objectpool.o objecthashpool.o timer.o task.o epoll.o active.o activeworker.o \
		accept.o client.o http.o https.o listener.o timerhandle.o handler.o multicurl.o \
		globalhandler.o globalsetting.o epollworker.o globalservice.o worker.o handlerqueue.o mainworker.o


COBJS = http_parser.o

all: $(COBJS) $(OBJS) $(LIB_NAME)

$(OBJS): %.o:%.cpp
	$(CC) $(DEBUG) -c $< -o $@ $(CXXFLAGS)

$(COBJS): %.o:%.c
	$(CC) $(DEBUG) -c $< -o $@ $(CFLAGS)

$(LIB_NAME):$(OBJS) $(COBJS)
	$(AR) $(ARFLAGS) $(LIB_NAME) $(OBJS) $(COBJS)

clean:
	-$(RM) $(LIB_NAME)
	-$(RM) *.o

common.o:common.cpp common.h

util.o:common.o util.cpp util.h

systemsignal.o:systemsignal.cpp systemsignal.h

token.o:common.o token.cpp token.h

log.o:common.o log.cpp log.h

buffer.o:common.o log.o buffer.cpp buffer.h

task.o:common.o task.cpp task.h

destination.o:common.o destination.cpp destination.h

packet.o:buffer.o packet.cpp packet.h

destinationpool.o:destination.o destinationpool.cpp destinationpool.h

destinationgroup.o:destinationpool.o task.o packet.o destinationgroup.cpp destinationgroup.h

object.o:common.o object.cpp object.h

objectpool.o:object.o objectpool.cpp objectpool.h

objecthashpool.o:object.o objecthashpool.cpp objecthashpool.h

timer.o:common.o objectpool.o task.o timer.cpp timer.h

epoll.o:common.o epoll.cpp epoll.h

active.o:epoll.o active.cpp active.h

activeworker.o:timer.o activeworker.cpp activeworker.h

accept.o:destination.o epoll.o packet.o util.o timer.o accept.cpp accept.h

client.o:accept.o client.cpp client.h

http.o:http_parser.o destination.o epoll.o packet.o util.o timer.o http.cpp http.h

https.o:http.o https.cpp https.h

listener.o:destination.o epoll.o listener.cpp listener.h

timerhandle.o:timer.o object.o objectpool.o timerhandle.cpp timerhandle.h

handler.o:destination.o task.o packet.o timer.o handler.cpp handler.h

multicurl.o:handler.o active.o epoll.o timer.o task.o util.o packet.o multicurl.cpp multicurl.h

globalhandler.o:handler.o destinationgroup.o globalhandler.cpp globalhandler.h

epollworker.o:activeworker.o accept.o client.o http.o https.o multicurl.o epollworker.cpp epollworker.h

globalservice.o:globalhandler.o epollworker.o globalservice.cpp globalservice.h

globalsetting.o:accept.o http.o globalsetting.cpp globalsetting.h

worker.o:handler.o worker.cpp worker.h

handlerqueue.o:handler.o worker.o handlerqueue.cpp handlerqueue.h

mainworker.o:epollworker.o mainworker.cpp mainworker.h










