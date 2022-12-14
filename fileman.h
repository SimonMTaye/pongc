#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "types.h"

#define PONG_FILE_PATH "./stats.dta"

pong_file_t* read_file();
void flush_to_file(pong_file_t* file_ds);

void sort_win_percentage(pong_file_t* file_ds);
void sort_win_count(pong_file_t* file_ds);

void add_win(pong_file_t* file_ds, char* player);
void add_loss(pong_file_t* file_ds, char* player);
