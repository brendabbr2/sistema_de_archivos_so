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

#define HASHNODE_MAX 1024  // Número máximo de nodos permitidos

typedef struct HashNode {
    char key[256];           // Nombre del archivo (clave)
    int file_index;          // Índice del archivo en el array de archivos
    int next_index;          // Índice del siguiente nodo en el array, -1 si no hay siguiente
} HashNode;

typedef struct {
    HashNode nodes[HASHNODE_MAX];  // Array de nodos
    int buckets[HASHMAP_SIZE];     // Índices a los primeros nodos de cada bucket
    int node_count;                // Contador de nodos utilizados
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
    memset(map->buckets, -1, sizeof(map->buckets));
    map->node_count = 0;
}

void hashmap_insert(HashMap *map, const char *key, int file_index) {
    if (map->node_count >= HASHNODE_MAX) {
        fprintf(stderr, "Error: Excedido el límite de nodos en HashMap.\n");
        return;
    }

    unsigned int index = hash(key);
    int new_node_index = map->node_count++;

    // Inicializa el nuevo nodo
    strcpy(map->nodes[new_node_index].key, key);
    map->nodes[new_node_index].file_index = file_index;
    map->nodes[new_node_index].next_index = map->buckets[index];

    // Apunta el bucket al nuevo nodo
    map->buckets[index] = new_node_index;
}

int hashmap_get(HashMap *map, const char *key) {
    unsigned int index = hash(key);
    int node_index = map->buckets[index];

    while (node_index != -1) {
        if (strcmp(map->nodes[node_index].key, key) == 0) {
            return map->nodes[node_index].file_index;
        }
        node_index = map->nodes[node_index].next_index;
    }
    return -1;
}

void hashmap_remove(HashMap *map, const char *key) {
    unsigned int index = hash(key);
    int *node_index = &map->buckets[index];
    int prev_index = -1;

    while (*node_index != -1) {
        if (strcmp(map->nodes[*node_index].key, key) == 0) {
            if (prev_index != -1) {
                map->nodes[prev_index].next_index = map->nodes[*node_index].next_index;
            } else {
                map->buckets[index] = map->nodes[*node_index].next_index;
            }
            return;
        }
        prev_index = *node_index;
        node_index = &map->nodes[*node_index].next_index;
    }
}

void hashmap_free(HashMap *map) {
    // Restablece cada bucket a -1, indicando que no hay nodos
    memset(map->buckets, -1, sizeof(map->buckets));
    
    // Reinicia el contador de nodos a 0
    map->node_count = 0;
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