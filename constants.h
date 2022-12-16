#ifndef PONG_GLOBAL_CONSTANTS
#define PONG_GLOBAL_CONSTANTS

/************************** CONSTANTS USED BY THE GAME *******************************/
// Dimesions for the game and the paddle
#define X_DIMEN                 35              // The X width of the board
#define Y_DIMEN                 10              // The Y height of the board
#define PADDLE_HEIGHT           2               // Height of the paddle
#define PADDLE_LENGTH           1               // Width of the paddle
#define BALL_HEIGHT             1               // Height of the ball
#define BALL_LENGTH             1               // Length of the ball

// Characters used for drawing game entities
#define BALL_CH                 ACS_BOARD       // Character used to draw ball
#define PADDLE_CH               ACS_CKBOARD     // Character Used to draw the paddle
#define BORDER_CH               ACS_BULLET      // Character used to draw walls and the background

#define SCORE_LINE_LENGTH       X_DIMEN         // Width of score board
#define SLEEP_INTERVAL          5              // Sleep interval (ms) between checking for user input. (Reverse FPS)
#define BALL_INTERVAL           600             // How to long to wait before moving the ball (ms)

// Keys used for controlling the paddles
#define LEFT_PADDLE_UP          'w'          
#define LEFT_PADDLE_DOWN        's'        
#define RIGHT_PADDLE_UP         KEY_UP 
#define RIGHT_PADDLE_DOWN       KEY_DOWN

#define MAX_SCORE               5              // Number of points needed to win the game
/************************** CONSTANTS USED BY MENU *******************************/

#endif



