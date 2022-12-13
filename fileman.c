#include "fileman.h"
#include <stdio.h>

pong_file_t* read_file() {
    pong_file_t* file_ds = malloc(sizeof(pong_file_t));
    FILE* file = fopen(PONG_FILE_PATH, "r");
    // First time creating file or failed to open. Either way, preceed like first time
    if (file == NULL) {
        file_ds->num_entries = 0;
        file_ds->entries = NULL;
        return file_ds;
    }
    int res = fread(&(file_ds->num_entries), sizeof(file_ds->num_entries), 1, file);
    if (res != 1) {
        char buffer[100];
        sprintf(buffer, "error reading game: expexted only one value to be read, actual: %d)", res);
        perror(buffer);
        exit(EXIT_FAILURE);
    }
    file_ds->entries = malloc(sizeof(pong_file_entry_t) * (file_ds->num_entries));
    res = fread(file_ds->entries, sizeof(pong_file_entry_t), file_ds->num_entries, file);
    if (res != file_ds->num_entries) {
        char buffer[100];
        sprintf(buffer, "error reading game: unexpected number of entries read (e: %d, a: %d)", 1, res);
        perror(buffer);
        exit(EXIT_FAILURE);
    }
    fclose(file);
    return file_ds;
}

void flush_to_file(pong_file_t *file_ds) {
    // If there are no entries to write, don't bother creating the file
    if (file_ds->num_entries == 0) return;
    FILE* file = fopen(PONG_FILE_PATH, "w");
    int res = fwrite(&(file_ds->num_entries), sizeof(int32_t), 1,file);
    if (res != 1) {
        char buffer[100];
        sprintf(buffer, "error writing game data to file (e: %d, a: %d)", 1, res);
        perror(buffer);
        exit(EXIT_FAILURE);
    }
    res = fwrite(file_ds->entries, sizeof(pong_file_entry_t), file_ds->num_entries, file);
    if (res != file_ds->num_entries) {
        char buffer[100];
        sprintf(buffer, "error writing data: number of entries (e: %d, a: %d)", file_ds->num_entries, res);
        perror(buffer);
        exit(EXIT_FAILURE);
    }
    free(file_ds->entries);
    free(file_ds);
    fclose(file);
}

void sort_win_count(pong_file_t* file_ds) {
    for (int i = 0; i < file_ds->num_entries; i++){
        int max_index = i;
        for (int j = i; j < file_ds->num_entries; j++){
            if (file_ds->entries[j].won > file_ds->entries[max_index].won) {
                max_index = j;
            }
        }
        if (max_index == i) continue;
        pong_file_entry_t temp = file_ds->entries[i];
        file_ds->entries[i] = file_ds->entries[max_index];
        file_ds->entries[max_index] = temp;
    }
}

void sort_win_percentage(pong_file_t* file_ds) {
    for (int i = 0; i < file_ds->num_entries; i++){
        int max_index = i;
        for (int j = i; j < file_ds->num_entries; j++){
            if (file_ds->entries[j].win_percentage > file_ds->entries[max_index].win_percentage) {
                max_index = j;
            }
        }
        if (max_index == i) continue;
        pong_file_entry_t temp = file_ds->entries[i];
        file_ds->entries[i] = file_ds->entries[max_index];
        file_ds->entries[max_index] = temp;
    }
}

pong_file_entry_t* add_player(pong_file_t* file_ds, char* name) {
    file_ds->num_entries++;
    if (file_ds->num_entries == 1) {
        file_ds->entries = malloc(sizeof(pong_file_entry_t));
    } else {
        file_ds->entries = realloc(file_ds->entries,  sizeof(pong_file_entry_t) * file_ds->num_entries);
    }
    pong_file_entry_t* new_entry = &(file_ds->entries[file_ds->num_entries - 1]);
    strncpy((char*) new_entry->name, name, strlen(name));
    new_entry->played = 1;
    return new_entry;
}

void add_win(pong_file_t* file_ds, char* name) {
    for (int i = 0; i < file_ds->num_entries; i++){
        if (strcmp(name, (char*) file_ds->entries[i].name) == 0) {
            file_ds->entries[i].played++;
            file_ds->entries[i].won++;
            file_ds->entries[i].win_percentage = (double) file_ds->entries[i].won / file_ds->entries[i].played;
            return;
        }
    }
    pong_file_entry_t* new_entry = add_player(file_ds, name);
    new_entry->won = 1;
    new_entry->win_percentage = 1;
}

void add_loss(pong_file_t* file_ds, char* name) {
    for (int i = 0; i < file_ds->num_entries; i++){
        if (strcmp(name, (char*) file_ds->entries[i].name) == 0) {
            file_ds->entries[i].played++;
            file_ds->entries[i].win_percentage = (double) file_ds->entries[i].won / file_ds->entries[i].played;
            return;
        }
    }
    pong_file_entry_t* new_entry = add_player(file_ds, name);
    new_entry->won = 0;
    new_entry->win_percentage = 0;
}
