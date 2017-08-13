
LUA_DIR := ./../../../third/LuaJIT
LUA_INC := $(LUA_DIR)/src

LIB_NAME = libluarapidjson.a
INCLUDES = -I$(LUA_INC) -I./
LIBPATH =
LIBS = -lstdc++ -ldl
# 注意，这里的 -ldl 需要放在flag的最后才能生效
CXXFLAGS = -O2 -Wall -fPIC -pthread -std=c++11 $(INCLUDES) $(LIBPATH) $(LIBS)
CFLAGS = -O2 -Wall -fPIC -pthread $(INCLUDES) $(LIBPATH) $(LIBS)
CC = gcc
AR = ar
ARFLAGS = cr
DEBUG = -g -ggdb

HEADERS =

OBJS = values.o Schema.o Document.o rapidjson.o

COBJS =

all: $(OBJS) $(COBJS) $(LIB_NAME)

$(OBJS): %.o:%.cpp
	$(CC) $(DEBUG) -c $< -o $@ $(CXXFLAGS)

$(COBJS): %.o:%.c
	$(CC) $(DEBUG) -c $< -o $@ $(CFLAGS)

$(LIB_NAME):$(OBJS) $(COBJS)
	$(AR) $(ARFLAGS) $(LIB_NAME) $(COBJS) $(OBJS)

values.o:values.cpp values.hpp luax.hpp rapidjson/rapidjson.h rapidjson/document.h

Schema.o:Schema.cpp values.hpp Userdata.hpp rapidjson/document.h rapidjson/error/en.h rapidjson/istreamwrapper.h rapidjson/schema.h rapidjson/stringbuffer.h

Document.o:Document.cpp values.hpp Userdata.hpp file.hpp rapidjson/document.h \
	rapidjson/error/en.h rapidjson/istreamwrapper.h rapidjson/ostreamwrapper.h \
	rapidjson/pointer.h rapidjson/schema.h rapidjson/stringbuffer.h \
	rapidjson/prettywriter.h rapidjson/filewritestream.h

rapidjson.o:rapidjson.cpp values.hpp Userdata.hpp luax.hpp file.hpp \
	rapidjson/document.h rapidjson/encodedstream.h rapidjson/error/en.h \
	rapidjson/error/error.h rapidjson/filereadstream.h rapidjson/filewritestream.h \
	rapidjson/prettywriter.h rapidjson/rapidjson.h rapidjson/reader.h \
	rapidjson/schema.h rapidjson/stringbuffer.h rapidjson/writer.h

#rjson.o:rjson.cpp rapidjson/reader.h rapidjson/writer.h

clean:
	-$(RM) *.a
	-$(RM) *.o



















