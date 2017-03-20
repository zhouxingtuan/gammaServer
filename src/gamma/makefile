
JEMALLOC_LIB := ./../../third/jemalloc/lib/libjemalloc_pic.a ./../../third/jemalloc/lib/libjemalloc.a
JEMALLOC_INC := ./../../third/jemalloc/include/jemalloc/

CURL_LIB := /usr/local/lib/libcurl.a
CURL_INC := /usr/local/include/curl/

CORE_LIB := ./../libcore/libcore.a
CORE_INC := ./../libcore/

# 静态库的引用需要注意顺序，最底层的静态库放在后面
STATIC_LIB := $(CORE_LIB) $(CURL_LIB) $(JEMALLOC_LIB)

# static的依赖库要放在static的后面，比如luasocket依赖openssl
LIBPATH = -L/usr/local/lib
LIBS = -lssl -lcrypto -lz -lrt -lstdc++ -ldl

INCLUDES = -I$(JEMALLOC_INC) -I$(CORE_INC) -I$(CURL_INC)

# 注意，这里的 -ldl 需要放在flag的最后才能生效
CFLAGS = -O2 -Wall -pthread -std=c++11 $(INCLUDES) $(LIBPATH) $(LIBS)

CC = gcc
DEBUG= -g -ggdb
RM = rm -f
BIN = ../../bin
TARGET = gamma

OBJS =

$(TARGET): main.o $(OBJS)
	$(MAKE) -C ./../../third/jemalloc
	$(MAKE) -C ./../libcore
	$(CC) $(DEBUG) main.o $(OBJS) $(STATIC_LIB) -o $(BIN)/$(TARGET) $(CFLAGS)

main.o: main.cpp
	$(CC) $(DEBUG) -c $< -o $@ $(CFLAGS)

$(OBJS): %.o:%.cpp
	$(CC) $(DEBUG) -c $< -o $@ $(CFLAGS)

clean:
	-$(RM) $(BIN)/$(TARGET)
	-$(RM) *.o





