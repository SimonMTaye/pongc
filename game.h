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
#define max(X, Y) ((X) > (Y) ? (X) : (Y))
#define min(X, Y) ((X) < (Y) ? (X) : (Y))
#define bound(UP, DOWN, X) (max((min(UP, X)), DOWN))

// Enable Debug Output when set to 1
#define DEBUGGING_ENABLED 0

// Constants used to identify which player the current pong instance controls
// in networked mode
#define LEFT_PLAYER 432
#define RIGHT_PLAYER 3553
#define BOTH 732

// Generic Curses Utility functions
/// @brief Fill the rectangle defined by (starty, endy) and (startx, endx) with ch
/// @param starty The top y coordinate of the rectangle
/// @param startx The top x coordinate of the rectangle
/// @param endy The bottom y coordinate of the rectangle
/// @param endx The bottom x cooridnate of the rectangle
/// @param ch The character to fill the screen with
void draw_area(int starty, int startx, int endy, int endx, chtype ch);

/// @brief Initalize curses and screen settings
void init_curses();
void safe_error_exit(int status, char *message);

// Run a game
/// @brief Run a game of pong. Move ball every N miliseconds where N goes down until a player gets a point. Update paddle based on user input
/// @param left name of player controlling left paddle
/// @param right name of player controlling right paddle
void run_local_mode(char *player_1, char *player_2);
/// @brief Run a game in client mode. Transmit keystrokes that control the paddles to the server and update board whenever new state is recieved from the server
/// @param left name of left player
/// @param right name of the right player
/// @param player flag indicating which paddle the local instance should control
void run_client_mode(char *player_1, char *player_2, int player);
/// @brief Run the game in server mode. Similar to local mode except that only paddle is controlled by the current instance. The other paddle
///        is controlled by keystrokes from the client. Also update the client with the current game state
/// @param left name of the left player
/// @param right name of the right player
/// @param player a flag indicating which player the local instance of pong should control
void run_server_mode(char *player_1, char *player_2, int player);
