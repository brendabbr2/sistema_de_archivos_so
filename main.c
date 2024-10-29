#include "memory_manager.h"

// Inicializa el sistema de archivos
void init_filesystem(FileSystem* fs) {
    fs->file_count = 0;
    memset(fs->block_bitmap, 0, sizeof(fs->block_bitmap));
}

// Encuentra bloques libres y los marca como ocupados
int allocate_blocks(FileSystem* fs, int blocks_needed, int* allocated) {
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
int create_file(FileSystem* fs, const char* name, int size) {
    if (fs->file_count >= MAX_FILES) return -1;

    int blocks_needed = (size + BLOCK_SIZE - 1) / BLOCK_SIZE; // Redondeo hacia arriba
    int allocated[blocks_needed];

    if (allocate_blocks(fs, blocks_needed, allocated) == -1) return -1;

    File* file = &fs->files[fs->file_count++];
    strcpy(file->name, name);
    file->size = size;
    file->block_count = blocks_needed;
    memcpy(file->blocks, allocated, sizeof(allocated));

    return 0;
}

// Escribe datos en un archivo
int write_file(FileSystem* fs, const char* name, int offset, const char* data) {
    for (int i = 0; i < fs->file_count; i++) {
        if (strcmp(fs->files[i].name, name) == 0) {
            if (offset + strlen(data) > fs->files[i].size) return -1; // Fuera de límites
            
            // Escribe los datos en el buffer del archivo
            memcpy(&fs->files[i].data[offset], data, strlen(data));
            return 0;
        }
    }
    return -1; // Archivo no encontrado
}


// Lee datos de un archivo
int read_file(FileSystem* fs, const char* name, int offset, int size) {
    for (int i = 0; i < fs->file_count; i++) {
        if (strcmp(fs->files[i].name, name) == 0) {
            if (offset + size > fs->files[i].size) return -1; // Fuera de límites
            
            // Imprimir los datos leídos
            printf("Salida: \"%.*s\"\n", size, &fs->files[i].data[offset]);
            return 0;
        }
    }
    return -1; // Archivo no encontrado
}


// Elimina un archivo
int delete_file(FileSystem* fs, const char* name) {
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
void list_files(FileSystem* fs) {
    if (fs->file_count == 0) {
        printf("(no hay archivos)\n");
        return;
    }
    for (int i = 0; i < fs->file_count; i++) {
        printf("%s - %d bytes\n", fs->files[i].name, fs->files[i].size);
    }
}
/*
// Programa principal
int main() {
    FileSystem fs;
    init_filesystem(&fs);

    // Prueba de creación de archivos
    if (create_file(&fs, "archivo1.txt", 1000) == 0) {
        printf("Archivo 'archivo1.txt' creado exitosamente.\n");
    } else {
        printf("Error al crear 'archivo1.txt'.\n");
    }

    if (create_file(&fs, "archivo2.txt", 500) == 0) {
        printf("Archivo 'archivo2.txt' creado exitosamente.\n");
    } else {
        printf("Error al crear 'archivo2.txt'.\n");
    }

    // Listar archivos creados
    list_files(&fs);

    // Probar la creación de un archivo que exceda el límite
    for (int i = 0; i < MAX_FILES; i++) {
        char filename[20];
        sprintf(filename, "archivo%d.txt", i + 3); // Crear más archivos
        if (create_file(&fs, filename, 100) == -1) {
            printf("Error al crear '%s', se alcanzó el límite de archivos.\n", filename);
            break; // Salir del bucle si no se puede crear más archivos
        }
    }

    // Listar archivos nuevamente para verificar
    list_files(&fs);

    return 0;
}
*/



// Programa principal
int main() {
    FileSystem fs;
    init_filesystem(&fs);

    create_file(&fs, "archivo1.txt", 1000);
    write_file(&fs, "archivo1.txt", 0, "Hola, mundo");
    read_file(&fs, "archivo1.txt", 0, 11);
    list_files(&fs);
    delete_file(&fs, "archivo1.txt");
    list_files(&fs);

    return 0;
}

