#include "constants.h"
#include "game.h"
#include <curses.h>

int MENU_SCALE = 1;
vector_t half;

void show_menu();

int main(void) {
    show_menu();
}


void init_menu_constants() {
    // Set the character processing to be non-blocking
    nodelay(stdscr, false);
    // Show cursor
    curs_set(1);
    half.x = COLS / 2;
    half.y = LINES / 2;
}

void print_centered(int y, char* message) {
    int length = strlen(message);
    move(y, half.x - (length / 2));
    printw("%s", message);
}

char* get_name() {
    print_centered(half.y - 7, "PONG!");
    print_centered(half.y + 2, "ENTER YOUR NAME:");
    move(half.y + 3, half.x - 4);
    int v = 0;
    int counter = 0;
    int size = 50;
    char* name = malloc(sizeof(char) * size);
    while (v != '\n') {
        v = getch();
        if (v == ERR) {
            safe_error_exit(1, "Error reading char from keyboard");
        }
        addch(v);
        if (counter == size) {
            size *= 2;
            name = realloc(name, sizeof(char) * size);
        }
        name[counter] = (char) v;
        counter ++;
    }
    return name;
}


void print_ip() {
    clear();
    print_centered(half.y - 7, "PONG!");
    move(half.y + 1, half.x - 8);
    char ip[10];
    printw("IP: %s", ip);
}


void show_options() {
    clear();
    print_centered(half.y - 7, "PONG!");
    print_centered(half.y - 3, "(P)lay");
    print_centered(half.y - 2, "(C)onnect");
    print_centered(half.y - 1, "(L)eaderboard");
    print_centered(half.y, "(Q)uit");
    flushinp();
    refresh();
    int v;
    while (true) {
        v = getch();
        switch (v) {
            case 'p':
                run_game();
            case 'q':
                safe_error_exit(0, "User quit game");
        }
    }
}

void show_menu() {
    init_curses();
    init_menu_constants();
    flushinp();
    char* name = get_name();
    //print_ip();
    show_options();
    
}
