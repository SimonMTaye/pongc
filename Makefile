CC = clang
CFLAGS = -g -lpthread -lncurses -lm

pong: board.c pong.c board.h constants.h
	$(CC) pong.c board.c  -o pong $(CFLAGS) 
