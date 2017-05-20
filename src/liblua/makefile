
CORE_INC := ./../libcore/

LUA_DIR := ./../../third/LuaJIT
LUA_STATIC := $(LUA_DIR)/src/libluajit.a
LUA_INC := $(LUA_DIR)/src
LUA_DLL_PATH := $(LUA_DIR)/src

TOLUA_STATIC := ./tolua/libtolua.a
TOLUA_INC := ./tolua
TOLUA_DLL_PATH := ./tolua


DLL_NAME := liblua.so

INC_PATH = -I$(CORE_INC) -I$(TOLUA_INC) -I$(LUA_INC)

# static的依赖库要放在static的后面，比如luasocket依赖openssl
LIB_PATH = -L/usr/local/lib -L$(LUA_DLL_PATH) -L$(TOLUA_DLL_PATH)
LIBS = -lssl -lcrypto -lz -lrt -lstdc++ -ldl

# 静态库的引用需要注意顺序，最底层的静态库放在后面
STATIC_LIB := $(TOLUA_STATIC) $(LUA_STATIC)

# 注意，这里的 -ldl 需要放在flag的最后才能生效
CXXFLAGS = -O2 -Wall -fPIC -pthread -std=c++11 $(INC_PATH) $(LIB_PATH) $(LIBS)
CFLAGS = -O2 -Wall -fPIC -pthread $(INC_PATH) $(LIB_PATH) $(LIBS)
CC = gcc
AR = ar
ARFLAGS = cr
DEBUG= -g -ggdb

BIN = ../../bin

OBJS = luahandler.o luamodule.o liblua.o script.o

all: $(OBJS) $(DLL_NAME)

$(OBJS): %.o:%.cpp
	$(CC) $(DEBUG) -c $< -o $@ $(CXXFLAGS)

$(DLL_NAME):$(OBJS)
	$(MAKE) -C tolua
	$(CC) $(DEBUG) $(OBJS) $(STATIC_LIB) $(CXXFLAGS) -shared -o $(DLL_NAME)
	$(RM) $(BIN)/$(DLL_NAME)
#	cp $(LUA_DLL_PATH)/libluajit.so $(BIN)
#	cp $(TOLUA_DLL_PATH)/libtolua.so $(BIN)
	cp $(DLL_NAME) $(BIN)/$(DLL_NAME)

liblua.o:liblua.cpp liblua.h manual.hpp

script.o:script.cpp script.h

luahandler.o:script.o luahandler.cpp luahandler.h

luamodule.o:luahandler.o luamodule.cpp luamodule.h

clean:
	-$(RM) $(TOLUA_DLL_PATH)/*.so
	-$(RM) $(TOLUA_DLL_PATH)/*.a
	-$(RM) $(TOLUA_DLL_PATH)/*.o
	-$(RM) $(DLL_NAME)
	-$(RM) *.so
	-$(RM) *.o


