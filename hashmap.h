#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ------------------ Estructuras -------------------
typedef struct {
    void** addresses;  // Arreglo de punteros (direcciones de memoria)
    size_t count;      // Número de direcciones almacenadas
} AddressArray;

typedef struct KeyValuePair {
    char* key;                   // Clave (nombre de la variable)
    AddressArray value;           // Valor (arreglo de direcciones de memoria)
    struct KeyValuePair* next;   // Siguiente par clave-valor en la lista
} KeyValuePair;

typedef struct {
    KeyValuePair* head;  // Apunta al primer par clave-valor
} HashMap;

// ----------------- Funciones ----------------------

// Inicializa un HashMap vacío
HashMap* create_hashmap() {
    HashMap* map = (HashMap*)malloc(sizeof(HashMap));
    map->head = NULL;
    return map;
}

// Crea un nuevo par clave-valor
KeyValuePair* create_pair(const char* key) {
    KeyValuePair* pair = (KeyValuePair*)malloc(sizeof(KeyValuePair));
    pair->key = strdup(key);  // Copia la clave
    pair->value.addresses = NULL;  // Inicialmente sin direcciones
    pair->value.count = 0;
    pair->next = NULL;
    return pair;
}

// Agrega una dirección de memoria al arreglo del par
void add_address(KeyValuePair* pair, void* address) {
    // Reasignar memoria para almacenar una nueva dirección
    pair->value.addresses = (void**)realloc(pair->value.addresses, 
                                    (pair->value.count + 1) * sizeof(void*));
    
    if (pair->value.addresses == NULL) {
        fprintf(stderr, "Error al reasignar memoria\n");
        exit(EXIT_FAILURE);
    }

    // Asignar la nueva dirección al arreglo
    pair->value.addresses[pair->value.count] = address;
    pair->value.count++;
}

// Agrega un par clave-valor al HashMap
void hashmap_insert(HashMap* map, const char* key, void* address) {
    // Buscar si ya existe un par con la clave
    KeyValuePair* current = map->head;
    while (current != NULL) {
        if (strcmp(current->key, key) == 0) {
            // Si la clave ya existe, agregar la dirección al arreglo
            add_address(current, address);
            return;
        }
        current = current->next;
    }

    // Si no existe la clave, crear un nuevo par
    KeyValuePair* new_pair = create_pair(key);
    add_address(new_pair, address);

    // Insertar al inicio de la lista
    new_pair->next = map->head;
    map->head = new_pair;
}

// Imprime el contenido del HashMap
void print_hashmap(HashMap* map) {
    KeyValuePair* current = map->head;
    while (current != NULL) {
        printf("Clave: %s\n", current->key);
        printf("Direcciones:\n");
        for (size_t i = 0; i < current->value.count; i++) {
            printf("  - 0x%lx\n", (unsigned long)current->value.addresses[i]);
        }
        printf("\n");
        current = current->next;
    }
}

// Imprime el contenido de una variable
void print_variable(const char* key, AddressArray* addresses) {
    printf("Clave: %s\n", key);
    printf("Direcciones:\n");
    for (size_t i = 0; i < addresses->count; i++) {
        printf("  - 0x%lx\n", (unsigned long)addresses->addresses[i]);
    }
    printf("\n");
}

// Libera toda la memoria usada por el HashMap
void free_hashmap(HashMap* map) {
    KeyValuePair* current = map->head;
    while (current != NULL) {
        KeyValuePair* next = current->next;
        free(current->key);
        free(current->value.addresses);
        free(current);
        current = next;
    }
    free(map);
}

// Busca una clave en el HashMap y devuelve el arreglo de direcciones
AddressArray* get_hashmap(HashMap* map, const char* key) {
    KeyValuePair* current = map->head;
    while (current != NULL) {
        if (strcmp(current->key, key) == 0) {
            return &current->value;
        }
        current = current->next;
    }
    return NULL;
}

// Borra una clave del HashMap
void delete_from_hashmap(HashMap* map, const char* key) {
    KeyValuePair* current = map->head;
    KeyValuePair* prev = NULL;
    while (current != NULL) {
        if (strcmp(current->key, key) == 0) {
            if (prev == NULL) {
                map->head = current->next;
            } else {
                prev->next = current->next;
            }
            free(current->key);
            free(current->value.addresses);
            free(current);
            return;
        }
        prev = current;
        current = current->next;
    }
}