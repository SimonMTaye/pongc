#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define PONG_FILE_PATH "./stats.dta"


typedef struct file_entry {
    uint8_t name[128];
    int32_t played;
    int32_t won;
    double win_percentage;
} pong_file_entry_t;

typedef struct file {
    int32_t num_entries;
    pong_file_entry_t* entries;
} pong_file_t;

pong_file_t* read_file();
void flush_to_file(pong_file_t* file_ds);

void sort_win_percentage(pong_file_t* file_ds);
void sort_win_count(pong_file_t* file_ds);

void add_win(pong_file_t* file_ds, char* player);
void add_loss(pong_file_t* file_ds, char* player);
