/*
Grafo Dirigido con Listas de Adyacencia (Implementación Dinámica)
Este programa implementa un grafo donde:
- Se usan Listas Enlazadas manualmente (sin std::vector) para las conexiones.
- Los nodos (Platillos y Restaurantes) tienen IDs únicos.
- La relación es DIRIGIDA: Platillo -> Restaurante (un platillo "apunta" a los restaurantes donde se vende).
- Se utiliza BFS para recorrer las conexiones del platillo.

Estructura:
- Grafo representado como un arreglo de punteros a Listas de Adyacencia.
- Búsqueda de índices por nombre.

Autores: Equipo SProblema 5
 - Franco Varela Villegas - A01199186
 - Nerik Nieto Gutiérrez - A01739652
 - Eduardo Mora Howard - A00829920

Fecha: 02/12/2025
*/

#include <iostream>
#include <fstream>
#include <cstring>
#include <queue>

using namespace std;

#define MAX_NOMBRE 256
#define MAX_NODOS 10000

//ESTRUCTURAS DE DATOS (LISTAS ENLAZADAS)

// Nodo de la Lista Enlazada (Representa una arista/conexión)
struct NodoAdyacencia {
    int idDestino;      // ID del nodo al que conecta (índice en el arreglo de vértices)
    int peso;           // Frecuencia de pedidos
    NodoAdyacencia* siguiente; // Puntero al siguiente nodo en la lista

    NodoAdyacencia(int id) {
        idDestino = id;
        peso = 1;
        siguiente = nullptr;
    }
};

// Vértice del Grafo (Puede ser Platillo o Restaurante)
struct Vertice {
    char nombre[MAX_NOMBRE];
    char tipo; // 'P' = Platillo, 'R' = Restaurante
    NodoAdyacencia* cabezaLista; // Inicio de la lista enlazada de vecinos

    Vertice() {
        strcpy(nombre, "");
        tipo = ' ';
        cabezaLista = nullptr;
    }
};

// --- VARIABLES GLOBALES DEL GRAFO ---
Vertice grafo[MAX_NODOS];
int numNodos = 0;

// --- FUNCIONES DE MANEJO DE LISTAS ---

// Busca si existe una conexión en la lista enlazada
NodoAdyacencia* buscarArista(NodoAdyacencia* cabeza, int idDestino) {
    NodoAdyacencia* actual = cabeza;
    while (actual != nullptr) {
        if (actual->idDestino == idDestino) {
            return actual;
        }
        actual = actual->siguiente;
    }
    return nullptr;
}

// Agrega un nodo al inicio de la lista enlazada
void agregarArista(int idOrigen, int idDestino) {
    NodoAdyacencia* existente = buscarArista(grafo[idOrigen].cabezaLista, idDestino);
    
    if (existente != nullptr) {
        existente->peso++;
    } else {
        NodoAdyacencia* nuevo = new NodoAdyacencia(idDestino);
        nuevo->siguiente = grafo[idOrigen].cabezaLista;
        grafo[idOrigen].cabezaLista = nuevo;
    }
}

// --- FUNCIONES DE PARSEO (Mantenemos la lógica robusta anterior) ---

void extraerNombrePlatillo(const char* linea, char* destino) {
    const char* inicio = strstr(linea, "O:");
    if (inicio) {
        inicio += 2;
        while(*inicio == ' ') inicio++;
        const char* fin = strrchr(inicio, '(');
        if (fin) {
            int len = fin - inicio;
            if (len > MAX_NOMBRE - 1) len = MAX_NOMBRE - 1;
            strncpy(destino, inicio, len);
            destino[len] = '\0';
            int l = strlen(destino);
            while (l > 0 && destino[l-1] == ' ') destino[--l] = '\0';
            return;
        }
    }
    strcpy(destino, "Desconocido");
}

void extraerNombreRestaurante(const char* linea, char* destino) {
    const char* inicio = strstr(linea, "R:");
    if (inicio) {
        inicio += 2; 
        while(*inicio == ' ') inicio++;
        const char* fin = strstr(inicio, " O:");
        if (fin) {
            int len = fin - inicio;
            if (len > MAX_NOMBRE - 1) len = MAX_NOMBRE - 1;
            strncpy(destino, inicio, len);
            destino[len] = '\0';
            int l = strlen(destino);
            while (l > 0 && destino[l-1] == ' ') destino[--l] = '\0';
            return;
        }
    }
    strcpy(destino, "Desconocido");
}

// --- GESTIÓN DEL GRAFO ---

// Busca un nodo por nombre y retorna su índice (ID)
int obtenerOcrearNodo(const char* nombre, char tipo) {
    for (int i = 0; i < numNodos; i++) {
        if (strcmp(grafo[i].nombre, nombre) == 0) {
            return i;
        }
    }

    if (numNodos >= MAX_NODOS) return -1;
    
    int nuevoId = numNodos;
    strcpy(grafo[nuevoId].nombre, nombre);
    grafo[nuevoId].tipo = tipo;
    grafo[nuevoId].cabezaLista = nullptr;
    numNodos++;
    
    return nuevoId;
}

// Procesa una línea del archivo
void procesarLinea(const char* linea) {
    char nombrePlatillo[MAX_NOMBRE];
    char nombreRestaurante[MAX_NOMBRE];

    extraerNombrePlatillo(linea, nombrePlatillo);
    extraerNombreRestaurante(linea, nombreRestaurante);

    if (strlen(nombrePlatillo) > 0 && strlen(nombreRestaurante) > 0) {
        int idPlatillo = obtenerOcrearNodo(nombrePlatillo, 'P');
        int idRestaurante = obtenerOcrearNodo(nombreRestaurante, 'R');

        if (idPlatillo != -1 && idRestaurante != -1) {
            // Grafo DIRIGIDO: Platillo -> Restaurante
            agregarArista(idPlatillo, idRestaurante);
        }
    }
}

// --- ALGORITMO BFS ---

void ejecutarBFS(int nodoInicio) {
    if (nodoInicio < 0 || nodoInicio >= numNodos) return;

    cout << "\n=== RECORRIDO BFS (Niveles) ===" << endl;
    cout << "Nodo Raíz (ID " << nodoInicio << "): " << grafo[nodoInicio].nombre << " [" << grafo[nodoInicio].tipo << "]" << endl;

    bool visitado[MAX_NODOS] = {false};
    queue<int> cola;

    visitado[nodoInicio] = true;
    cola.push(nodoInicio);

    int totalPedidos = 0;
    
    if (!cola.empty()) {
        int actual = cola.front();
        cola.pop();
        
        cout << "Conexiones directas (Vecinos):" << endl;
        
        NodoAdyacencia* temp = grafo[actual].cabezaLista;
        while (temp != nullptr) {
            int vecinoId = temp->idDestino;
            if (!visitado[vecinoId]) {
                cout << "   -> " << grafo[vecinoId].nombre 
                     << " (ID: " << vecinoId << ", Tipo: " << grafo[vecinoId].tipo << ")"
                     << " [Pedidos: " << temp->peso << "]" << endl;
                
                totalPedidos += temp->peso;
                visitado[vecinoId] = true;
                cola.push(vecinoId);
            }
            temp = temp->siguiente;
        }
    }
    
    cout << "--------------------------------" << endl;
    cout << "Total de frecuencia en conexiones directas: " << totalPedidos << endl;
    cout << "================================" << endl;
}

// --- INTERFAZ ---

void buscarPlatillo() {
    char busqueda[MAX_NOMBRE];
    cout << "\nIngrese el nombre del platillo a buscar: ";
    cin.getline(busqueda, MAX_NOMBRE);

    int idEncontrado = -1;
    for (int i = 0; i < numNodos; i++) {
        // Solo buscamos entre Platillos para evitar ambigüedades si un restaurante se llamara igual
        if (grafo[i].tipo == 'P' && strcmp(grafo[i].nombre, busqueda) == 0) {
            idEncontrado = i;
            break;
        }
    }

    if (idEncontrado != -1) {
        ejecutarBFS(idEncontrado);
    } else {
        cout << "Platillo no encontrado en la base de datos." << endl;
    }
}

void listarPlatillos() {
    cout << "\n--- LISTA DE PLATILLOS REGISTRADOS ---" << endl;
    int contador = 0;
    for (int i = 0; i < numNodos; i++) {
        if (grafo[i].tipo == 'P') {
            cout << "- " << grafo[i].nombre << endl;
            contador++;
        }
    }
    cout << "Total: " << contador << " platillos." << endl;
}

int main() {
    ifstream archivo("orders.txt");
    if (!archivo.is_open()) {
        cout << "Error: No se pudo abrir orders.txt" << endl;
        return 1;
    }

    cout << "Construyendo grafo con Listas de Adyacencia..." << endl;
    char buffer[MAX_NOMBRE];
    while (archivo.getline(buffer, MAX_NOMBRE)) {
        procesarLinea(buffer);
    }
    archivo.close();
    cout << "Grafo construido. Nodos totales: " << numNodos << endl;

    int opcion = 0;
    while (true) {
        cout << "\n=== SISTEMA DE GRAFOS (IMPLEMENTACIÓN LISTAS ENLAZADAS) ===" << endl;
        cout << "1. Ver lista de platillos" << endl;
        cout << "2. Buscar platillo (BFS)" << endl;
        cout << "3. Salir" << endl;
        cout << "Seleccione: ";
        
        if (!(cin >> opcion)) {
            cout << "Entrada inválida." << endl;
            cin.clear();
            cin.ignore(10000, '\n'); // hardcoded limit ok for cin ignore
            continue;
        }
        cin.ignore(); 

        if (opcion == 3) break;

        switch(opcion) {
            case 1: listarPlatillos(); break;
            case 2: buscarPlatillo(); break;
            default: cout << "Opción no válida." << endl;
        }
    }

    // Limpieza de memoria (Buena práctica en C++ manual)
    for(int i=0; i<numNodos; i++) {
        NodoAdyacencia* actual = grafo[i].cabezaLista;
        while(actual != nullptr) {
            NodoAdyacencia* temp = actual;
            actual = actual->siguiente;
            delete temp;
        }
    }

    return 0;
}

/* comando terminal para compilar y ejecutar el programa
g++ entregafinal_listas.cpp -o entregafinal_listas
./entregafinal_listas
*/