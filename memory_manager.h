#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hashmap.h"

#define MAX_BLOCKS 100
#define MAX_FILES 10
#define BLOCK_SIZE 512

// Estructura para un archivo
typedef struct {
    char name[256];
    int size;
    int block_count;
    int blocks[MAX_BLOCKS];
    char data[MAX_BLOCKS * BLOCK_SIZE];
} File;

typedef struct {
    File files[MAX_FILES];
    int file_count;
    int block_bitmap[MAX_BLOCKS];
    HashMap file_map;  // Hashmap para acceder rápidamente a los archivos por nombre
} FileSystem;

void init_filesystem(FileSystem *fs);
int create_file(FileSystem *fs, const char *name, int size);
int write_file(FileSystem *fs, const char *name, int offset, const char *data);
int read_file(FileSystem *fs, const char *name, int offset, int size);
int delete_file(FileSystem *fs, const char *name);
void list_files(FileSystem *fs);
void save_file_system(FileSystem *fs, const char *filename);
void load_file_system(FileSystem *fs, const char *filename);

#endif