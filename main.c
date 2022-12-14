#include "constants.h"
#include "game.h"
#include <curses.h>
#include <stdlib.h>

#define LEADERBOARD_ENTRIES 5

int MENU_SCALE = 1;

vector_t half;
char* player = NULL;

void menu_start();
void leaderboard();
char* get_name(char* message);

int main(void) {
    menu_start();
}

void init_menu_constants() {
    // Set the character processing to be non-blocking
    nodelay(stdscr, false);
    // Hide cursor
    curs_set(0);
    // Get keyboard input completely.
    keypad(stdscr, true);
    cbreak();
    // Init Half Width and Length to avoid recomputation
    half.x = COLS / 2;
    half.y = LINES / 2;
}

// Utility functions
void print_centered(int y, char* message) {
    int length = strlen(message);
    move(y, half.x - (length / 2));
    printw("%s", message);
}

void clear_and_title() {
    clear();
    flushinp();
    print_centered(half.y - 7, "PONG!");
}

char* get_name(char* message) {
    clear_and_title();
    int menu_items_start = half.y;
    print_centered(menu_items_start, message);
    print_centered(menu_items_start + 2, "ENTER YOUR NAME:");
    move(menu_items_start + 3, half.x - 4);
    int v = 0;
    int counter = 0;
    int size = 50;
    char* name = malloc(sizeof(char) * size);
    curs_set(2);
    while (v != '\n') {
        v = getch();
        if (v == ERR) {
            safe_error_exit(1, "Error reading char from keyboard");
        }
        if (v == KEY_BACKSPACE || v == '\b' || v == 127) {
            counter --;
            counter = max(0, counter);
            mvaddch(menu_items_start+3, half.x - 4 + counter, ' ');
            move(menu_items_start+3, half.x-4 + counter);
            refresh();
            name[counter]  = '\0';
            continue;
        }
        if (((char) v <= 'z') && ((char) v >= 'A')) {
            addch(v);
            if (counter == size) {
                size *= 2;
                name = realloc(name, sizeof(char) * size);
            }

            name[counter] = (char) v;
            counter ++;
        }
    }
    // Add space for a null terminator
    if (counter == size) {
        name = realloc(name,  sizeof(char) * (size + 1));
    }
    name[counter] = '\0';
    curs_set(0);
    return name;
}


void print_ip() {
    clear();
    print_centered(half.y - 7, "PONG!");
    move(half.y + 1, half.x - 8);
    char ip[10];
    printw("IP: %s", ip);
}


void leaderboard() {
    clear_and_title();
    print_centered(half.y - 5, "(B)ack");
    pong_file_t* file_ds = read_file();
    int entries_to_show = (LEADERBOARD_ENTRIES > file_ds->num_entries) ? file_ds->num_entries : LEADERBOARD_ENTRIES;
    // DISPLAY TOTAL WINS COLUMN
    sort_win_count(file_ds);
    move(half.y - 2, half.x - 30);
    printw("Total Wins");
    for (int i = 0; i < entries_to_show; i++) {
        move(half.y + i, half.x - 30);
        pong_file_entry_t entry = file_ds->entries[i];
        printw("%d: %s (%d win(s))", i + 1, entry.name, entry.won);
    }
    // DISPLAY WIN PERCENTAGE COLUMN
    sort_win_percentage(file_ds);
    move(half.y - 2, half.x + 20);
    printw("Win Percentage");
    for (int i = 0; i < entries_to_show; i++) {
        move(half.y + i, half.x + 20);
        pong_file_entry_t entry = file_ds->entries[i];
        printw("%d: %s (%.2f%)", i+ 1, entry.name, (entry.win_percentage * 100));
    }
    refresh();
    int v;
    while(true) {
        v = getch();
        switch (v) {
            case 'b':
            case 'B':
                return; 
            case 'q':
            case 'Q':
                safe_error_exit(0, "user quit game");
        }
    }
}

void local_game(char* player_1) {
    clear_and_title();
    char* player_2 = get_name("PLAYER 2 (RIGHT PADDLE)");
    // left, right
    run_local_game(player_1, player_2);
}

void options() {
    while (true) {
        init_menu_constants();
        clear_and_title();
        int menu_items_start = half.y - 3;
        print_centered(menu_items_start,   "(P)lay Localy");
        print_centered(menu_items_start+1, "(C)onnect and Play");
        print_centered(menu_items_start+2, "(L)eaderboard");
        print_centered(menu_items_start+3, "(Q)uit");
        flushinp();
        refresh();
        int v;
        v = getch();
        switch (v) {
            case 'p':
            case 'P':
                local_game(player);
                break;
            case 'l':
            case 'L':
                leaderboard();
                break;
            case 'q':
            case 'Q':
                safe_error_exit(0, "user quit game");
                return;
        }
        flushinp();
    }
}

void menu_start() {
    init_curses();
    init_menu_constants();
    flushinp();
    clear_and_title();
    // Get the players name
    if (player == NULL) player = get_name("HELLO! WELCOME TO PONG!");
    options();
}
