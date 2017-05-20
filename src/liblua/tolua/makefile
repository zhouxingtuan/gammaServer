
LUA_DIR := ./../../../third/LuaJIT
LUA_INC := $(LUA_DIR)/src
LUA_DLL_PATH := $(LUA_DIR)/src


LIB_NAME = libtolua.a
DLL_NAME = libtolua.so

INCLUDES = -I$(LUA_INC)
LIBPATH = -L/usr/local/lib -L$(LUA_DLL_PATH)
LIBS = -lluajit -lstdc++ -ldl
# 注意，这里的 -ldl 需要放在flag的最后才能生效
CXXFLAGS = -O2 -Wall -fPIC -pthread -std=c++11 $(INCLUDES) $(LIBPATH) $(LIBS)
CFLAGS = -O2 -Wall -fPIC -pthread $(INCLUDES) $(LIBPATH) $(LIBS)
CC = gcc
AR = ar
ARFLAGS = cr
DEBUG = -g -ggdb

OBJS =

COBJS = tolua_event.o tolua_is.o tolua_to.o tolua_map.o tolua_push.o

all: $(OBJS) $(COBJS) $(LIB_NAME) $(DLL_NAME)

$(OBJS): %.o:%.cpp
	$(CC) $(DEBUG) -c $< -o $@ $(CXXFLAGS)

$(COBJS): %.o:%.c
	$(CC) $(DEBUG) -c $< -o $@ $(CFLAGS)

$(LIB_NAME):$(OBJS) $(COBJS)
	$(AR) $(ARFLAGS) $(LIB_NAME) $(COBJS) $(OBJS)

$(DLL_NAME):$(OBJS)
	$(CC) $(DEBUG) $(OBJS) $(COBJS) $(STATIC_LIB) $(CXXFLAGS) -shared -o $(DLL_NAME)

tolua_event.o:tolua_event.c tolua_event.h tolua++.h

tolua_is.o:tolua_is.c tolua++.h

tolua_to.o:tolua_to.c tolua++.h

tolua_map.o:tolua_map.c tolua_event.o tolua++.h

tolua_push.o:tolua_push.c tolua++.h


clean:
	-$(RM) *.a
	-$(RM) *.so
	-$(RM) *.o



















