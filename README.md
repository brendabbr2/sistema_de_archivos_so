# Sistema de Archivos Simple

Integrantes:
- Brenda Badilla Rodríguez, 2020065241
- Joctan Porras Esquivel, 2021069671
  
[Enlace del github](https://github.com/brendabbr2/sistema_de_archivos_so/tree/main)

## Compilación y Ejecución ⚙️
   ```bash
make clean
make 
make run
make test
   ```
## 1. Estructuras de datos utilizadas

File: representa un archivo en el sistema, con atributos para su nombre, tamaño, número de bloques asignados y el contenido en sí (almacenado en data). Este diseño permite guardar la información esencial de cada archivo, como el nombre, el tamaño en bloques y la asignación de estos bloques.

FileSystem: esta estructura organiza los archivos y administra los bloques de memoria del sistema. Contiene:
        files: un array de archivos.
        file_count: cuenta de archivos en el sistema.
        block_bitmap: un arreglo que indica la disponibilidad de cada bloque.
        file_map: un hashmap para localizar rápidamente archivos por su nombre.

HashMap y HashNode: para lograr un acceso eficiente, se emplea un hashmap que permite insertar, buscar y eliminar archivos basados en sus nombres. Cada HashNode contiene un nombre de archivo (key), el índice del archivo en el array de files, y un next_index para la resolución de colisiones mediante encadenamiento en la misma posición del bucket. HashMap mantiene un array de buckets para referencias iniciales a cada lista de nodos en un bucket y un contador node_count para controlar el uso.

## 2. Decisiones de diseño

  Mapeo de Archivos: el uso de un hashmap (implementado en file_map) permite un acceso O(1) promedio al buscar archivos, optimizando las operaciones sobre el sistema de archivos.

  Asignación de Bloques: la función allocate_blocks maneja la asignación de bloques en block_bitmap, buscando bloques libres y asignándolos al archivo cuando se crea. La función verifica si existen suficientes bloques disponibles antes de asignarlos, reduciendo el riesgo de errores de asignación.

Persistencia: las funciones save_file_system y load_file_system permiten guardar y cargar el estado del sistema de archivos en un archivo binario, conservando información de archivos y del hashmap. Así, el sistema persiste entre ejecuciones, manteniendo los datos y las ubicaciones de los archivos en el disco.

Interfaz de Usuario: el sistema cuenta con una interfaz de comandos (handle_command) que ofrece operaciones de creación, lectura, escritura y eliminación de archivos. Las instrucciones se procesan en tiempo real, lo que facilita la manipulación del sistema.
