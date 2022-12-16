#include "game.h"
#include "constants.h"
#include <curses.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
/// @brief Reset all positions and speeds to initial values and redraw screen
void reset_game();
/// @brief Set scores to 0
void reset_score();
/// @brief Draw paddles, ball, borders and print score
void draw_all();

/// @brief  Encode the game state as a 32 bit int and return it
///         bit [0-3] left score
///         bit [4-7] right score
///         bit [8-11] left paddle y
///         bit [12-15] right paddle y
///         bit [16-27] ball x position
///         bit [28-31] ball y position
/// @return
uint32_t state_to_int32();

/// @brief Unpack 32 bit state and set game constants
/// @param state a 32 bit int that has state encoded as described in state_to_int32
void set_game_state(uint32_t state);
/// @brief Get game state and send it to the client instance
void send_state();

/// @brief Move the right/left paddle down if the right input is recieved
/// @param v user input
/// @param player flag indicating which player the input comes from. Used to avoid moving the incorrect paddle
void process_user_input(int v, int player);
/// @brief Move the ball to new coordinates as determined by its speed and poisiton. In case it collides with paddles or walls, reflect x or y speeds respectively.
///         Increment score if collides with the left or right walls (i.e. the goals)
void move_ball();
/// @brief Check if a player has a score over the threshold. If so, track the win in the stat file and show game over screen.
void check_for_game_over();
/// @brief Get time in miliseconds
int getmilis();

///            The following macros all unpack to draw_area calls with preconfigured values for erasing (draw area with blank ch) and drawing paddles, ball...
// Scaled and Offset aware area draws
#define draw_area_scaled_offset(starty, startx, endy, endx, ch) draw_area(((starty)*GAME_SCALE) + NS_OFFSET.y, ((startx)*GAME_SCALE) + NS_OFFSET.x, ((endy)*GAME_SCALE) + NS_OFFSET.y, ((endx)*GAME_SCALE) + NS_OFFSET.x, ch);
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

///                 Global variables
// Position Constants
vector_t ball_speed;
vector_t ball_position;
vector_t right_paddle;
vector_t left_paddle;
// Vector constants for determining board area. ns indcates no scale applied
vector_t NS_OFFSET;
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
char *left_player;
char *right_player;
// Ball movement control variable
int ball_move_interval = BALL_INTERVAL;

void init_curses()
{
    if (CURSES_INIT)
        return;
    CURSES_INIT = 1;
    // Init curses library
    initscr();
    // Do not wait for linebreaks before transmitting user input
    cbreak();
    // Do not echo characters to the screen
    noecho();
    CURSES_INIT = 1;
}
/// @brief  Initilize curses settings only used for the game and calculate GAME_SCALE and OFFSET variables
void init_game_constants()
{
    // Enable reading of the function and arrow keys
    keypad(stdscr, TRUE);
    // Set the character processing to be non-blocking
    nodelay(stdscr, true);
    // Hide the cursor from the user
    curs_set(0);
    // If the screen is too small, quit
    if (COLS < X_DIMEN | LINES < Y_DIMEN)
    {
        perror("Screen is too small\n");
        exit(1);
    }
    // -6 to account for some UI elements
    int y_scale = (LINES - 6) / Y_DIMEN;
    int x_scale = COLS / X_DIMEN;
    // Used to scale game on screens bigger than the games dimensions
    GAME_SCALE = min(x_scale, y_scale);
    // Determine the offset needed to center the pong board
    NS_OFFSET.y = (LINES - (Y_DIMEN * GAME_SCALE)) / 2;
    NS_OFFSET.x = (COLS - (X_DIMEN * GAME_SCALE)) / 2;
}
/// @brief Initialize ncurses, game constants, reset all variabes and draw stating board
/// @param left name of left player
/// @param right name of right player
void init_new_game(char *left, char *right)
{
    // Save player names
    left_player = left;
    right_player = right;
    // Init Curses
    init_curses();
    init_game_constants();
    // Reset game
    reset_score();
    reset_game();
    // Set RUN_GAME flag
    RUN_GAME = true;
    draw_all();
}

/// @brief Decrement right paddle poisiton and redraw it
void right_paddle_up()
{
    erase_right_paddle();
    right_paddle.y = max(right_paddle.y - 1, 0);
    draw_right_paddle();
}
/// @brief Increment right paddle poisiton and redraw it
void right_paddle_down()
{
    erase_right_paddle();
    right_paddle.y = min(right_paddle.y + 1, Y_DIMEN - PADDLE_HEIGHT);
    draw_right_paddle();
}
/// @brief Decrement left paddle poisiton and redraw it
void left_paddle_up()
{
    erase_left_paddle();
    left_paddle.y = max(left_paddle.y - 1, 0);
    draw_left_paddle();
}
/// @brief Increment left paddle poisiton and redraw it
void left_paddle_down()
{
    erase_left_paddle();
    left_paddle.y = min(left_paddle.y + 1, Y_DIMEN - PADDLE_HEIGHT);
    draw_left_paddle();
}

void run_local_mode(char *left, char *right)
{
    init_new_game(left, right);
    int last_move = getmilis();
    int last_inc = getmilis();
    int now;
    while (RUN_GAME)
    {
        now = getmilis();
        if ((now - last_move) > ball_move_interval)
        {
            last_move = now;
            move_ball();
        }
        // Decrement speed every BALL_INTERVAL ms
        if ((now - last_inc) > BALL_INTERVAL)
        {
            last_inc = now;
            ball_move_interval -= 10;
        }
        int v = getch();
        process_user_input(v, BOTH);
        usleep(SLEEP_INTERVAL);
    }
    endwin();
}

void run_server_mode(char *left, char *right, int player)
{
    if (player != RIGHT_PLAYER && player != LEFT_PLAYER)
    {
        safe_error_exit(1, "[game.c] server-mode: unexpected player value");
    }
    // pthread_t input_listener_thread;
    // pthread_create(&input_listener_thread, NULL, receive_user_input, NULL);
    init_new_game(left, right);
    int last_move = getmilis();
    int last_inc = getmilis();
    int now;
    while (RUN_GAME)
    {
        now = getmilis();
        if ((now - last_move) > ball_move_interval)
        {
            last_move = now;
            move_ball();
            send_state();
        }
        if ((now - last_inc) > BALL_INTERVAL)
        {
            last_inc = now;
            ball_move_interval -= 10;
            send_state();
        }
        int v = getch();
        process_user_input(v, player);
        v = get_next_int();
        process_user_input(v, BOTH);
    }
}

void run_client_mode(char *left, char *right, int player)
{
    if (player != RIGHT_PLAYER && player != LEFT_PLAYER)
    {
        safe_error_exit(1, "[game.c] client-mode: unexpected player value");
    }
    init_new_game(left, right);
    draw_all();
    while (RUN_GAME)
    {
        uint32_t new_state = get_new_game_state();
        if ((int) (new_state) != -1 && state_to_int32() != new_state) {
            set_game_state(new_state);
            check_for_game_over();
            clear();
            draw_all();
        }
        int v = getch();
        switch (v)
        {
        case LEFT_PADDLE_UP:
        case LEFT_PADDLE_DOWN:
            if (player == LEFT_PLAYER)
                send_int32(v);
            break;
        case RIGHT_PADDLE_UP:
        case RIGHT_PADDLE_DOWN:
            if (player == RIGHT_PLAYER)
                send_int32(v);
            break;
        case 'q':
            send_int32(v);
            return;
        }
    }
}
/// @brief Show a brief congratulations message to the winner and return
/// @param winner name of return
void game_over_screen(char *winner)
{
    clear();
    int center_offset_x = (strlen(winner) / 2) + 8;
    move(LINES / 2, (COLS / 2) - center_offset_x);
    printw("CONGRATULATIONS %s", winner);
    refresh();
    sleep(3);
    flushinp();
    return;
}
/// @brief Check if a player has a score over the threshold. If so, track the win in the stat file and show game over screen.
void check_for_game_over()
{
    // If scores are below threshold, no game over so return
    if ((right_score < MAX_SCORE) && (left_score < MAX_SCORE))
        return;
    char *winner = left_player;
    char *loser = right_player;
    if (right_score >= MAX_SCORE)
    {
        winner = right_player;
        loser = left_player;
    }
    pong_file_t *file_ds = read_file();
    add_win(file_ds, winner);
    add_loss(file_ds, loser);
    flush_to_file(file_ds);
    file_ds = NULL;
    RUN_GAME = false;
    game_over_screen(winner);
}
/// @brief Move the ball to new coordinates as determined by its speed and poisiton. In case it collides with paddles or walls, reflect x or y speeds respectively.
///         Increment score if collides with the left or right walls (i.e. the goals)
void move_ball()
{
    // Bounce on left paddle
    if ((ball_position.x) == (left_paddle.x + PADDLE_LENGTH) && ((ball_position.y + BALL_HEIGHT - 1) >= left_paddle.y) && (ball_position.y <= (left_paddle.y + PADDLE_HEIGHT - 1)))
    {
        ball_speed.x = ball_speed.x * -1;
    }
    // Bounce on right paddle
    if (((ball_position.x + BALL_LENGTH) == right_paddle.x) && ((ball_position.y + BALL_HEIGHT - 1) >= right_paddle.y) && (ball_position.y <= (right_paddle.y + PADDLE_HEIGHT - 1)))
    {
        ball_speed.x = ball_speed.x * -1;
        // In case the ball is about to fly off the board bounds, make it stick to the paddle
    }
    // Reflect ball y speed if it hits a wall
    if (ball_position.y == 0 || (ball_position.y == (Y_DIMEN - BALL_HEIGHT)))
        ball_speed.y = ball_speed.y * -1;
    // If the ball hits a wall on the left or right side, adjust the score and reset the game
    if (ball_position.x <= 0)
    {
        right_score += 1;
        reset_game();
        draw_all();
        check_for_game_over();
        return;
    }
    if (ball_position.x >= (X_DIMEN - BALL_LENGTH))
    {
        left_score += 1;
        reset_game();
        draw_all();
        check_for_game_over();
        return;
    }
    erase_ball();
    ball_position.y += ball_speed.y;
    ball_position.x += ball_speed.x;
    ball_position.y = bound(Y_DIMEN - BALL_HEIGHT, 0, ball_position.y);
    ball_position.x = bound(X_DIMEN - BALL_LENGTH, 0, ball_position.x);
    draw_ball();
}

void draw_area(int starty, int startx, int endy, int endx, chtype ch)
{
    for (int i = starty; i < endy; i++)
    {
        move(i, startx);
        for (int j = startx; j < endx; j++)
        {
            addch(ch);
        }
    }
}
/// @brief Fill non-board areas with BORDER_CH. Non Board area is determined by values in NS_OFFSET and SCALE as well as the boards dimensions defined in constatns.h
void draw_borders()
{
    int bottom_end = NS_OFFSET.y + (Y_DIMEN * GAME_SCALE);
    int right_end = NS_OFFSET.x + (X_DIMEN * GAME_SCALE);
    // LEFT CHUNK
    draw_area(0, 0, LINES, NS_OFFSET.x, BORDER_CH);
    // RIGHT CHUNK
    draw_area(0, right_end, LINES, COLS, BORDER_CH);
    // TOP CHUNK
    draw_area(0, 0, NS_OFFSET.y, COLS, BORDER_CH);
    // BOTTOM CHUNK
    draw_area(bottom_end, 0, LINES, COLS, BORDER_CH);
}
/// @brief Exit ncurses mode with status and message
/// @param status a status number
/// @param message exit message
void safe_error_exit(int status, char *message)
{
    endwin();
    if (message != NULL)
        perror(message);
    exit(status);
}
/// @brief Draw the score above the game board
void print_score()
{
    // Use low-level draw since score is off the game board
    draw_area(NS_OFFSET.y / 2, NS_OFFSET.x, (NS_OFFSET.y / 2) + 1, NS_OFFSET.x + (X_DIMEN * GAME_SCALE), ' ');
    move(NS_OFFSET.y / 2, (COLS / 2) - 3);
    attron(A_BOLD);
    printw("%d - %d", left_score, right_score);
    attroff(A_BOLD);
}
/// @brief Reset all the speed/position of the paddles and the ball to initial positions.
void reset_positions_and_speed()
{
    // Initialize ball constants
    ball_speed.y = 1;
    ball_speed.x = 1;
    ball_position.y = Y_DIMEN / 2;
    ball_position.x = X_DIMEN / 2;

    // Initialize paddle positions
    right_paddle.y = Y_DIMEN / 2;
    right_paddle.x = X_DIMEN - PADDLE_LENGTH;
    left_paddle.y = Y_DIMEN / 2;
    left_paddle.x = 0;
    ball_move_interval = BALL_INTERVAL;
}
/// @brief Set scores to 0
void reset_score()
{
    // Set score to 0
    right_score = 0;
    left_score = 0;
}

void draw_all()
{
    draw_borders();
    draw_left_paddle();
    draw_right_paddle();
    draw_ball();
    print_score();
}

void reset_game()
{
    reset_positions_and_speed();
    draw_all();
    clear();
    refresh();
}

int getmilis()
{
    struct timespec t;
    clock_gettime(CLOCK_REALTIME, &t);
    return (t.tv_sec * 1000) + lround(t.tv_nsec / 1e06);
}

uint32_t state_to_int32()
{
    uint32_t state = left_score;
    state = state << 4;
    state += right_score;
    state = state << 4;
    state += left_paddle.y;
    state = state << 4;
    state += right_paddle.y;
    state = state << 12;
    state += ball_position.x;
    state = state << 4;
    state += ball_position.y;
    return state;
}

void set_game_state(uint32_t state)
{
    left_score = (state & (0xF << 28)) >> 28;
    right_score = (state & (0xF << 24)) >> 24;
    left_paddle.y = (state & (0xF << 20)) >> 20;
    right_paddle.y = (state & (0xF << 16)) >> 16;
    ball_position.x = (state & (0xFFF << 4)) >> 4;
    ball_position.y = state & 0xF;
}

void send_state()
{
    uint32_t state = state_to_int32();
    send_int32(state);
}

void process_user_input(int v, int player)
{
    switch (v)
    {
    case 'q':
        RUN_GAME = false;
        return;
    case RIGHT_PADDLE_DOWN:
        if (player == RIGHT_PLAYER || player == BOTH)
        {
            right_paddle_down();
            break;
        }
    case RIGHT_PADDLE_UP:
        if (player == RIGHT_PLAYER || player == BOTH)
        {
            right_paddle_up();
            break;
        }
    case LEFT_PADDLE_DOWN:
        if (player == LEFT_PLAYER || player == BOTH)
        {
            left_paddle_down();
            break;
        }
    case LEFT_PADDLE_UP:
        if (player == LEFT_PLAYER || player == BOTH)
        {
            left_paddle_up();
            break;
        }
    }
}
