CC      := gcc
CFLAGS  := -O2 -std=c11 -Wall -Wextra
AR      := ar
ARFLAGS := rcs

TARGETS := server client
LIB     := libcommon.a

OBJS_COMMON := common.o
OBJS_SERVER := server.o
OBJS_CLIENT := client.o

all: $(LIB) $(TARGETS)

$(LIB): $(OBJS_COMMON)
	$(AR) $(ARFLAGS) $@ $^

server: $(OBJS_SERVER) $(LIB)
	$(CC) $(CFLAGS) -o $@ $(OBJS_SERVER) $(LIB) -pthread

client: $(OBJS_CLIENT) $(LIB)
	$(CC) $(CFLAGS) -o $@ $(OBJS_CLIENT) $(LIB)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

run-server: server
	./server --port 20001 --tnum 4

run-client: client
	./client --k 20 --mod 97 --servers x

clean:
	rm -f $(TARGETS) *.o $(LIB)

.PHONY: all clean run-server run-client
