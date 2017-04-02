
HIVENET_STATICLIB :=

# static的依赖库要放在static的后面，比如luasocket依赖openssl
LIBPATH = -L/usr/local/lib
LIBS =

INCLUDES =

# 注意，这里的 -ldl 需要放在flag的最后才能生效
CFLAGS = -O2 -Wall -pthread -lrt $(INCLUDES) -lstdc++ -std=c++11 $(HIVENET_STATICLIB) $(LIBPATH) $(LIBS) -ldl

CC = gcc
RM = rm -f
BIN = .
TARGET = client

$(TARGET): client.o main.o
	$(CC) -g client.o main.o -o $(BIN)/$(TARGET) $(CFLAGS)

client.o: client.cpp client.h
	$(CC) -g -c $< -o $@ -O2 -Wall -pthread $(INCLUDES) -lstdc++ -std=c++11 -ldl

main.o: main.cpp
	$(CC) -g -c $< -o $@ -O2 -Wall -pthread $(INCLUDES) -lstdc++ -std=c++11 -ldl

clean:
	-$(RM) client.o
	-$(RM) main.o
	-$(RM) $(BIN)/$(TARGET)
