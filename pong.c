#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

#include "constants.h"

void init_curses();
void init_borders();
void draw_paddle_left(int paddle_start);
void draw_paddle_right(int paddle_start);
void safe_error_exit(int status, char* message);

int x_scale;
int y_scale;

int main(void) {
    //setlocale(LC_ALL, "");
    init_curses();
    if (COLS < X_DIMEN | LINES < Y_DIMEN) {
        perror("Screen is too small\n");
        exit(1);
    }
    int x_scale = COLS / X_DIMEN;
    int y_scale = LINES / Y_DIMEN;
    init_borders();
    draw_paddle_left(0);
    draw_paddle_right(0);
    refresh();
    int v;
    do {
        v = getch();
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
}

void init_borders() {
    int x_rem = COLS % X_DIMEN;
    int y_rem = LINES % Y_DIMEN;
    attron(A_BOLD);
    attron(A_VERTICAL);
    for (int i= 0; i < y_rem; i++) {
        mvhline(LINES - i, 0, '*', COLS);
    } for (int i= 0; i < x_rem; i++) {
        mvvline(0, COLS - i, 0, LINES);
    }
    attroff(A_BOLD);
    attroff(A_VERTICAL);
    move(0,0);
}

void draw_paddle_left(int paddle_start) {
    // Add thickness to the paddle
    int y = paddle_start;
    attron(A_BOLD);
    attron(A_VERTICAL);
    for (int i = 1; i < x_scale; i++) {
        //mvvline(paddle_start, i-1, 0, y_scale * PADDLE_HEIGHT);
        mvvline(y, i-1, 0, LINES);
    }
    mvvline(y, 5, ']', y_scale * PADDLE_HEIGHT);
    attroff(A_BOLD);
    attroff(A_VERTICAL);
}

void draw_paddle_right(int paddle_start) {
    // Determine what coordine the right paddle should be drawn at
    // Do this in a global constant to avoid recomputation
    attron(A_BOLD);
    attron(A_VERTICAL);
    int x_end = COLS - (COLS % X_DIMEN);
    int res;
    for (int i = 1; i < x_scale; i++) {
        res = mvvline(paddle_start, (x_end) - (i-1), '|', y_scale * PADDLE_HEIGHT);
        if (res) safe_error_exit(res, "error drawing paddle\n");
    }
    res = mvvline(paddle_start, x_end - x_scale, '[', y_scale * PADDLE_HEIGHT);
    if (res) safe_error_exit(res, "error drawing paddle\n");
    attroff(A_BOLD);
    attroff(A_VERTICAL);
}

void safe_error_exit(int status, char* message) {
    endwin();
    if (message != NULL) perror(message);
    exit(status);
}
