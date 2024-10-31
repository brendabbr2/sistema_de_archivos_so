#include "memory_manager.h"
#include <stdbool.h>

// Función para contar la cantidad de bytes usados por cada carácter en data, excluyendo \0
int count_non_null_bytes(File *file) {
    int byte_count = 0;

    // Recorremos hasta file->size, que es el tamaño real del contenido en data
    for (int i = 0; i < file->size; i++) {
        if (file->data[i] != '\0') {  // Solo contamos si el byte no es '\0'
            byte_count++;
        }
    }
    
    return byte_count;
}

// memory_manager.c
int allocate_blocks(FileSystem *fs, int blocks_needed, int *allocated) {
    int count = 0;
    for (int i = 0; i < MAX_BLOCKS && count < blocks_needed; i++) {
        if (fs->block_bitmap[i] == 0) {
            fs->block_bitmap[i] = 1;
            allocated[count++] = i;
        }
    }
    return (count == blocks_needed) ? 0 : -1;
}

void load_file_system(FileSystem *fs, const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        perror("Error al abrir el archivo para cargar el sistema de archivos");
        return;
    }

    // Cargar la estructura completa del FileSystem
    fread(fs, sizeof(FileSystem), 1, file);

    // Cargar la cantidad de nodos en el hashmap
    fread(&fs->file_map.node_count, sizeof(int), 1, file);

    // Cargar los nodos en el array `nodes`
    fread(fs->file_map.nodes, sizeof(HashNode), fs->file_map.node_count, file);

    // Cargar el array `buckets`
    fread(fs->file_map.buckets, sizeof(int), HASHMAP_SIZE, file);

    fclose(file);
}

void save_file_system(FileSystem *fs, const char *filename) {
    FILE *file = fopen(filename, "wb");
    if (!file) {
        perror("Error al abrir el archivo para guardar el sistema de archivos");
        return;
    }

    // Guardar la estructura completa del FileSystem
    fwrite(fs, sizeof(FileSystem), 1, file);

    // Guardar la cantidad de nodos en el hashmap
    fwrite(&fs->file_map.node_count, sizeof(int), 1, file);

    // Guardar los nodos en el array `nodes`
    fwrite(fs->file_map.nodes, sizeof(HashNode), fs->file_map.node_count, file);

    // Guardar el array `buckets`
    fwrite(fs->file_map.buckets, sizeof(int), HASHMAP_SIZE, file);

    fclose(file);
}

void init_filesystem(FileSystem *fs) {
    fs->file_count = 0;
    memset(fs->block_bitmap, 0, sizeof(fs->block_bitmap));
    hashmap_init(&fs->file_map);  // Inicializar el hashmap
}

int create_file(FileSystem *fs, const char *name, int size) {
    if (fs->file_count >= MAX_FILES) return -1;

    int blocks_needed = (size + BLOCK_SIZE - 1) / BLOCK_SIZE;
    int allocated[blocks_needed];

    if (allocate_blocks(fs, blocks_needed, allocated) == -1) return -1;

    File *file = &fs->files[fs->file_count];
    strcpy(file->name, name);
    file->size = size;
    file->block_count = blocks_needed;
    memcpy(file->blocks, allocated, sizeof(allocated));

    hashmap_insert(&fs->file_map, name, fs->file_count);  // Insertar en el hashmap
    fs->file_count++;
    save_file_system(fs, "filesystem.bin");
    return 0;
}

int write_file(FileSystem *fs, const char *name, int offset, const char *data) {
    int index = hashmap_get(&fs->file_map, name);
    if (index == -1) return -1;

    File *file = &fs->files[index];
    if (offset + strlen(data) > file->size) return -1;

    memcpy(&file->data[offset], data, strlen(data));
    save_file_system(fs, "filesystem.bin");
    return 0;
}

int read_file(FileSystem *fs, const char *name, int offset, int size) {
    int index = hashmap_get(&fs->file_map, name);
    if (index == -1) return -1;

    File *file = &fs->files[index];
    if (offset + size > file->size) return -1;

    printf("Salida: \"%.*s\"\n", size, &file->data[offset]);
    return 0;
}

int delete_file(FileSystem *fs, const char *name) {
    int index = hashmap_get(&fs->file_map, name);
    if (index == -1) return -1;

    File *file = &fs->files[index];
    for (int i = 0; i < file->block_count; i++) {
        fs->block_bitmap[file->blocks[i]] = 0;
    }

    hashmap_remove(&fs->file_map, name);  // Eliminar del hashmap
    fs->files[index] = fs->files[--fs->file_count];
    save_file_system(fs, "filesystem.bin");
    return 0;
}

void list_files(FileSystem *fs) {
    printf("Archivos en el sistema:\n");
    printf("----------------------------------------------------------------------------------------------------------\n");
    printf("| %-20s | %-21s | %-23s | %-23s |\n", "Nombre", "Tamaño (bloques / bytes)", "Bytes usados", "Bytes libres");
    printf("----------------------------------------------------------------------------------------------------------\n");

    // Recorremos cada bucket en el hashmap
    for (int i = 0; i < HASHMAP_SIZE; i++) {
        int node_index = fs->file_map.buckets[i];
        
        // Recorremos los nodos en el bucket usando los índices
        while (node_index != -1) {
            HashNode *node = &fs->file_map.nodes[node_index];
            File *file = &fs->files[node->file_index];
            int blocks_needed = (file->size + BLOCK_SIZE - 1) / BLOCK_SIZE;
            int used_bytes = count_non_null_bytes(file);  // Bytes diferentes de '\0'
            int free_bytes = file->size - used_bytes;     // Bytes que son '\0'

            // Muestra el nombre, tamaño en bloques/bytes, bytes usados y bytes libres
            printf("| %-20s | %5d bloques / %5d bytes | %19d bytes | %19d bytes |\n",
                   file->name, blocks_needed, file->size, used_bytes, free_bytes);

            // Mover al siguiente nodo en la lista del bucket
            node_index = node->next_index;
        }
    }

    printf("----------------------------------------------------------------------------------------------------------\n");
}

void handle_command(FileSystem *fs, FILE *input) {
    char command[256];

    printf("Sistema de archivos interactivo.\n");
    printf("Comandos disponibles:\n");
    printf("CREATE nombre tamaño\n");
    printf("WRITE nombre offset \"contenido\"\n");
    printf("READ nombre offset tamaño\n");
    printf("DELETE nombre\n");
    printf("LIST\n");
    printf("Escribe 'exit' para salir.\n");

    bool is_test_mode = (input != stdin);  // Verificar si estamos en modo de prueba

    while (true) {

        // Si estamos en modo de prueba, imprimir el comando en amarillo
        if (!is_test_mode) {
            printf("> ");
        }

        
        if (!fgets(command, sizeof(command), input)) break;
        command[strcspn(command, "\n")] = '\0';  // Remueve el salto de línea

        // Si estamos en modo de prueba, imprimir el comando en amarillo
        if (is_test_mode) {
            printf("\033[1;33mEjecutando comando de prueba:\n%s\033[0m\n", command);
        }

        char *args[5];  // Espacio para hasta 5 partes del comando
        int argc = 0;

        // Separar el primer argumento (comando) y siguientes hasta llegar a "contenido" o el final
        char *token = strtok(command, " ");
        while (token && argc < 5) {
            args[argc++] = token;
            if (argc == 3 && strcmp(args[0], "WRITE") == 0) {
                // Para WRITE, tomar el contenido completo a partir del tercer argumento
                args[argc++] = strtok(NULL, "\"");
                break;
            }
            token = strtok(NULL, " ");
        }

        

        if (argc == 0) continue;  // Comando vacío
        if (strcmp(args[0], "exit") == 0) break;  // Salir del bucle

        // Manejo de los diferentes comandos con validación de argumentos
        if (strcmp(args[0], "CREATE") == 0 && argc == 3) {
            int size = atoi(args[2]);
            if (create_file(fs, args[1], size) == 0)
                printf("Archivo '%s' creado.\n", args[1]);
            else
                printf("Error al crear '%s'.\n", args[1]);
        } else if (strcmp(args[0], "WRITE") == 0 && argc == 4) {
            int offset = atoi(args[2]);
            if (write_file(fs, args[1], offset, args[3]) == 0)
                printf("Datos escritos en '%s'.\n", args[1]);
            else
                printf("Error al escribir en '%s'.\n", args[1]);
        } else if (strcmp(args[0], "READ") == 0 && argc == 4) {
            int offset = atoi(args[2]);
            int size = atoi(args[3]);
            if (read_file(fs, args[1], offset, size) != 0)
                printf("Error al leer '%s'.\n", args[1]);
        } else if (strcmp(args[0], "DELETE") == 0 && argc == 2) {
            if (delete_file(fs, args[1]) == 0)
                printf("Archivo '%s' eliminado.\n", args[1]);
            else
                printf("Error al eliminar '%s'.\n", args[1]);
        } else if (strcmp(args[0], "LIST") == 0 && argc == 1) {
            list_files(fs);
        } else {
            printf("Comando no reconocido o argumentos incorrectos.\n");
        }
    }
}

int main(int argc, char *argv[]) {
    FileSystem fs;
    init_filesystem(&fs);
    load_file_system(&fs, "filesystem.bin");

    // Verificar si el parámetro --test está presente
    if (argc == 3 && strcmp(argv[1], "--test") == 0) {
        FILE *input_file = fopen(argv[2], "r");
        if (!input_file) {
            perror("Error al abrir el archivo de prueba");
            return 1;
        }
        handle_command(&fs, input_file);  // Pasar el archivo a handle_command
        fclose(input_file);
    } else {
        handle_command(&fs, stdin);  // Pasar stdin para modo interactivo
    }

    save_file_system(&fs, "filesystem.bin");
    return 0;
}