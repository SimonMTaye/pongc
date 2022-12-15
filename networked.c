#include "networked.h"
#include "message.h"
#include "socket.h"


int SERVER_SOCKET_FD = -1;
int CLIENT_SOCKET_FD = -1;
//
// Transmit keystrokes from ncurses to connection
// Args should be a char
void send_key_stroke(int v) {
    char buff[5];
    sprintf(buff, "%d", v);
    send_message(SERVER_SOCKET_FD, buff);
}

int get_key_stroke() {
    char* mssg = receive_message(CLIENT_SOCKET_FD);
    return atoi(mssg);
}

// Transmit the entire game state to the 
void send_game_state(int64_t state) {
    char buff[5];
    sprintf(buff, "%ld", state);
    send_message(CLIENT_SOCKET_FD, buff);
}

int64_t get_new_game_state() {
    char* mssg = receive_message(CLIENT_SOCKET_FD);
    return atoll(mssg);
}

void* connect_to_pong(void* args) {
    char* server = (char*) args;
    CLIENT_SOCKET_FD = socket_connect(server, PONG_PORT);
    return NULL;
}

void* listen_for_connections() {
    int* port = malloc(sizeof(unsigned short));
    *port = PONG_PORT;
    while (SERVER_SOCKET_FD == -1) {
        SERVER_SOCKET_FD = server_socket_open(port);
    }
    //CLIENT_SOCKET_FD = server_socket_accept(SERVER_SOCKET_FD);
    return NULL;
}

bool connected_client_mode() {
    return (CLIENT_SOCKET_FD != -1); 
}

bool connected_server_mode() {
    return (SERVER_SOCKET_FD != -1); 
}
