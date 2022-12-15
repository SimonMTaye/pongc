#include <stdbool.h>
#include <stdint.h>


// Must return names eventually
bool connect_to_pong(char* server);
bool listen_for_connections(int timeout_s);

// Transmit keystrokes from ncurses to connection
// Args should be a char
void send_key_stroke(int v);
int get_key_stroke();

// Transmit the entire game state to the 
void send_game_state(int64_t state);
int64_t get_new_game_state();

bool connected_client_mode();
bool connected_server_mode();
