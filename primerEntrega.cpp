/*
Descripción del programa:
1.⁠ ⁠Abra el archivo de entrada llamado "orders.txt", lo lea y almacene los datos en arreglos. Es valido crear objetos.
2.⁠ ⁠Ordena la información por fecha para la realización de las búsquedas. 
3.⁠ ⁠Despliega los primeros 10 registros en pantalla y almacena en un archivo salida.txt el resultado del ordenamiento completo. 
4.⁠ ⁠Solicita al usuario las fechas de inicio y fin de búsqueda de información. 
5.⁠ ⁠Despliega los registros correspondientes a esas fechas. Toma en cuenta que si no hay coincidencias exactas para inicio y fin de todas formas debes devolver registros dentro del rango. 
6.⁠ ⁠Despliegue en pantalla el resultado de las búsquedas, y da al usuario la opción de guardarlo en un archivo. 

Autores: Equipo SProblema 5
 - Franco Varela Villegas - A01199186
 - Nerik Nieto Gutiérrez - A01739652
 - Eduardo Mora Howard - A00829920
*/

#include <iostream>
using namespace std;
#include <fstream>
#include <cstring>
#include <algorithm>

// Estructura para almacenar los datos completos de cada orden
struct Orden {
    unsigned long int fecha;
    char restaurante[256];
    char orden[256];
    unsigned long int precio;
    char lineaOriginal[256];
    
    // Constructor default
    Orden() {
        fecha = 0;
        strcpy(restaurante, "");
        strcpy(orden, "");
        precio = 0;
        strcpy(lineaOriginal, "");
    }
    
    // Constructor con parámetros
    Orden(unsigned long int _fecha, const char* _restaurante, const char* _orden, unsigned long int _precio, const char* _linea) {
        fecha = _fecha;
        strcpy(restaurante, _restaurante);
        strcpy(orden, _orden);
        precio = _precio;
        strcpy(lineaOriginal, _linea);
    }
};

// Función para extraer fecha de una línea
int extraerFecha(char *linea) {
    // Crear una copia de la linea original para no modificarla
    char copiaLinea[256];
    strcpy(copiaLinea, linea);

    const char* meses[12] = {"ene", "Feb", "Mar", "Abr", "May", "Jun", "Jul", "Ago", "Sep", "Oct", "Nov", "Dic"};
    char mesCaracter[4], diaCaracter[3], marcaTiempoCaracter[7];

    // Extraer con separadores y copiar en los arreglos de mes, dia y marcaTiempo (horas, minutos y segundos)
    char *token = strtok(copiaLinea, " "); // Crear un token igual a la copia de la línea original pero cortado por strtok
    strcpy(mesCaracter, token);
    token = strtok(NULL, " ");
    strcpy(diaCaracter, token);
    token = strtok(NULL, ":");
    strcpy(marcaTiempoCaracter, token); // Extrae Horas
    token = strtok(NULL, ":");
    strcat(marcaTiempoCaracter, token); // Extrae y concatena Minutos
    token = strtok(NULL, " ");
    strcat(marcaTiempoCaracter, token); // Extrae y concatena Segundos

    // Convertir mes de cadena de caracteres a int
    int mesNum = 0;
    for (int i = 0; i < 12; i++) {
        if (strncmp(mesCaracter, meses[i], 3) == 0) {
            mesNum = i + 1;
        }
    }
    char mesNumCaracter[3];
    mesNumCaracter[0] = (mesNum / 10) + '0';
    mesNumCaracter[1] = (mesNum % 10) + '0';
    mesNumCaracter[2] = '\0';

    // Unir cadenas en una sola fecha
    char fechaCaracter[10];
    strcpy(fechaCaracter, mesNumCaracter);
    strcat(fechaCaracter, diaCaracter);
    strcat(fechaCaracter, marcaTiempoCaracter);

    unsigned long int fecha = atoi(fechaCaracter);

    return fecha;
}

// Función para extraer restaurante de una línea
void extraerRestaurante(char *linea, char *restaurante) {
    char copiaLinea[256];
    strcpy(copiaLinea, linea);
    
    char *token = strtok(copiaLinea, " ");
    token = strtok(NULL, " ");
    token = strtok(NULL, " ");
    token = strtok(NULL, " ");
    token = strtok(NULL, " ");
    token = strtok(NULL, " ");
    
    if (token != NULL) {
        strcpy(restaurante, token);
    }
}

// Función para extraer orden de una línea
void extraerOrden(char *linea, char *orden) {
    char copiaLinea[256];
    strcpy(copiaLinea, linea);
    
    char *token = strtok(copiaLinea, " ");
    token = strtok(NULL, " ");
    token = strtok(NULL, " ");
    token = strtok(NULL, " ");
    token = strtok(NULL, " ");
    token = strtok(NULL, " ");
    
    if (token != NULL) {
        strcpy(orden, token);
    }
}

// Función para extraer precio de una línea
unsigned long int extraerPrecio(char *linea) {
    char copiaLinea[256];
    strcpy(copiaLinea, linea);
    
    // Buscar el último paréntesis
    char *inicio = strrchr(copiaLinea, '(');
    char *fin = strrchr(copiaLinea, ')');
    
    if (inicio != NULL && fin != NULL) {
        char precioStr[10];
        int len = fin - inicio - 1;
        strncpy(precioStr, inicio + 1, len);
        precioStr[len] = '\0';
        return atoi(precioStr);
    }
    return 0;
}

// Función QuickSort para ordenar por fecha
void quickSort(Orden arr[], int low, int high) {
    if (low < high) {
        // Particionar el arreglo
        int pivot = arr[high].fecha;
        int i = (low - 1);
        
        for (int j = low; j <= high - 1; j++) {
            if (arr[j].fecha <= pivot) {
                i++;
                swap(arr[i], arr[j]);
            }
        }
        swap(arr[i + 1], arr[high]);
        int pi = i + 1;
        
        // Ordenar recursivamente
        quickSort(arr, low, pi - 1);
        quickSort(arr, pi + 1, high);
    }
}

// Función para mostrar los primeros 10 registros
void mostrarPrimeros10(Orden ordenes[], int total) {
    cout << "\n=== PRIMEROS 10 REGISTROS ORDENADOS POR FECHA ===" << endl;
    int limite = min(10, total);
    for (int i = 0; i < limite; i++) {
        cout << i + 1 << ". " << ordenes[i].lineaOriginal << endl;
    }
}

// Función para guardar todos los registros ordenados en salida.txt
void guardarOrdenamientoCompleto(Orden ordenes[], int total) {
    ofstream archivo_salida("salida.txt");
    if (archivo_salida.is_open()) {
        for (int i = 0; i < total; i++) {
            archivo_salida << ordenes[i].lineaOriginal << endl;
        }
        archivo_salida.close();
        cout << "\nArchivo 'salida.txt' creado exitosamente con " << total << " registros ordenados." << endl;
    } else {
        cout << "Error al crear el archivo salida.txt" << endl;
    }
}

// Función para buscar registros en un rango de fechas
void buscarPorRango(Orden ordenes[], int total, unsigned long int fechaInicio, unsigned long int fechaFin) {
    cout << "\n=== RESULTADOS DE BÚSQUEDA ===" << endl;
    cout << "Buscando registros entre fechas: " << fechaInicio << " y " << fechaFin << endl;
    
    int encontrados = 0;
    for (int i = 0; i < total; i++) {
        if (ordenes[i].fecha >= fechaInicio && ordenes[i].fecha <= fechaFin) {
            cout << encontrados + 1 << ". " << ordenes[i].lineaOriginal << endl;
            encontrados++;
        }
    }
    
    if (encontrados == 0) {
        cout << "No se encontraron registros en el rango especificado." << endl;
    } else {
        cout << "\nTotal de registros encontrados: " << encontrados << endl;
    }
}

// Función para guardar resultados de búsqueda
void guardarBusqueda(Orden ordenes[], int total, unsigned long int fechaInicio, unsigned long int fechaFin) {
    ofstream archivo_busqueda("busqueda.txt");
    if (archivo_busqueda.is_open()) {
        archivo_busqueda << "=== RESULTADOS DE BÚSQUEDA ===" << endl;
        archivo_busqueda << "Rango de fechas: " << fechaInicio << " a " << fechaFin << endl << endl;
        
        int encontrados = 0;
        for (int i = 0; i < total; i++) {
            if (ordenes[i].fecha >= fechaInicio && ordenes[i].fecha <= fechaFin) {
                archivo_busqueda << encontrados + 1 << ". " << ordenes[i].lineaOriginal << endl;
                encontrados++;
            }
        }
        
        archivo_busqueda << "\nTotal de registros encontrados: " << encontrados << endl;
        archivo_busqueda.close();
        cout << "Resultados de búsqueda guardados en 'busqueda.txt'" << endl;
    } else {
        cout << "Error al crear el archivo de búsqueda" << endl;
    }
}

int main() {
    Orden ordenes[10000];
    int totalOrdenes = 0;
    
    // 1. Leer archivo y almacenar datos
    ifstream archivo_entrada("orders.txt");
    
    if (archivo_entrada.is_open()) {
        cout << "- - - - - ARCHIVO ABIERTO - - - - -" << endl;
        char linea[256];
        
        while (archivo_entrada.getline(linea, sizeof(linea)) && totalOrdenes < 10000) {
            // Extraer datos de la línea
            unsigned long int fecha = extraerFecha(linea);
            char restaurante[256], orden[256];
            extraerRestaurante(linea, restaurante);
            extraerOrden(linea, orden);
            unsigned long int precio = extraerPrecio(linea);
            
            // Crear objeto Orden y almacenarlo
            ordenes[totalOrdenes] = Orden(fecha, restaurante, orden, precio, linea);
            totalOrdenes++;
        }
        
        archivo_entrada.close();
        cout << "Líneas leídas: " << totalOrdenes << endl;
        cout << "- - - - - ARCHIVO CERRADO - - - - -" << endl;
    } else {
        cout << "Error al abrir el archivo orders.txt" << endl;
        return 1;
    }
    
    // 2. Ordenar por fecha
    cout << "\nOrdenando registros por fecha..." << endl;
    quickSort(ordenes, 0, totalOrdenes - 1);
    cout << "Ordenamiento completado." << endl;
    
    // 3. Mostrar primeros 10 registros
    mostrarPrimeros10(ordenes, totalOrdenes);
    
    // 4. Guardar ordenamiento completo en salida.txt
    guardarOrdenamientoCompleto(ordenes, totalOrdenes);
    
    // 5. Solicitar fechas de búsqueda al usuario
    cout << "\n=== BÚSQUEDA POR RANGO DE FECHAS ===" << endl;
    cout << "Ingrese la fecha de inicio (formato: MMDDHHMMSS): ";
    unsigned long int fechaInicio;
    cin >> fechaInicio;
    
    cout << "Ingrese la fecha de fin (formato: MMDDHHMMSS): ";
    unsigned long int fechaFin;
    cin >> fechaFin;
    
    // 6. Buscar y mostrar registros en el rango
    buscarPorRango(ordenes, totalOrdenes, fechaInicio, fechaFin);
    
    // 7. Preguntar si quiere guardar los resultados
    char opcion;
    cout << "\n¿Desea guardar los resultados de búsqueda en un archivo? (s/n): ";
    cin >> opcion;
    
    if (opcion == 's' || opcion == 'S') {
        guardarBusqueda(ordenes, totalOrdenes, fechaInicio, fechaFin);
    }
    
    cout << "\nPrograma finalizado." << endl;
    return 0;
}
