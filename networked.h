#include "message.h"
#include "types.h"
#include <stdbool.h>

#define PONG_PORT 11384

// Must return names eventually
void* connect_to_pong(void* args);
void* listen_for_connections();

// Transmit keystrokes from ncurses to connection
// Args should be a char
void send_key_stroke(int v);
int get_key_stroke();

// Transmit the entire game state to the 
void send_game_state(int64_t state);
int64_t get_new_game_state();

bool connected_client_mode();
bool connected_server_mode();
