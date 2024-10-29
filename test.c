#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "memory_manager.h"

// Función para manejar la entrada de comandos y ejecutar las operaciones correspondientes
void process_command(FileSystem *fs) {
    char command[256];
    char filename[100];
    int offset, size;
    char data[512]; // Asume que la data no excederá 512 caracteres.

    while (1) {
        printf("> ");
        fgets(command, sizeof(command), stdin); // Leer entrada del usuario

        // Quitar el salto de línea al final del comando
        command[strcspn(command, "\n")] = 0;

        // Verificar si es el comando CREATE
        if (sscanf(command, "CREATE %s %d", filename, &size) == 2) {
            if (create_file(fs, filename, size) == 0) {
                printf("Archivo '%s' creado con éxito.\n", filename);
            } else {
                printf("Error: No se pudo crear el archivo.\n");
            }
        }
        // Verificar si es el comando WRITE
        else if (sscanf(command, "WRITE %s %d \"%[^\"]\"", filename, &offset, data) == 3) {
            if (write_file(fs, filename, offset, data) == 0) {
                printf("Escritura exitosa en '%s'.\n", filename);
            } else {
                printf("Error: No se pudo escribir en el archivo.\n");
            }
        }
        // Verificar si es el comando READ
        else if (sscanf(command, "READ %s %d %d", filename, &offset, &size) == 3) {
            char *buffer = (char *)malloc(size + 1); // Espacio para leer los datos
            if (read_file(fs, filename, offset, size, buffer) == 0) {
                buffer[size] = '\0'; // Asegurar que esté terminado en nulo
                printf("Salida: \"%s\"\n", buffer);
            } else {
                printf("Error: No se pudo leer del archivo.\n");
            }
            free(buffer);
        }
        // Verificar si es el comando DELETE
        else if (sscanf(command, "DELETE %s", filename) == 1) {
            if (delete_file(fs, filename) == 0) {
                printf("Archivo '%s' eliminado con éxito.\n", filename);
            } else {
                printf("Error: No se pudo eliminar el archivo.\n");
            }
        }
        // Verificar si es el comando LIST
        else if (strcmp(command, "LIST") == 0) {
            list_files(fs);
        }
        // Verificar si es el comando EXIT
        else if (strcmp(command, "EXIT") == 0) {
            break; // Terminar el bucle
        }
        // Comando desconocido
        else {
            printf("Comando no reconocido. Intente de nuevo.\n");
        }
    }
}

// Función principal
int main() {
    FileSystem fs;
    init_filesystem(&fs); // Inicializa el sistema de archivos

    printf("Sistema de archivos iniciado. Ingrese comandos:\n");
    process_command(&fs); // Procesa los comandos ingresados por el usuario

    return 0;
}
