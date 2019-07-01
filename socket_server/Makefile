CC=gcc
CFLAGS=-I.
DEPS = tcp_server.h
OBJ = tcp_server.o
LIBS=-lwsock32

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

tcp_server: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	del /Q /S *.o tcp_server.exe