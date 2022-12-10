#include "board.h"
#include "constants.h"
#include <ncurses.h>
#include <stdbool.h>
#include <stdint.h>
#include <time.h>
#include <unistd.h>


typedef struct vector {
    int16_t x;
    int16_t y;
} vector_t;

void draw_area(int starty, int startx, int endy, int endx, chtype ch);
void reset_game();

// Min-Max macros
#define max(X,Y) ((X) > (Y) ? (X) : (Y))
#define min(X,Y) ((X) < (Y) ? (X) : (Y))

// Scaled and Offset aware area draws
#define draw_area_offset(starty, startx, endy, endx, ch) draw_area(starty + NS_OFFSET.y, startx + NS_OFFSET.x, endy + NS_OFFSET.y, endx + NS_OFFSET.x, ch)
#define draw_area_scaled(starty, startx, endy, endx, ch) draw_area(starty * SCALE, startx * SCALE, endy* SCALE, endx * SCALE, ch)
#define draw_area_scaled_offset(starty, startx, endy, endx, ch) draw_area_offset(starty * SCALE, startx * SCALE, endy * SCALE, endx * SCALE, ch);

//  Erase Macros
#define erase_area(starty, startx, endy, endx) draw_area_scaled_offset(starty, startx, endy, endx, ' ')
#define erase_ball() erase_area(ball_position.y, ball_position.x, ball_position.y + BALL_HEIGHT, ball_position.x + BALL_LENGTH)
#define erase_right_paddle() erase_area(right_paddle.y, right_paddle.x, right_paddle.y + PADDLE_HEIGHT, right_paddle.x + PADDLE_LENGTH)
#define erase_left_paddle() erase_area(left_paddle.y, left_paddle.x, left_paddle.y + PADDLE_HEIGHT, left_paddle.x + PADDLE_LENGTH)
// Draw Macros
#define draw_paddle(starty, startx) draw_area_scaled_offset(starty, startx, starty + PADDLE_HEIGHT, startx + PADDLE_LENGTH, PADDLE_CH)
#define draw_ball() draw_area_scaled_offset(ball_position.y, ball_position.x, ball_position.y + BALL_HEIGHT, ball_position.x + BALL_LENGTH, BALL_CH)
#define draw_right_paddle() draw_paddle(right_paddle.y, right_paddle.x)
#define draw_left_paddle() draw_paddle(left_paddle.y, left_paddle.x)


// Position Constants
vector_t ball_speed;
vector_t ball_position;
vector_t right_paddle;
vector_t left_paddle;

// Vector constants for determining board area. ns indcates no scale applied
vector_t NS_OFFSET;
vector_t NS_END;

int SCALE;

// Flags for checking initialization
int CURSES_INIT = 0;

// Score variables
int right_score = 0;
int left_score = 0;

void init_curses() {
    // Init curses library
    initscr();
    // Do not wait for linebreaks before transmitting user input
    cbreak();
    // Do not echo characters to the screen
    noecho();
    // Enable reading of the function and arrow keys
    keypad(stdscr, TRUE);
    CURSES_INIT = 1;
}

void init_game_constants() {
    // Set the character processing to be non-blocking
    nodelay(stdscr, true);
    // Hide the cursor from the user
    curs_set(0);
    // If the screen is too small, quit
    if (COLS < X_DIMEN | LINES < Y_DIMEN) {
        perror("Screen is too small\n");
        exit(1);
    }
    // Used to scale game on terminals significantly bigger than the game dimensions
    SCALE = 1;
    // Center the game
    NS_OFFSET.y = (LINES - Y_DIMEN) / 2;
    NS_OFFSET.x = (COLS - X_DIMEN) / 2;

    // Initialize ball constants
    ball_speed.y = 1;
    ball_speed.x = 1;
    ball_position.y = Y_DIMEN / 2;
    ball_position.x = X_DIMEN / 2;
    
    // Initialize paddle positions
    right_paddle.y = Y_DIMEN / 2;
    right_paddle.x = (X_DIMEN -1) - PADDLE_LENGTH;

    left_paddle.y = Y_DIMEN / 2;
    left_paddle.x = 0;
}


void right_paddle_up() {
    erase_right_paddle();
    right_paddle.y = max(right_paddle.y - 1, 0);
    draw_right_paddle();
}

void right_paddle_down(){
    erase_right_paddle();
    right_paddle.y = min(right_paddle.y + 1, Y_DIMEN - PADDLE_HEIGHT);
    draw_right_paddle();
}

void left_paddle_up() {
    erase_left_paddle();
    left_paddle.y = max(left_paddle.y - 1, 0);
    draw_left_paddle();
}

void left_paddle_down(){
    erase_left_paddle();
    left_paddle.y = min(left_paddle.y + 1, Y_DIMEN - PADDLE_HEIGHT);
    draw_left_paddle();
}

void move_ball() {
    // Reflect ball y speed if it hits a wall
    if (ball_position.y == 0 || (ball_position.y == (Y_DIMEN - BALL_HEIGHT))) ball_speed.y = ball_speed.y * -1;
    // If the ball hits the left or right wall, change score and return
    if (ball_position.x == 0) {
        right_score += 1;
        reset_game();
        return;
    }
    if (ball_position.x == ((X_DIMEN - 1) - BALL_LENGTH)) {
        left_score += 1;
        reset_game();
        return;
    }
    //
    if (((ball_position.x - 1) == left_paddle.x - PADDLE_LENGTH) &&  ((ball_position.y - BALL_HEIGHT) >= left_paddle.y) && (ball_position.y <= (left_paddle.y + PADDLE_HEIGHT))) {
        ball_speed.x = ball_speed.x * -1;
    }
    if (((ball_position.x + BALL_LENGTH) == right_paddle.x -1) &&  ((ball_position.y - BALL_HEIGHT) >= right_paddle.y) && (ball_position.y <= (right_paddle.y + PADDLE_HEIGHT))) {
        ball_speed.x = ball_speed.x * -1;
    }
    erase_ball();
    ball_position.y += ball_speed.y;
    ball_position.x += ball_speed.x;
    draw_ball();
    
}

void draw_area(int starty, int startx, int endy, int endx, chtype ch) {
    for (int i = starty; i < endy; i++) {
        move(i,startx);
        for (int j = startx; j < endx; j++) {
            addch( ch);
        }
    }
}

void draw_borders() {
    draw_area(0, 0, LINES, NS_OFFSET.x, BORDER_CH);
    draw_area(0, (COLS -1) - NS_OFFSET.x, LINES, COLS, BORDER_CH);
    draw_area(0, 0, NS_OFFSET.y, COLS, BORDER_CH);
    draw_area((LINES - 1) - NS_OFFSET.y, 0, LINES, COLS, BORDER_CH);
}

void safe_error_exit(int status, char* message) {
    endwin();
    if (message != NULL) perror(message);
    exit(status);
}

void print_score() {
    // Use low-level draw since score is off the game board
    draw_area(NS_OFFSET.y / 2, NS_OFFSET.x, (NS_OFFSET.y /2) + 1, NS_OFFSET.x + X_DIMEN, ' ');
    move(NS_OFFSET.y / 2, (COLS / 2) - 3);
    printw("%d - %d", left_score, right_score);

}

void reset_score() {
    right_score = 0;
    left_score = 0;
}

void reset_game() {
    if (!CURSES_INIT) init_curses();
    clear();
    init_game_constants();
    draw_borders();
    draw_left_paddle();
    draw_right_paddle();
    draw_ball();
    print_score();
    refresh();
}

void run_game() {
    reset_score();
    reset_game();
    time_t last = time(NULL);
    while (true) {
        time_t now = time(NULL);
        if (last - now) move_ball();
        int v = getch();
        switch (v) {
            case 'w':
                left_paddle_up();
                break;
            case 's':
                left_paddle_down();
                break;
            case KEY_UP:
                right_paddle_up();
                break;
            case KEY_DOWN:
                right_paddle_down();
                break;
            // TODO: Remove
            case 'q':
                safe_error_exit(1, "User hit q\n");
                break;
        }
        last = now;
        usleep(20);
    }
    endwin();
}


