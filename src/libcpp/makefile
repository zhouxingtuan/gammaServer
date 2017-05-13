
CORE_INC := ./../libcore/

DLL_NAME := libcpp.so

INC_PATH = -I$(CORE_INC)

# static的依赖库要放在static的后面，比如luasocket依赖openssl
LIB_PATH = -L/usr/local/lib
LIBS = -lssl -lcrypto -lz -lrt -lstdc++ -ldl

LIBS = -lssl -lcrypto -lz -lrt -lstdc++ -ldl

# 注意，这里的 -ldl 需要放在flag的最后才能生效
CXXFLAGS = -O2 -Wall -fPIC -pthread -std=c++11 $(INC_PATH) $(LIB_PATH) $(LIBS)
CFLAGS = -O2 -Wall -fPIC -pthread $(INC_PATH) $(LIB_PATH) $(LIBS)
CC = gcc
AR = ar
ARFLAGS = cr
DEBUG= -g -ggdb

BIN = ../../bin

OBJS = cpphandler.o cppmodule.o

all: $(OBJS) $(DLL_NAME)

$(OBJS): %.o:%.cpp
	$(CC) $(DEBUG) -c $< -o $@ $(CXXFLAGS)

$(DLL_NAME):$(OBJS)
	$(CC) $(DEBUG) $(OBJS) $(CXXFLAGS) -shared -o $(DLL_NAME)
	$(RM) $(BIN)/$(DLL_NAME)
	cp $(DLL_NAME) $(BIN)/$(DLL_NAME)

cpphandler.o:cpphandler.cpp cpphandler.h

cppmodule.o:cpphandler.o cppmodule.cpp cppmodule.h

clean:
	-$(RM) $(DLL_NAME)
	-$(RM) *.o


