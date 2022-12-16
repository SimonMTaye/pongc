#include "networked.h"
#include "message.h"
#include "socket.h"
#include <errno.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <fcntl.h>

unsigned short PONG_PORT = 11384; int SERVER_SOCKET_FD = -1;
int PONG_SOCKET_FD = -1;

int buff[10];
int size = 0;

#define INPUT_BUF_SIZE 3


typedef struct buffer
{
    int buff[INPUT_BUF_SIZE];
    int first;
    int last;
    int size;
} input_buf_t;

bool input_thread = false;
input_buf_t *KEYSTROKE_BUFFER = NULL;
int32_t KEY_BUFFER = -1;

bool game_state_thread = false;
uint32_t GAME_STATE_BUFFER = -1;

/// @brief Return the first item in the buffer. -1 is returned if its empty
/// @param buf the buffer
/// @return first item in buffer
int get_next(input_buf_t *buf)
{
    if (buf->size == 0)
        return -1;
    buf->size--;
    int c = buf->buff[buf->first];
    if (buf->first == 0)
    {
        buf->first = INPUT_BUF_SIZE - 1;
    }
    else
    {
        buf->first--;
    }
    return c;
}
/// @brief Add an int to the buffer
/// @param buf buffer
/// @param v int
/// @return index of added int
int queue(input_buf_t *buf, int v)
{
    if (size == 10)
    {
        // Delete the last key
        buf->first = (buf->first + 1) % INPUT_BUF_SIZE;
        buf->size--;
    }
    buf->last = (buf->last + 1) % INPUT_BUF_SIZE;
    buf->buff[buf->last] = v;
    buf->size++;
    return buf->last;
}

/// @brief Send an int32 through the socket
/// @param v int representing a keystroke
void send_int32(int32_t v)
{
    char buff[5];
    sprintf(buff, "%d", v);
    send_message(PONG_SOCKET_FD, buff);
}
/// @brief listen for messages in a loop and queue them in the buffer
void *message_listner()
{
    while (PONG_SOCKET_FD != -1)
    {
        char *mssg = receive_message(PONG_SOCKET_FD);
        if (mssg != NULL)
        {
            queue(KEYSTROKE_BUFFER, atoi(mssg));
            
        }
    }
    return NULL;
}
/// @brief Return the key in the socket. Upto 10 keys may be queud. Return the first one.
int get_next_int()
{
    if (!input_thread)
    {
        KEYSTROKE_BUFFER = malloc(sizeof(input_buf_t));
        input_thread = true;
        pthread_t listener;
        pthread_create(&listener, NULL, message_listner, NULL);
    }
    return get_next(KEYSTROKE_BUFFER);
}

void* state_listenter() {
    while (PONG_SOCKET_FD != -1)
    {
        char *mssg = receive_message(PONG_SOCKET_FD);
        if (mssg != NULL)
        {
            GAME_STATE_BUFFER = atoi(mssg);
            
        }
    }
    return NULL;

}

uint32_t get_new_game_state()
{
    if (!game_state_thread)
    {
        game_state_thread = true;
        pthread_t listener;
        pthread_create(&listener, NULL, state_listenter, NULL);
    }
    return GAME_STATE_BUFFER;
}

/// @brief Open a server socket on PONG_PORT and wait for incoming connections. Set PONG_SOCKET_FD on succesfull connection
/// @return
void *listen_blocking()
{
    while (SERVER_SOCKET_FD == -1)
    {
        SERVER_SOCKET_FD = server_socket_open(&PONG_PORT);
        usleep(10);
    }
    int res = listen(SERVER_SOCKET_FD, 1);
    if (res == -1)
    {
        return NULL;
    }
    PONG_SOCKET_FD = server_socket_accept(SERVER_SOCKET_FD);
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

bool connect_to_pong(char *server)
{
    PONG_SOCKET_FD = socket_connect(server, PONG_PORT);
    return (PONG_SOCKET_FD != -1);
}
