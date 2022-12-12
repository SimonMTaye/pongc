CC = clang
CFLAGS = -g -lpthread -lncurses -lm

main: game.c main.c game.h constants.h
	$(CC) main.c game.c  -o pong $(CFLAGS) 
