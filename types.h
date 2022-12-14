#include <stdint.h>

#ifndef PONG_TYPES
#define PONG_TYPES

// Represent X,Y coordiantes
typedef struct vector {
    int16_t x;
    int16_t y;
} vector_t;

// Hold all the state that defines a game
typedef struct game_struct {
    vector_t ball_position;
    vector_t ball_speed;
    vector_t right_paddle;
    vector_t left_paddle;
    int16_t right_score;
    int16_t left_score;
} game_state_t;

// Represent a signle entry in the PONG stats file
typedef struct file_entry {
    uint8_t name[128];
    int32_t played;
    int32_t won;
    double win_percentage;
} pong_file_entry_t;
// Struct for a pong stat file
typedef struct file {
    int32_t num_entries;
    pong_file_entry_t* entries;
} pong_file_t;


#endif



