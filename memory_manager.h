#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_BLOCKS 128 // Tamaño máximo para los bloques
#define BLOCK_SIZE 256 // Tamaño de bloque 
#define MAX_FILES 100 // Número máximo de archivos

typedef struct {
    char name[32];
    int size;
    int block_count;
    int blocks[MAX_BLOCKS]; // Índices de bloques ocupados
    char data[MAX_BLOCKS * BLOCK_SIZE]; // Almacenamiento de datos
} File;

typedef struct {
    File files[MAX_FILES];
    int file_count;
    char block_bitmap[MAX_BLOCKS]; // Bitmap para bloques
} FileSystem;

// Prototipos de funciones
void init_filesystem(FileSystem* fs);
int create_file(FileSystem* fs, const char* name, int size);
int write_file(FileSystem* fs, const char* name, int offset, const char* data);
int read_file(FileSystem* fs, const char* name, int offset, int size);
int delete_file(FileSystem* fs, const char* name);
void list_files(FileSystem* fs);

#endif // MEMORY_MANAGER_H

