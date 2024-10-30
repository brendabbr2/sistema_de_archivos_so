#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "memory_manager.h"

// Inicializa el sistema de archivos
void init_filesystem(FileSystem *fs) {
    fs->file_count = 0;
    memset(fs->block_bitmap, 0, sizeof(fs->block_bitmap));
}

// Encuentra bloques libres y los marca como ocupados
int allocate_blocks(FileSystem *fs, int blocks_needed, int *allocated) {
    int count = 0;
    for (int i = 0; i < MAX_BLOCKS && count < blocks_needed; i++) {
        if (fs->block_bitmap[i] == 0) {
            allocated[count++] = i;
            fs->block_bitmap[i] = 1;
        }
    }
    return (count == blocks_needed) ? 0 : -1; // Error si no hay suficientes bloques
}

// Crea un archivo
int create_file(FileSystem *fs, const char *name, int size) {
    if (fs->file_count >= MAX_FILES) return -1;

    int blocks_needed = (size + BLOCK_SIZE - 1) / BLOCK_SIZE;
    int allocated[blocks_needed];

    if (allocate_blocks(fs, blocks_needed, allocated) == -1) return -1;

    File *file = &fs->files[fs->file_count++];
    strcpy(file->name, name);
    file->size = size;
    file->block_count = blocks_needed;
    memcpy(file->blocks, allocated, sizeof(allocated));

    return 0;
}

// Escribe datos en un archivo
int write_file(FileSystem *fs, const char *name, int offset, const char *data) {
    for (int i = 0; i < fs->file_count; i++) {
        if (strcmp(fs->files[i].name, name) == 0) {
            if (offset + strlen(data) > fs->files[i].size) return -1; // Fuera de límites

            memcpy(&fs->files[i].data[offset], data, strlen(data));
            return 0;
        }
    }
    return -1; // Archivo no encontrado
}

// Lee datos de un archivo
int read_file(FileSystem *fs, const char *name, int offset, int size) {
    for (int i = 0; i < fs->file_count; i++) {
        if (strcmp(fs->files[i].name, name) == 0) {
            if (offset + size > fs->files[i].size) return -1; // Fuera de límites

            printf("Salida: \"%.*s\"\n", size, &fs->files[i].data[offset]);
            return 0;
        }
    }
    return -1; // Archivo no encontrado
}

// Elimina un archivo
int delete_file(FileSystem *fs, const char *name) {
    for (int i = 0; i < fs->file_count; i++) {
        if (strcmp(fs->files[i].name, name) == 0) {
            for (int j = 0; j < fs->files[i].block_count; j++) {
                fs->block_bitmap[fs->files[i].blocks[j]] = 0; // Liberar bloques
            }
            fs->files[i] = fs->files[--fs->file_count]; // Eliminar archivo
            return 0;
        }
    }
    return -1; // Archivo no encontrado
}

// Lista los archivos
void list_files(FileSystem *fs) {
    if (fs->file_count == 0) {
        printf("(no hay archivos)\n");
        return;
    }
    for (int i = 0; i < fs->file_count; i++) {
        printf("%s - %d bytes\n", fs->files[i].name, fs->files[i].size);
    }
}

// Guarda el estado del sistema de archivos en un archivo binario
void save_file_system(FileSystem *fs, const char *filename) {
    FILE *file = fopen(filename, "wb");
    if (!file) {
        perror("Error al abrir el archivo");
        return;
    }
    if (fwrite(fs, sizeof(FileSystem), 1, file) != 1) {
        perror("Error al escribir el sistema de archivos");
    }
    fclose(file);
}

// Carga el estado del sistema de archivos desde un archivo binario
void load_file_system(FileSystem *fs, const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (!file) { // Si el archivo no existe, inicializa un nuevo sistema
        printf("Archivo no encontrado. Creando un nuevo sistema de archivos...\n");
        init_filesystem(fs);
        save_file_system(fs, filename); // Guarda el nuevo sistema
        return;
    }
    if (fread(fs, sizeof(FileSystem), 1, file) != 1) {
        perror("Error al leer el sistema de archivos");
        init_filesystem(fs); // Si hay error, reinicia el sistema de archivos
    }
    fclose(file);
}
/*
// Programa principal
int main() {
    FileSystem fs;
    const char *filesystem_file = "filesystem.dat";

    // Cargar el sistema de archivos desde el archivo, o crear uno nuevo si no existe
    load_file_system(&fs, filesystem_file);

    // Prueba de creación, escritura, lectura y eliminación
    create_file(&fs, "archivo1.txt", 1000);
    write_file(&fs, "archivo1.txt", 0, "Hola, mundo");
    read_file(&fs, "archivo1.txt", 0, 11);
    list_files(&fs);

    delete_file(&fs, "archivo1.txt");
    list_files(&fs);

    // Guardar el estado actualizado del sistema de archivos
    save_file_system(&fs, filesystem_file);

    return 0;
}
*/

void handle_command(FileSystem *fs) {
    char command[256];

    printf("Sistema de archivos interactivo.\n");
    printf("Comandos disponibles:\n");
    printf("CREATE nombre tamaño\n");
    printf("WRITE nombre offset \"contenido\"\n");
    printf("READ nombre offset tamaño\n");
    printf("DELETE nombre\n");
    printf("LIST\n");
    printf("Escribe 'exit' para salir.\n");

    while (1) {
        printf("> ");
        if (!fgets(command, sizeof(command), stdin)) break;

        // Eliminar el salto de línea al final del comando
        command[strcspn(command, "\n")] = '\0';

        // Verificar si el usuario quiere salir
        if (strcmp(command, "exit") == 0) break;

        char *args[10];
        int argc = 0;

        // Separar la línea en palabras
        char *token = strtok(command, " ");
        while (token != NULL && argc < 10) {
            args[argc++] = token;
            token = strtok(NULL, " ");
        }

        if (argc == 0) continue;

        if (strcmp(args[0], "CREATE") == 0 && argc == 3) {
            if (create_file(fs, args[1], atoi(args[2])) == 0)
                printf("Archivo '%s' creado exitosamente.\n", args[1]);
            else
                printf("Error al crear '%s'.\n", args[1]);
        } else if (strcmp(args[0], "WRITE") == 0 && argc == 4) {
            if (write_file(fs, args[1], atoi(args[2]), args[3]) == 0)
                printf("Datos escritos en '%s'.\n", args[1]);
            else
                printf("Error al escribir en '%s'.\n", args[1]);
        } else if (strcmp(args[0], "READ") == 0 && argc == 4) {
            if (read_file(fs, args[1], atoi(args[2]), atoi(args[3])) != 0)
                printf("Error al leer '%s'.\n", args[1]);
        } else if (strcmp(args[0], "DELETE") == 0 && argc == 2) {
            if (delete_file(fs, args[1]) == 0)
                printf("Archivo '%s' eliminado.\n", args[1]);
            else
                printf("Error al eliminar '%s'.\n", args[1]);
        } else if (strcmp(args[0], "LIST") == 0) {
            list_files(fs);
        } else {
            printf("Comando no reconocido.\n");
        }
    }
}

int main() {
    FileSystem fs;
    init_filesystem(&fs);
    handle_command(&fs);
    return 0;
}
