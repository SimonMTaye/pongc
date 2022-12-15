#include "constants.h"
#include "game.h"
#include <curses.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define LEADERBOARD_ENTRIES 5

int MENU_SCALE = 1;
int MENU_Y_START;

vector_t half;
char *player = NULL;

void menu_start();
void leaderboard();
char *get_input(char *message, char* wanted);

int main(void)
{
    menu_start();
}

void init_menu_constants()
{
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
    MENU_Y_START = half.y - 3;
}

// Utility functions
void print_centered(int y, char *message)
{
    int length = strlen(message);
    move(y, half.x - (length / 2));
    printw("%s", message);
}

void clear_and_title()
{
    clear();
    flushinp();
    attron(A_BOLD);
    print_centered(half.y - 7, "PONG!");
    attroff(A_BOLD);
}

char *get_input(char *message, char* wanted)
{
    clear_and_title();
    int menu_items_start = half.y;
    if (message != NULL) 
        print_centered(menu_items_start, message);
    int l = strlen(wanted);
    char buff[l + 20];
    sprintf(buff, "Enter %s:", wanted);
    print_centered(menu_items_start + 2, buff);

    move(menu_items_start + 3, half.x - 4);
    int v = 0;
    int counter = 0;
    int size = 50;
    char *name = malloc(sizeof(char) * size);
    curs_set(2);
    while (v != '\n')
    {
        v = getch();
        if (v == ERR)
        {
            safe_error_exit(1, "Error reading char from keyboard");
        }
        if (v == KEY_BACKSPACE || v == '\b' || v == 127)
        {
            counter--;
            counter = max(0, counter);
            mvaddch(menu_items_start + 3, half.x - 4 + counter, ' ');
            move(menu_items_start + 3, half.x - 4 + counter);
            refresh();
            name[counter] = '\0';
            continue;
        }
        if (((char)v <= 'z') && ((char)v >= 'A'))
        {
            addch(v);
            if (counter == size)
            {
                size *= 2;
                name = realloc(name, sizeof(char) * size);
            }

            name[counter] = (char)v;
            counter++;
        }
    }
    // Add space for a null terminator
    if (counter == size)
    {
        name = realloc(name, sizeof(char) * (size + 1));
    }
    name[counter] = '\0';
    curs_set(0);
    return name;
}

void print_ip()
{
    clear();
    print_centered(half.y - 7, "PONG!");
    move(half.y + 1, half.x - 8);
    char ip[10];
    printw("IP: %s", ip);
}

void leaderboard()
{
    clear_and_title();
    print_centered(half.y - 5, "(B)ack");
    pong_file_t *file_ds = read_file();
    int entries_to_show = (LEADERBOARD_ENTRIES > file_ds->num_entries) ? file_ds->num_entries : LEADERBOARD_ENTRIES;
    // DISPLAY TOTAL WINS COLUMN
    sort_win_count(file_ds);
    move(half.y - 2, half.x - 30);
    printw("Total Wins");
    for (int i = 0; i < entries_to_show; i++)
    {
        move(half.y + i, half.x - 30);
        pong_file_entry_t entry = file_ds->entries[i];
        printw("%d: %s (%d win(s))", i + 1, entry.name, entry.won);
    }
    // DISPLAY WIN PERCENTAGE COLUMN
    sort_win_percentage(file_ds);
    move(half.y - 2, half.x + 20);
    printw("Win Percentage");
    for (int i = 0; i < entries_to_show; i++)
    {
        move(half.y + i, half.x + 20);
        pong_file_entry_t entry = file_ds->entries[i];
        printw("%d: %s (%.2f%%)", i + 1, entry.name, (entry.win_percentage * 100));
    }
    refresh();
    int v;
    while (true)
    {
        v = getch();
        switch (v)
        {
        case 'b':
        case 'B':
            return;
        case 'q':
        case 'Q':
            safe_error_exit(0, "user quit game");
        }
    }
}

void server_mode(char* player) {
    char* player_2 = strdup("Test");
    clear_and_title();
    print_centered(MENU_Y_START, "Waiting for connections...");
    refresh();
    bool success = listen_for_connections(5);
    if (success) { 
        run_server_mode(player, player_2, LEFT_PLAYER);
        return;
    }
    clear_and_title();
    print_centered(MENU_Y_START, "Connection Failed");
    refresh();
    sleep(3);
    return;
}

void client_mode(char* player) {
    char* player_2 = strdup("Test");
    char* server = get_input("Connect to Server", "Server Name");
    clear_and_title();
    print_centered(MENU_Y_START, "Trying to connect...");
    refresh();
    bool success; 
    int counter = 0;
    while (!success && counter < 10) {
        success = connect_to_pong(server);
        counter += 1;
        sleep(1);
    }
    if (success) {
        run_client_mode(player, player_2, RIGHT_PLAYER);
        return;
    }
    clear_and_title();
    print_centered(MENU_Y_START, "Connection Failed");
    refresh();
    sleep(3);
}

void local_game(char *player_1)
{
    char *player_2 = get_input("PLAYER 2 (RIGHT PADDLE)", "Your Name");
    run_local_mode(player_1, player_2);
}

void options()
{
    while (true)
    {
        init_menu_constants();
        clear_and_title();
        print_centered(MENU_Y_START, "(P)lay Localy");
        print_centered(MENU_Y_START + 1, "(C)lient Mode");
        print_centered(MENU_Y_START + 2, "(S)erver Mode");
        print_centered(MENU_Y_START + 3, "(L)eaderboard");
        print_centered(MENU_Y_START + 4, "(Q)uit");
        flushinp();
        refresh();
        int v;
        v = getch();
        switch (v)
        {
        case 'p':
        case 'P':
            local_game(player);
            break;
        case 'l':
        case 'L':
            leaderboard();
            break;
        case 'C':
        case 'c':
            client_mode(player);
            break;
        case 'S':
        case 's':
            server_mode(player);
            break;
        case 'q':
        case 'Q':
            safe_error_exit(0, "user quit game");
            return;
        }
        flushinp();
    }
}

void menu_start()
{
    init_curses();
    init_menu_constants();
    flushinp();
    clear_and_title();
    // Get the players name
    if (player == NULL)
        player = get_input("HELLO! WELCOME TO PONG!",  "Your Name");
    options();
}
