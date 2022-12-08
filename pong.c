#include "constants.h"
#include <curses.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>

WINDOW* init_curses();
void init_borders();
int draw_paddle(WINDOW* win, int starty, int startx);
int move_paddle(WINDOW* win, int starty, int startx, int y_offset);
void safe_error_exit(int status, char* message);
void* ballcontrol(void* args);
void draw_area(WINDOW* win, int starty, int startx, int endy, int endx, chtype ch);

#define max(X,Y) ((X) > (Y) ? (X) : (Y))
#define min(X,Y) ((X) < (Y) ? (X) : (Y))

#define PADDLE_LEFT_X (0)
#define PADDLE_RIGHT_X (end.x - scale)

int scale;
int paddle_last;


bool reset = FALSE;

typedef struct vector {
    int x;
    int y;
} vector_t;

vector_t ball_speed;
vector_t ball_position;
vector_t end;

int main(void) {
    //setlocale(LC_ALL, "");
    WINDOW* game = init_curses();
    init_borders();
    wclear(game);
    touchwin(game);
    wnoutrefresh(game);
    doupdate();

    // MOVE TO INIT
    // Initiliaze ball coordinates
    ball_speed.x = scale;
    ball_speed.y = scale;
    ball_position.x = end.x / 2;
    ball_position.y = end.y / 2;
    // USE CONTANTS
    int right_paddle = (Y_DIMEN / 2);
    int left_paddle = right_paddle;
    right_paddle = draw_paddle(game, right_paddle, PADDLE_RIGHT_X);
    left_paddle = draw_paddle(game, left_paddle, PADDLE_LEFT_X);
    int v;
    //pthread_t ball_thread;
    //pthread_create(&ball_thread, NULL, ballcontrol, NULL);
    do {
        wnoutrefresh(game);
        doupdate();
        v = wgetch(game);
        switch (v) {
            case ERR:
                continue;
            case KEY_UP:
                right_paddle = move_paddle(game, right_paddle, PADDLE_RIGHT_X, (0 - scale));
                break;
            case KEY_DOWN:
                right_paddle = move_paddle(game, right_paddle, PADDLE_RIGHT_X, scale);
                break;
            case 'W':
            case 'w':
                left_paddle = move_paddle(game, left_paddle, PADDLE_LEFT_X, (0 -scale));
                break;
            case 'S':
            case 's':
                left_paddle = move_paddle(game, left_paddle, PADDLE_LEFT_X, scale);
                break;               
        }
    } while (v != (int) 'q');
    endwin();
}

WINDOW* init_curses() {
    // Init curses library
    initscr();
    // If the screen is too small, quit
    if (COLS < X_DIMEN | LINES < Y_DIMEN) {
        perror("Screen is too small\n");
        exit(1);
    }
    // Do not wait for linebreaks before transmitting user input
    cbreak();
    // Do not echo characters to the screen
    noecho();
    //
    // Initiliaze Variables used for measurement and coordinate computations
    scale = COLS / X_DIMEN;
    end.x = scale * X_DIMEN;
    end.y = scale * Y_DIMEN;
    paddle_last = end.y - (scale * PADDLE_HEIGHT);
    // height, width, starty, startx
    WINDOW* game = newwin(end.y, end.x, (LINES / 2) - (end.y - 2) , (COLS / 2) - (end.x - 2));
    // Enable reading of the function and arrow keys
    keypad(game, TRUE);
    /*
    move(end.y + DEBUG_LINE_START, 0);
    char buffer[100];
    sprintf(buffer, "SCALE: %d\t END.Y: %d\t END.X: %d\t PADDLE LAST: %d        ", scale, end.y, end.x, paddle_last);
    printw(buffer);
    */
    return game;
}

void init_borders() { 
    for (int i = 0; i < LINES; i++) {
        if (i <= end.y +DEBUG_LINE_END && i>= end.y + DEBUG_LINE_START) continue;
        mvhline(i, 0, '*', COLS);
    }     
    wnoutrefresh(stdscr);
}

void erase_paddle(WINDOW* win, int starty, int startx) {
    draw_area(win, starty, startx, starty + (scale*PADDLE_HEIGHT), startx + scale, ' ');
}

int draw_paddle(WINDOW* win, int starty, int startx) {
    int begin_y = min(max(0, starty), end.y - (scale * PADDLE_HEIGHT));
    /*
    move(end.y + DEBUG_LINE_END, 0);
    char buffer[100];
    sprintf(buffer, "DRAWING PADDLE ID: %d TO %d. VALUE RECIEVED WAS: %d     ", startx,  begin_y, starty);
    printw(buffer);
    */
    draw_area(win, begin_y, startx, begin_y + (scale * PADDLE_HEIGHT), startx + scale, ACS_BLOCK);
    return begin_y;
}


int move_paddle(WINDOW* win, int starty, int startx, int y_offset) {
    // For each 'row', delete scale amount of chars
    erase_paddle(win, starty, startx);
    /*
    move(end.y + DEBUG_LINE_END - 1, 0);
    char buffer[100];
    sprintf(buffer, "MOVING PADDLE ID: %d FROM %d WITH OFFSET %d         ", startx,  starty, y_offset);
    printw(buffer);
    */
    int new_y = starty+y_offset;
    new_y = draw_paddle(win, new_y, startx);
    return new_y;
}

void draw_area(WINDOW* win, int starty, int startx, int endy, int endx, chtype ch) {
    starty = max(starty, 0);
    startx = max(startx, 0);
    endy = min(endy, LINES);
    endx = min(endx, COLS);
    /*
    move(win, end.y + DEBUG_LINE_END-2, 0);
    char buffer[100];
    sprintf(buffer, "DRAWING BOX (START, END), Y: (%d, %d), X: (%d, %d)", starty,  endy, startx, endx);
    printw(buffer);
    */
    for (int i = starty; i < endy; i++) {
        wmove(win, i,startx);
        for (int j = startx; j < endx; j++) {
            waddch(win, ch);
        }
    }
}


void safe_error_exit(int status, char* message) {
    endwin();
    if (message != NULL) perror(message);
    exit(status);
}

void draw_ball(WINDOW* win, int y, int x) {
    draw_area(win, y, x, y+scale, x+scale, ACS_BLOCK);
}

void erase_ball(WINDOW* win, int y, int x) {
    draw_area(win, y, x, y+scale, x+scale, ' ');
}

// do boundary checking on the ball and modify its 
void bounce() {
    // Bounce on top or bottom wall:
    if ((ball_position.y < 0) || (ball_position.y > (end.y - scale))) {
        // reflect y speed, stick y position to be one unit back
        ball_speed.y = 0 - ball_speed.y;
        // current pos + (-) 2 units of movement
        ball_position.y = ball_position.y + (scale * 2 * ball_speed.y);
    } else if (ball_position.x < 0 || ball_position.x > (end.x - scale) ){
        // reflect y speed, stick y position to be one unit back
        ball_speed.x = 0 - ball_speed.x;
        // current pos + (-) 2 units of movement
        ball_position.x = ball_position.x + (scale * 2 * ball_speed.x);

    }
    // TODO Implement bounce on paddle
    // TODO Implement scoring when bounce on left/right wall
}


void* ballcontrol(void* args) {
    WINDOW* game = (WINDOW*) args;
    draw_ball(game, ball_position.y, ball_position.x);
    slk_noutrefresh();
    while(true) {
        usleep(500000);
        erase_ball(game,ball_position.y, ball_position.x);
        ball_position.y += (ball_speed.y * scale);
        ball_position.x += (ball_speed.x * scale);
        bounce();
        draw_ball(game, ball_position.y, ball_position.x);
        slk_noutrefresh();
    }
    return NULL;
}
