# Variables
CC = gcc
CFLAGS = -Wall -Wextra -std=c99
TARGET = filesystem
OBJS = main.o

# Regla principal para compilar el ejecutable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

# Regla para compilar main.c (incluye memory_manager.h)
main.o: main.c memory_manager.h
	$(CC) $(CFLAGS) -c main.c

# Limpieza de archivos generados
clean:
	rm -f $(OBJS) $(TARGET)

# Regla para ejecutar el programa
run: $(TARGET)
	./$(TARGET)

