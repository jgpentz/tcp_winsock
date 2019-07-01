CC=gcc
CFLAGS=-I.
DEPS = tcp_client.h
OBJ = tcp_client.o
LIBS=-lwsock32 -lWs2_32

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

tcp_client: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	del /Q /S *.o tcp_client.exe