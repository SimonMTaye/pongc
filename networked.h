#include <stdbool.h>
#include <stdint.h>

// Must return names eventually
bool connect_to_pong(char *server);
bool listen_for_connections(int timeout_s);

// Transmit the entire game state to the
/// @brief wait for next game state and return
/// @return game state
uint32_t get_new_game_state();

/// @brief try to connect to the serverr
/// @param server to connect to
/// @return a bool indicating whether a connection was sucessfully established
bool connect_to_pong(char *server);
/// @brief Listen for client connections
/// @param timeout_s determines how long to listen
bool listen_for_connections(int timeout_s);
/// @brief Return the key in the socket. Upto 10 keys may be queud. Return the first one.
int get_next_int();
/// @brief Send an int32 through the socket
/// @param v int representing a keystroke
void send_int32(int32_t v);