#include "memory_manager.h"
#include "hashmap.h"

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
    return 0;
}

int write_file(FileSystem *fs, const char *name, int offset, const char *data) {
    int index = hashmap_get(&fs->file_map, name);
    if (index == -1) return -1;

    File *file = &fs->files[index];
    if (offset + strlen(data) > file->size) return -1;

    memcpy(&file->data[offset], data, strlen(data));
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
    return 0;
}

void list_files(FileSystem *fs) {
    printf("Archivos en el sistema:\n");
    for (int i = 0; i < fs->file_count; i++) {
        printf("  - %s (Tamaño: %d bytes)\n", fs->files[i].name, fs->files[i].size);
    }
}
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
        command[strcspn(command, "\n")] = '\0';

        char *args[4];
        int argc = 0;
        char *token = strtok(command, " ");
        while (token && argc < 4) {
            args[argc++] = token;
            token = strtok(NULL, " ");
        }

        if (argc == 0) continue;
        if (strcmp(args[0], "exit") == 0) break;

        if (strcmp(args[0], "CREATE") == 0 && argc == 3) {
            if (create_file(fs, args[1], atoi(args[2])) == 0)
                printf("Archivo '%s' creado.\n", args[1]);
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
    load_file_system(&fs, "filesystem.bin");
    handle_command(&fs);
    save_file_system(&fs, "filesystem.bin");
    return 0;
}