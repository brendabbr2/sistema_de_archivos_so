#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_BLOCKS 100
#define MAX_FILES 10
#define BLOCK_SIZE 512

// ------------------ Estructuras -------------------
#define HASHMAP_SIZE 256  // Tamaño del hashmap

typedef struct HashNode {
    char key[256];           // Nombre del archivo (clave)
    int file_index;          // Índice del archivo en el array de archivos
    struct HashNode *next;   // Colisión (lista enlazada)
} HashNode;

typedef struct {
    HashNode *buckets[HASHMAP_SIZE];  // Array de punteros a nodos
} HashMap;


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


// ----------------- Funciones ----------------------

// Función de hash simple basada en suma de caracteres
unsigned int hash(const char *key) {
    unsigned int hash_value = 0;
    while (*key) {
        hash_value += (unsigned char)(*key);
        key++;
    }
    return hash_value % HASHMAP_SIZE;
}

void hashmap_init(HashMap *map) {
    memset(map->buckets, 0, sizeof(map->buckets));
}

void hashmap_insert(HashMap *map, const char *key, int file_index) {
    unsigned int index = hash(key);
    HashNode *new_node = (HashNode *)malloc(sizeof(HashNode));  
    if (!new_node) {
        fprintf(stderr, "Error al asignar memoria para HashNode\n");
        exit(EXIT_FAILURE);
    }
    
    strcpy(new_node->key, key);
    new_node->file_index = file_index;
    new_node->next = map->buckets[index];
    map->buckets[index] = new_node;
}

int hashmap_get(HashMap *map, const char *key) {
    unsigned int index = hash(key);
    HashNode *node = map->buckets[index];

    while (node) {
        if (strcmp(node->key, key) == 0) {
            return node->file_index;
        }
        node = node->next;
    }
    return -1;  // No se encontró la clave
}

void hashmap_remove(HashMap *map, const char *key) {
    unsigned int index = hash(key);
    HashNode *node = map->buckets[index];
    HashNode *prev = NULL;

    while (node) {
        if (strcmp(node->key, key) == 0) {
            if (prev) {
                prev->next = node->next;
            } else {
                map->buckets[index] = node->next;
            }
            free(node);
            return;
        }
        prev = node;
        node = node->next;
    }
}

void hashmap_free(HashMap *map) {
    for (int i = 0; i < HASHMAP_SIZE; i++) {
        HashNode *node = map->buckets[i];
        while (node) {
            HashNode *temp = node;
            node = node->next;
            free(temp);
        }
    }
}

void init_filesystem(FileSystem *fs);
int create_file(FileSystem *fs, const char *name, int size);
int write_file(FileSystem *fs, const char *name, int offset, const char *data);
int read_file(FileSystem *fs, const char *name, int offset, int size);
int delete_file(FileSystem *fs, const char *name);
void list_files(FileSystem *fs);
void save_file_system(FileSystem *fs, const char *filename);
void load_file_system(FileSystem *fs, const char *filename);

#endif