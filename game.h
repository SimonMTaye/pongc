#include "constants.h"
#include "types.h"
#include "fileman.h"
#include "networked.h"

#include <ncurses.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>

// Min-Max macros
#define max(X,Y) ((X) > (Y) ? (X) : (Y))
#define min(X,Y) ((X) < (Y) ? (X) : (Y))
#define bound(UP, DOWN, X) (max((min(UP, X)), DOWN)) 


#define DEBUGGING_ENABLED  0
#define LEFT_PLAYER 432
#define RIGHT_PLAYER 3553

// Generic Curses helper functions
void draw_area(int starty, int startx, int endy, int endx, chtype ch);
void init_curses();
void safe_error_exit(int status, char* message);

// Start the game
void run_local_mode(char* player_1, char* player_2);
void run_client_mode(char* player_1, char* player_2, int player);
void run_server_mode(char* player_1, char* player_2, int player);

// Paddle Controls
void right_paddle_up();
void right_paddle_down();
void left_paddle_up();
void left_paddle_down();
