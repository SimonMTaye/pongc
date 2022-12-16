#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "types.h"
/// File path used for stating game statistics
#define PONG_FILE_PATH "./stats.dta"
/// @brief Read data stored in the game stat file and return a pointer to pong_file_t* data structre
/// @return a pointer to a pong_file_t* data structure
pong_file_t *read_file();
/// @brief Save the input data structure to a file and free the data strcutre
/// @param file_ds input to save then free
void flush_to_file(pong_file_t *file_ds);
/// @brief Sort the entries in the data structure by win percentage
/// @param file_ds data structure
void sort_win_percentage(pong_file_t *file_ds);
/// @brief Sort the entries in the data structure by total win count
/// @param file_ds data structre
void sort_win_count(pong_file_t *file_ds);
/// @brief Add a win for the player. If the player doesn't already exist, it will be created
/// @param file_ds data strucure
/// @param name name of player
void add_win(pong_file_t *file_ds, char *player);
/// @brief Add a loss for the plyaer. If the player doesn't already exist, it will be created
/// @param file_ds data structure
/// @param name  name of the player
void add_loss(pong_file_t *file_ds, char *player);
