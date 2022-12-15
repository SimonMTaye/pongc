#include "networked.h"
#include "message.h"
#include "socket.h"
#include <pthread.h>
#include <stdio.h>
#include <sys/socket.h>

unsigned short PONG_PORT = 11384;

int SERVER_SOCKET_FD = -1;
int PONG_SOCKET_FD = -1;

void log_file(char* msg) {
    FILE* f = fopen("log.txt", "a");
    fprintf(f, "%s\n", msg);
    fclose(f);
}
// Transmit keystrokes from ncurses to connection
// Args should be a char
void send_key_stroke(int v)
{
    char buff[5];
    sprintf(buff, "%d", v);
    send_message(SERVER_SOCKET_FD, buff);
}

int get_key_stroke()
{
    char *mssg = receive_message(PONG_SOCKET_FD);
    return atoi(mssg);
}

// Transmit the entire game state to the
void send_game_state(int64_t state)
{
    char buff[5];
    sprintf(buff, "%ld", state);
    send_message(PONG_SOCKET_FD, buff);
}

int64_t get_new_game_state()
{
    char *mssg = receive_message(PONG_SOCKET_FD);
    return atoll(mssg);
}


void* listen_blocking() {
    while (SERVER_SOCKET_FD == -1)
    {
        SERVER_SOCKET_FD = server_socket_open(&PONG_PORT);
        usleep(10);
        
    }
    int res = listen(SERVER_SOCKET_FD, 1);
    if (res == -1) {
        log_file("[networked.c] could not listen for new connections");
        return NULL;
    }
    PONG_SOCKET_FD = server_socket_accept(SERVER_SOCKET_FD);
    if (PONG_SOCKET_FD != -1)
        log_file("[networked.c] connection accepted\n");
    if (PONG_SOCKET_FD == -1) 
        log_file("[networked.c] connection failed\n");
    return NULL;
}

bool listen_for_connections(int timeout_s)
{
    pthread_t thread;
    pthread_create(&thread, NULL, listen_blocking, NULL);
    sleep(timeout_s);
    pthread_cancel(thread);
    return (PONG_SOCKET_FD != -1);
}


bool connect_to_pong(char* server)
{
    PONG_SOCKET_FD = socket_connect(server, PONG_PORT);
    return (PONG_SOCKET_FD != -1);
}
