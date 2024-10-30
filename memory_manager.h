#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#define MAX_BLOCKS 100
#define MAX_FILES 10
#define BLOCK_SIZE 512

typedef struct {
    char name[100];
    int size;
    int block_count;
    int blocks[MAX_BLOCKS];  // Índices de bloques asignados
    char data[BLOCK_SIZE * 10];  // Asumiendo que cada archivo puede tener varios bloques
} File;

typedef struct {
    int file_count;
    File files[MAX_FILES];  // Array de archivos
    int block_bitmap[MAX_BLOCKS];  // Bitmap para bloques libres y ocupados
} FileSystem;

// Prototipos de funciones
void init_filesystem(FileSystem* fs);
int create_file(FileSystem* fs, const char* name, int size);
int write_file(FileSystem* fs, const char* name, int offset, const char* data);
int read_file(FileSystem* fs, const char* name, int offset, int size);
int delete_file(FileSystem* fs, const char* name);
void list_files(FileSystem* fs);

#endif // MEMORY_MANAGER_H

