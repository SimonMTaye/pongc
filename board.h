#include "constants.h"
#include <ncurses.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <unistd.h>


// Generic Curses helper functions
void draw_area(int starty, int startx, int endy, int endx, chtype ch);
void init_curses();

// Start the game
void run_game();


// Paddle Controls
void right_paddle_up();
void right_paddle_down();
void left_paddle_up();
void left_paddle_down();
