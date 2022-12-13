#include "game.h"
#include "constants.h"
#include <curses.h>
#include <unistd.h>

void draw_area(int starty, int startx, int endy, int endx, chtype ch);
void draw_area_debug(int starty, int startx, int endy, int endx, chtype ch);
void reset_game();
void debug(char* message, int offset);
// Scaled and Offset aware area draws
//#define draw_area_offset(starty, startx, endy, endx, ch) draw_area(starty + NS_OFFSET.y, startx + NS_OFFSET.x, endy + NS_OFFSET.y, endx + NS_OFFSET.x, ch)
//#define draw_area_scaled(starty, startx, endy, endx, ch) draw_area(starty * GAME_SCALE, startx * GAME_SCALE, endy* GAME_SCALE, endx * GAME_SCALE, ch)
#define draw_area_scaled_offset(starty, startx, endy, endx, ch) draw_area_debug(((starty) * GAME_SCALE) + NS_OFFSET.y, ((startx) * GAME_SCALE) + NS_OFFSET.x, ((endy) * GAME_SCALE) + NS_OFFSET.y, ((endx) * GAME_SCALE) + NS_OFFSET.x, ch);

//  Erase Macros
#define erase_area(starty, startx, endy, endx) draw_area_scaled_offset(starty, startx, endy, endx, ' ')
#define erase_ball() erase_area((ball_position.y), (ball_position.x), (ball_position.y + BALL_HEIGHT), (ball_position.x + BALL_LENGTH))
#define erase_right_paddle() erase_area((right_paddle.y), (right_paddle.x), (right_paddle.y + PADDLE_HEIGHT), (right_paddle.x + PADDLE_LENGTH))
#define erase_left_paddle() erase_area((left_paddle.y), (left_paddle.x), (left_paddle.y + PADDLE_HEIGHT), (left_paddle.x + PADDLE_LENGTH))
// Draw Macros
#define draw_paddle(starty, startx) draw_area_scaled_offset((starty), (startx), (starty + PADDLE_HEIGHT), (startx + PADDLE_LENGTH), PADDLE_CH)
#define draw_ball() draw_area_scaled_offset((ball_position.y), (ball_position.x), (ball_position.y + BALL_HEIGHT), (ball_position.x + BALL_LENGTH), BALL_CH)
#define draw_right_paddle() draw_paddle((right_paddle.y), (right_paddle.x))
#define draw_left_paddle() draw_paddle((left_paddle.y), (left_paddle.x))


// Position Constants
vector_t ball_speed;
vector_t ball_position;
vector_t right_paddle;
vector_t left_paddle;

// Vector constants for determining board area. ns indcates no scale applied
vector_t NS_OFFSET;
vector_t NS_END;
// Scale for drawing game elements
int GAME_SCALE;

// Bool for checking if game should keep looping
bool RUN_GAME = true;

// Flags for checking initialization
int CURSES_INIT = 0;

// Score variables
int right_score = 0;
int left_score = 0;

// Name of the players for stat tracking
char* left_player;
char* right_player;

void init_curses() {
    if (CURSES_INIT) return;
    CURSES_INIT = 1;
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
    // -6 to account for some UI elements
    int y_scale = (LINES - 6) / Y_DIMEN;
    int x_scale = COLS / X_DIMEN;
    // Used to scale game on screens bigger than the games dimensions
    GAME_SCALE = min(x_scale, y_scale);
    // Determine the end coordinates of the board
    NS_END.x = X_DIMEN * GAME_SCALE;
    NS_END.y = Y_DIMEN * GAME_SCALE;
    // Determine the offset needed to center the pong board
    NS_OFFSET.y = (LINES - NS_END.y) / 2;
    NS_OFFSET.x = (COLS - NS_END.x) / 2;
    
}


void right_paddle_up() {
    erase_right_paddle();
    right_paddle.y = max(right_paddle.y - 1, 0);
    draw_right_paddle();
}

void right_paddle_down(){
    erase_right_paddle();
    right_paddle.y = min(right_paddle.y + 1, (Y_DIMEN - 1) - PADDLE_HEIGHT);
    draw_right_paddle();
}

void left_paddle_up() {
    erase_left_paddle();
    left_paddle.y = max(left_paddle.y - 1, 0);
    draw_left_paddle();
}

void left_paddle_down(){
    erase_left_paddle();
    left_paddle.y = min(left_paddle.y + 1, (Y_DIMEN - 1) - PADDLE_HEIGHT);
    draw_left_paddle();
}

void debug(char * message, int offset) {
    int y = (LINES - ((NS_OFFSET.y / 2) +1)) + offset;
    draw_area(y, 0, y+1, COLS, ' ');
    move(y, (COLS / 2) - (X_DIMEN/2));
    printw("%s", message);
    refresh();
}

void game_over_screen(char* winner) {
    clear();
    int center_offset_x = (strlen(winner) / 2) + 8;
    move(LINES / 2, (COLS / 2) - center_offset_x);
    printw("CONGRATULATIONS %s", winner);
    refresh();
    sleep(3);
    flushinp();
    return;
}

void check_for_game_over() {
    // If scores are below threshold, no game over so return
    if ((right_score < MAX_SCORE) && (left_score < MAX_SCORE)) return;
    char * winner = left_player;
    char * loser = right_player;
    if (right_score >= MAX_SCORE) {
        winner = right_player;
        loser = left_player;
    }
    pong_file_t* file_ds = read_file();
    add_win(file_ds, winner);  
    add_loss(file_ds, loser);  
    flush_to_file(file_ds);
    file_ds = NULL;
    RUN_GAME = false;
    game_over_screen(winner);
}

void move_ball() {
    erase_ball();
    ball_position.y += ball_speed.y;
    ball_position.x += ball_speed.x;
    ball_position.y = bound(Y_DIMEN - BALL_HEIGHT, 0, ball_position.y);
    ball_position.x = bound(X_DIMEN - BALL_LENGTH, 0, ball_position.x);
    // Bounce on left paddle
    if ((ball_position.x) == (left_paddle.x + PADDLE_LENGTH) &&  ((ball_position.y + BALL_HEIGHT - 1) >= left_paddle.y) && (ball_position.y <= (left_paddle.y + PADDLE_HEIGHT -  1))) {
        ball_speed.x = ball_speed.x * -1;
        // In case the ball is about to fly off the board bounds, make it stick to the paddle
        ball_position.x = max(left_paddle.x + PADDLE_LENGTH, ball_position.x);
    }
    // Bounce on right paddle
    if ( ((ball_position.x + BALL_LENGTH) == right_paddle.x) &&  ((ball_position.y + BALL_HEIGHT - 1) >= right_paddle.y) && (ball_position.y <= (right_paddle.y + PADDLE_HEIGHT - 1))) {
        ball_speed.x = ball_speed.x * -1;
        // In case the ball is about to fly off the board bounds, make it stick to the paddle
        ball_position.x = min(ball_position.x, right_paddle.x - BALL_LENGTH);
    }

    draw_ball();
    // Reflect ball y speed if it hits a wall
    if (ball_position.y == 0 || (ball_position.y == (Y_DIMEN -1 - BALL_HEIGHT))) ball_speed.y = ball_speed.y * -1;
    
    // If the ball hits a wall on the left or right side, adjust the score and reset the game
    if (ball_position.x <= 0) {
        right_score += 1;
        reset_game();
        check_for_game_over();
        return;
    }
    if (ball_position.x >= ((X_DIMEN - 1) - BALL_LENGTH)) {
        left_score += 1;
        reset_game();
        check_for_game_over();
        return;
    }
        
}

void draw_area_debug(int starty, int startx, int endy, int endx, chtype ch) {
    char* buff = malloc(sizeof(char) * 100);
    sprintf(buff, "[DRAW AREA] Y: (%d, %d), X: (%d, %d)", starty, endy, startx, endx);
    debug(buff, 0);
    free(buff);
    draw_area(starty, startx, endy, endx, ch);
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
    draw_area(NS_OFFSET.y / 2, NS_OFFSET.x, (NS_OFFSET.y /2) + 1, NS_OFFSET.x + (X_DIMEN * GAME_SCALE), ' ');
    move(NS_OFFSET.y / 2, (COLS / 2) - 3);
    printw("%d - %d", left_score, right_score);

}

void reset_positions() {
    // Initialize ball constants
    ball_speed.y = 1;
    ball_speed.x = 1;
    ball_position.y = Y_DIMEN / 2;
    ball_position.x = X_DIMEN / 2;
    
    // Initialize paddle positions
    right_paddle.y = Y_DIMEN  / 2;
    right_paddle.x = (X_DIMEN -1) - PADDLE_LENGTH;
    left_paddle.y = Y_DIMEN / 2;
    left_paddle.x = 0;

}

void reset_score() {
    // Set score to 0
    right_score = 0;
    left_score = 0;
}



void reset_game() {
    clear();
    reset_positions();
    draw_borders();
    draw_left_paddle();
    draw_right_paddle();
    draw_ball();
    print_score();
    char buff[100];
    sprintf(buff, "[CONSTANTS] (Y,X) NS_OFFSET: (%d, %d) NS_END: (%d, %d) GAME_SCALE: %d", NS_OFFSET.y, NS_OFFSET.x, NS_END.y, NS_END.x, GAME_SCALE);
    debug(buff, 2);
    refresh();
}

int getmilis() {
    struct timespec t;
    clock_gettime(CLOCK_REALTIME, &t);
    return (t.tv_sec * 1000) + lround(t.tv_nsec / 1e06);
}

void run_game(char* left, char* right) {
    // Save player names
    left_player = left;
    right_player = right;
    // Init Curses
    init_curses();
    init_game_constants();
    // Reset game
    reset_score();
    reset_game();
    int last = getmilis();
    int now; 
    RUN_GAME = true;
        while (RUN_GAME) {
        now = getmilis(); 
        if ((now - last) > BALL_MOVE_INTERVAL) {
            last = now;
            move_ball();
        }
        int v = getch();
        switch (v) {
            case LEFT_PADDLE_UP:
                left_paddle_up();
                break;
            case LEFT_PADDLE_DOWN:
                left_paddle_down();
                break;
            case RIGHT_PADDLE_UP:
                right_paddle_up();
                break;
            case RIGHT_PADDLE_DOWN:
                right_paddle_down();
                break;
            case 'q':
                return;
        }
        usleep(SLEEP_INTERVAL);
    }
    endwin();
}


