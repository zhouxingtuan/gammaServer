
CORE_INC := ./../libcore/

LUA_DIR := ./../../third/LuaJIT
LUA_STATIC := $(LUA_DIR)/src/libluajit.a
LUA_INC := $(LUA_DIR)/src
LUA_DLL_PATH := $(LUA_DIR)/src

TOLUA_STATIC := ./tolua/libtolua.a
TOLUA_INC := ./tolua
TOLUA_DLL_PATH := ./tolua

HIREDIS_STATICLIB := ./hiredis/libhiredis.a
HIREDIS_INC := ./hiredis

LUARAPIDJSON_STATICLIB := ./luarapidjson/libluarapidjson.a
LUARAPIDJSON_INC := ./luarapidjson

DLL_NAME := liblua.so

INC_PATH = -I./ -I-I/usr/include/mysql -I$(CORE_INC) -I$(LUA_INC) -I$(TOLUA_INC) -I$(HIREDIS_INC) -I$(LUARAPIDJSON_INC)

# static的依赖库要放在static的后面，比如luasocket依赖openssl
LIB_PATH = -L/usr/lib/mysql -L/usr/lib64/mysql -L/usr/local/lib -L$(LUA_DLL_PATH) -L$(TOLUA_DLL_PATH)
LIBS = -lmysqlclient -lssl -lcrypto -lz -lrt -lstdc++ -ldl

# 静态库的引用需要注意顺序，最底层的静态库放在后面
STATIC_LIB := $(LUARAPIDJSON_STATICLIB) $(TOLUA_STATIC) $(LUA_STATIC) $(HIREDIS_STATICLIB)

# 注意，这里的 -ldl 需要放在flag的最后才能生效
CXXFLAGS = -O2 -Wall -fPIC -Wstrict-overflow=3 -pthread -std=c++11 $(INC_PATH) $(LIB_PATH) $(LIBS)
CFLAGS = -O2 -Wall -fPIC -Wstrict-overflow=3 -pthread $(INC_PATH) $(LIB_PATH) $(LIBS)
CC = gcc
AR = ar
ARFLAGS = cr
DEBUG= -g -ggdb

BIN = ../../bin

OBJS = luamodule.o luahandler.o dbredis.o dbmysql.o liblua.o md5.o eproto.o script.o

COBJS = mime.o

all: $(COBJS) $(OBJS) $(DLL_NAME)

$(OBJS): %.o:%.cpp
	$(CC) $(DEBUG) -c $< -o $@ $(CXXFLAGS)

$(COBJS): %.o:%.c
	$(CC) $(DEBUG) -c $< -o $@ $(CFLAGS)

$(DLL_NAME):$(OBJS) $(COBJS)
	$(MAKE) -C tolua
	$(MAKE) -C luarapidjson
	$(MAKE) -C hiredis
	$(CC) $(DEBUG) $(OBJS) $(COBJS) $(STATIC_LIB) $(CXXFLAGS) -shared -o $(DLL_NAME)
	$(RM) $(BIN)/$(DLL_NAME)
#	cp $(LUA_DLL_PATH)/libluajit.so $(BIN)
#	cp $(TOLUA_DLL_PATH)/libtolua.so $(BIN)
	cp $(DLL_NAME) $(BIN)/$(DLL_NAME)

mime.o:mime.c mime.h

script.o:script.cpp script.h

liblua.o:liblua.cpp liblua.h manual.hpp

eproto.o:eproto.cpp eproto.h

md5.o:md5.cpp md5.h

dbredis.o:script.o dbredis.cpp dbredis.h

dbmysql.o:script.o dbmysql.cpp dbmysql.h

luahandler.o:md5.o dbredis.o dbmysql.o luahandler.cpp luahandler.h

luamodule.o:luahandler.o luamodule.cpp luamodule.h


clean:
	-$(RM) $(TOLUA_DLL_PATH)/*.so
	-$(RM) $(TOLUA_DLL_PATH)/*.a
	-$(RM) $(TOLUA_DLL_PATH)/*.o
	-$(RM) $(DLL_NAME)
	-$(RM) *.so
	-$(RM) *.o


