CC = clang
CFLAGS = -g -lpthread -lncurses -lm 
CFLAGS_STRICT = -g -lpthread -lncurses -lm -Werror -Wall 

main: game.c main.c game.h constants.h fileman.o
	$(CC) main.c game.c fileman.o  -o pong $(CFLAGS_STRICT) 

fileman.o: fileman.c fileman.h
	$(CC) fileman.c  -c 

clean:
	rm -rf *.o pong *.dta
