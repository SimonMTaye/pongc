#include "constants.h"
#include <curses.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>

void init_curses();
void init_borders();
int draw_paddle(int starty, int startx);
int move_paddle(int starty, int startx, int y_offset);
void safe_error_exit(int status, char* message);
void* ballcontrol(void* args);
void draw_area(int starty, int startx, int endy, int endx, chtype ch);

#define draw_paddle_left(Y) (draw_paddle((Y), 0))
#define draw_paddle_right(Y) (draw_paddle((Y), (end.x - scale)))
#define move_paddle_left(Y, C) (move_paddle((Y), 0, (C)))
#define move_paddle_right(Y, C) (move_paddle((Y), (end.x - scale), (C)))
#define max(X,Y) ((X) > (Y) ? (X) : (Y))
#define min(X,Y) ((X) < (Y) ? (X) : (Y))

int scale;

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
    init_curses();
    init_borders();
    // Initiliaze ball coordinates
    ball_speed.x = scale;
    ball_speed.y = scale;
    ball_position.x = end.x / 2;
    ball_position.y = end.y / 2;
    int right_paddle = draw_paddle_right(0);
    int left_paddle = draw_paddle_left(0);
    int v;
    //pthread_t ball_thread;
    //pthread_create(&ball_thread, NULL, ballcontrol, NULL);
    do {
        v = getch();
        switch (v) {
            case KEY_UP:
                right_paddle = move_paddle_right(right_paddle, -scale);
                break;
            case KEY_DOWN:
                right_paddle = move_paddle_right(right_paddle, scale);
                break;
            case 'W':
            case 'w':
                left_paddle = move_paddle_left(left_paddle, -scale);
                break;
            case 'S':
            case 's':
                left_paddle = move_paddle_left(left_paddle, scale);
                break;               
        }
    } while (v != (int) 'q');
    endwin();
}

void init_curses() {
    // Init curses library
    initscr();
    // Do not wait for linebreaks before transmitting user input
    cbreak();
    // Do not echo characters to the screen
    noecho();
    // Enable reading of the function and arrow keys
    keypad(stdscr, TRUE);
    // If the screen is too small, quit
    if (COLS < X_DIMEN | LINES < Y_DIMEN) {
        perror("Screen is too small\n");
        exit(1);
    }
    // Initiliaze Variables used for measurement and coordinate computations
    scale = COLS / X_DIMEN;
    end.x = scale * X_DIMEN;
    end.y = scale * Y_DIMEN;
}

void init_borders() { 
    for (int i = end.y; i < LINES; i++) {
        if (i == end.y +DEBUG_LINE) continue;
        mvhline(i, 0, '*', COLS);
    } for (int i = end.x; i < COLS; i++) {
        mvvline(0, i, 0, LINES);
    }
}

void erase_paddle(int starty, int startx) {
    draw_area(starty, startx, starty + (scale*PADDLE_HEIGHT), startx + scale, ' ');
}

int draw_paddle(int starty, int startx) {
    int begin_y = min(max(0, starty), end.y - (scale * PADDLE_HEIGHT));
    move(end.y + DEBUG_LINE, 0);
    char buffer[50];
    sprintf(buffer, "MOVING PADDLE ID: %d TO %d                 ", startx,  begin_y);
    printw(buffer);
    draw_area(starty, startx, starty + (scale * PADDLE_HEIGHT), startx + scale, ACS_BLOCK);
    return begin_y;
}


int move_paddle(int starty, int startx, int y_offset) {
    // For each 'row', delete scale amount of chars
    erase_paddle(starty, startx);
    return draw_paddle(starty+y_offset , startx);
}

void draw_area(int starty, int startx, int endy, int endx, chtype ch) {
    for (int i = starty; i < endy; i++) {
        move(i+starty,startx);
        for (int j = startx; j < endx; j++) {
            addch(ch);
        }
    }
}


void safe_error_exit(int status, char* message) {
    endwin();
    if (message != NULL) perror(message);
    exit(status);
}

void draw_ball(int y, int x) {
    draw_area(y, x, y+scale, x+scale, ACS_BULLET);
}

void erase_ball(int y, int x) {
    draw_area(y, x, y+scale, x+scale, ' ');
}


void* ballcontrol(void* args) {
    while(true) {
        draw_ball(ball_position.y, ball_position.x);
        usleep(1000);
        erase_ball(ball_position.y, ball_position.x);
        ball_position.y += (ball_speed.y * scale);
        ball_position.x += (ball_speed.x * scale);
    }
    return NULL;
}
