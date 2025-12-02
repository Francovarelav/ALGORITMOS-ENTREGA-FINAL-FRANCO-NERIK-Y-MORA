/*
Descripción del programa:
7. Agregue todos los menús y almacénelos en una estructura tipo BST  dónde la llave será el número de platillor y el valor es el platillo.
8. Encuentre los pedidos con más solicitud.

Autores: Equipo SProblema 5
 - Franco Varela Villegas - A01199186
 - Nerik Nieto Gutiérrez - A01739652
 - Eduardo Mora Howard - A00829920

Fecha de modificación: 13/11/2025
*/

#include <iostream>
#include <fstream>
#include <cstring>
using namespace std;

#define MAX_ORDENES 10000
#define MAX_NOMBRE 256
#define MAX_PLATILLOS 5000  // máximo de platillos distintos

// Nodo del árbol binario de búsqueda
struct NodoArbol {
    int frecuencia;
    char ordenes[10][MAX_NOMBRE]; // agrupa platillos con la misma frecuencia (máx. 10)
    int numOrdenes;
    NodoArbol *izq, *der;

    NodoArbol(int f, const char* nombre) {
        frecuencia = f;
        strcpy(ordenes[0], nombre);
        numOrdenes = 1;
        izq = der = nullptr;
    }
};

// Inserta en el árbol según frecuencia
NodoArbol* insertar(NodoArbol* raiz, int frecuencia, const char* nombre) {
    if (raiz == nullptr)
        return new NodoArbol(frecuencia, nombre);

    if (frecuencia < raiz->frecuencia)
        raiz->izq = insertar(raiz->izq, frecuencia, nombre);
    else if (frecuencia > raiz->frecuencia)
        raiz->der = insertar(raiz->der, frecuencia, nombre);
    else {
        // Misma frecuencia → agregar al mismo nodo
        if (raiz->numOrdenes < 10) {
            strcpy(raiz->ordenes[raiz->numOrdenes], nombre);
            raiz->numOrdenes++;
        }
    }
    return raiz;
}

// Recorrido descendente (de mayor a menor frecuencia)
void imprimirDescendente(NodoArbol* raiz) {
    if (raiz == nullptr) return;

    imprimirDescendente(raiz->der);

    cout << raiz->frecuencia << " veces: ";
    for (int i = 0; i < raiz->numOrdenes; i++) {
        cout << raiz->ordenes[i];
        if (i < raiz->numOrdenes - 1) cout << ", ";
    }
    cout << endl;

    imprimirDescendente(raiz->izq);
}

// Extrae el nombre del platillo del texto completo
void extraerOrden(const char* linea, char* resultado) {
    const char* inicio = strstr(linea, "O:");
    if (inicio == nullptr) {
        strcpy(resultado, ""); // no hay orden
        return;
    }
    inicio += 2; // saltar "O:"
    const char* fin = strchr(inicio, '(');
    if (fin == nullptr) {
        strcpy(resultado, inicio);
    } else {
        int len = fin - inicio;
        strncpy(resultado, inicio, len);
        resultado[len] = '\0';
    }

    // Eliminar espacios al inicio y fin
    while (resultado[0] == ' ') memmove(resultado, resultado + 1, strlen(resultado));
    int n = strlen(resultado);
    while (n > 0 && resultado[n - 1] == ' ') resultado[--n] = '\0';
}

int main() {
    ifstream archivo("orders.txt");
    if (!archivo.is_open()) {
        cout << "Error: no se pudo abrir 'orders.txt'" << endl;
        return 1;
    }

    cout << "Leyendo y procesando órdenes..." << endl;

    char linea[MAX_NOMBRE];
    char ordenes[MAX_ORDENES][MAX_NOMBRE];
    int totalOrdenes = 0;

    // Leer todas las órdenes del archivo
    while (archivo.getline(linea, sizeof(linea)) && totalOrdenes < MAX_ORDENES) {
        char platillo[MAX_NOMBRE];
        extraerOrden(linea, platillo);
        if (strlen(platillo) > 0) {
            strcpy(ordenes[totalOrdenes], platillo);
            totalOrdenes++;
        }
    }
    archivo.close();

    cout << "Total de órdenes leídas: " << totalOrdenes << endl;

    // Contar repeticiones
    char nombres[MAX_PLATILLOS][MAX_NOMBRE];
    int conteos[MAX_PLATILLOS];
    int numPlatos = 0;

    for (int i = 0; i < totalOrdenes; i++) {
        bool encontrado = false;
        for (int j = 0; j < numPlatos; j++) {
            if (strcmp(nombres[j], ordenes[i]) == 0) {
                conteos[j]++;
                encontrado = true;
                break;
            }
        }
        if (!encontrado && numPlatos < MAX_PLATILLOS) {
            strcpy(nombres[numPlatos], ordenes[i]);
            conteos[numPlatos] = 1;
            numPlatos++;
        }
    }

    cout << "Platillos únicos encontrados: " << numPlatos << endl;

    // Crear el árbol binario por frecuencia
    NodoArbol* raiz = nullptr;
    for (int i = 0; i < numPlatos; i++) {
        raiz = insertar(raiz, conteos[i], nombres[i]);
    }

    // Mostrar el árbol descendente
    cout << "\n=== ÁRBOL DE FRECUENCIAS (de más pedidas a menos) ===\n";
    imprimirDescendente(raiz);

    return 0;
}
