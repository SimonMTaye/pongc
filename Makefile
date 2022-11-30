CC = clang
CFLAGS = -g -lpthread -lncurses

pong: pong.c constants.h
	$(CC) pong.c  -o pong $(CFLAGS) 
