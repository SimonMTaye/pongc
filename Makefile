CC = clang 
LINK_FLAGS = -lpthread -lncurses -lm
ERROR_FLAGS = -Werror -Wall -g

pong: main.c game.c game.h networked.h networked.c constants.h fileman.o
	$(CC) main.c game.c networked.c message.c fileman.o  -o pong  $(LINK_FLAGS) 

fileman.o: fileman.c fileman.h
	$(CC) fileman.c  -c 

clean:
	rm -rf *.o pong *.dta
