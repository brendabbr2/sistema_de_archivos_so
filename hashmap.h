#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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