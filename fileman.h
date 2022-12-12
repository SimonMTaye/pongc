#include <stdio.h>
#include <stdint.h>

#define FILE_PATH "./stats.dta"

typedef struct entry {
    char name[100];
    double win_percentage;
    int games_won;
} player_stat_entry;

typedef struct header {
    int16_t players;
    int64_t file_size;
} pong_file_header_t;

typedef struct file_entry {
    uint8_t name[100];
    int16_t played;
    int16_t won;
} pong_file_entry_t;

typedef struct file {
    pong_file_header_t* header;
    pong_file_entry_t* entries;
}pong_file_t;
