/*
GRAFO BIPARTITO CON LISTAS DE ADYACENCIA
Este programa implementa un grafo bipartito ponderado donde:
- Se lee el archivo "bitacora.txt" (o "orders.txt") y almacena los datos en una lista de adyacencia organizada por platillo
- Grafo bipartito: Nodos de restaurantes y nodos de platillos
- Grafo ponderado: Las aristas tienen peso (frecuencia de pedidos)
- La relación es DIRIGIDA: Platillo -> Restaurante (un platillo "apunta" a los restaurantes donde se vende)
- Se utiliza BFS para mostrar cómo un platillo se reparte en diferentes restaurantes

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
#include <algorithm>
#include <string>

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

// BFS: Muestra cómo un platillo se reparte en diferentes restaurantes
void ejecutarBFS(int nodoInicio) {
    if (nodoInicio < 0 || nodoInicio >= numNodos) return;
    
    if (grafo[nodoInicio].tipo != 'P') {
        cout << "Error: El BFS debe iniciarse desde un platillo." << endl;
        return;
    }

    cout << "\n=== BÚSQUEDA BFS: DISTRIBUCIÓN DEL PLATILLO EN RESTAURANTES ===" << endl;
    cout << "Platillo: " << grafo[nodoInicio].nombre << endl;
    cout << string(60, '-') << endl;

    bool visitado[MAX_NODOS] = {false};
    queue<int> cola;

    visitado[nodoInicio] = true;
    cola.push(nodoInicio);

    int totalPedidos = 0;
    int numRestaurantes = 0;
    
    int actual = cola.front();
    cola.pop();
    
    cout << "\nRestaurantes donde se ofrece este platillo:" << endl;
    cout << string(60, '-') << endl;
    
    NodoAdyacencia* temp = grafo[actual].cabezaLista;
    while (temp != nullptr) {
        int vecinoId = temp->idDestino;
        if (!visitado[vecinoId] && grafo[vecinoId].tipo == 'R') {
            numRestaurantes++;
            totalPedidos += temp->peso;
            
            cout << "[" << numRestaurantes << "] " << grafo[vecinoId].nombre 
                 << " - Pedidos: " << temp->peso << endl;
            
            visitado[vecinoId] = true;
            cola.push(vecinoId);
        }
        temp = temp->siguiente;
    }
    
    cout << string(60, '-') << endl;
    cout << "Total de restaurantes: " << numRestaurantes << endl;
    cout << "Total de pedidos: " << totalPedidos << endl;
    cout << string(60, '=') << endl;
}

// --- FUNCIONES DE VISUALIZACIÓN MEJORADAS ---

// Genera y muestra la matriz de adyacencia
void mostrarMatrizAdyacencia() {
    cout << "\n=== MATRIZ DE ADYACENCIA ===" << endl;
    
    int numPlatillos = 0, numRestaurantes = 0;
    int indicesPlatillos[MAX_NODOS];
    int indicesRestaurantes[MAX_NODOS];
    
    for (int i = 0; i < numNodos; i++) {
        if (grafo[i].tipo == 'P') {
            indicesPlatillos[numPlatillos++] = i;
        } else if (grafo[i].tipo == 'R') {
            indicesRestaurantes[numRestaurantes++] = i;
        }
    }
    
    if (numPlatillos == 0 || numRestaurantes == 0) {
        cout << "No hay suficientes datos." << endl;
        return;
    }
    
    cout << "Generando matriz completa..." << endl;
    cout << "Total Platillos: " << numPlatillos << ", Total Restaurantes: " << numRestaurantes << endl;
    
    // Guardar matriz completa en archivo
    ofstream archivo("matriz_adyacencia.txt");
    if (archivo.is_open()) {
        archivo << "MATRIZ DE ADYACENCIA" << endl;
        archivo << "Platillos: " << numPlatillos << ", Restaurantes: " << numRestaurantes << "\n" << endl;
        
        archivo << "PLATILLO";
        for (int j = 0; j < numRestaurantes; j++) {
            archivo << "\t" << grafo[indicesRestaurantes[j]].nombre;
        }
        archivo << endl;
        
        for (int i = 0; i < numPlatillos; i++) {
            int idPlatillo = indicesPlatillos[i];
            archivo << grafo[idPlatillo].nombre;
            
            for (int j = 0; j < numRestaurantes; j++) {
                int idRestaurante = indicesRestaurantes[j];
                NodoAdyacencia* temp = grafo[idPlatillo].cabezaLista;
                int peso = 0;
                
                while (temp != nullptr) {
                    if (temp->idDestino == idRestaurante) {
                        peso = temp->peso;
                        break;
                    }
                    temp = temp->siguiente;
                }
                
                archivo << "\t" << peso;
            }
            archivo << endl;
        }
        archivo.close();
        cout << "Matriz guardada en 'matriz_adyacencia.txt'" << endl;
    }
}

// Estructura para ordenar restaurantes por solicitudes
struct RestauranteInfo {
    int id;
    int totalSolicitudes;
    
    RestauranteInfo() {
        id = -1;
        totalSolicitudes = 0;
    }
    
    RestauranteInfo(int _id, int _solicitudes) {
        id = _id;
        totalSolicitudes = _solicitudes;
    }
};

// Encuentra y muestra los restaurantes con mayor cantidad de solicitudes
void restaurantesConMasSolicitudes() {
    cout << "\n=== RESTAURANTES CON MAYOR CANTIDAD DE SOLICITUDES ===" << endl;
    
    RestauranteInfo restaurantes[MAX_NODOS];
    int numRestaurantes = 0;
    
    // Inicializar restaurantes
    for (int i = 0; i < numNodos; i++) {
        if (grafo[i].tipo == 'R') {
            restaurantes[numRestaurantes].id = i;
            restaurantes[numRestaurantes].totalSolicitudes = 0;
            numRestaurantes++;
        }
    }
    
    // Sumar pedidos por restaurante
    for (int i = 0; i < numNodos; i++) {
        if (grafo[i].tipo == 'P') {
            NodoAdyacencia* temp = grafo[i].cabezaLista;
            while (temp != nullptr) {
                for (int j = 0; j < numRestaurantes; j++) {
                    if (restaurantes[j].id == temp->idDestino) {
                        restaurantes[j].totalSolicitudes += temp->peso;
                        break;
                    }
                }
                temp = temp->siguiente;
            }
        }
    }
    
    // Ordenar (bubble sort)
    for (int i = 0; i < numRestaurantes - 1; i++) {
        for (int j = 0; j < numRestaurantes - i - 1; j++) {
            if (restaurantes[j].totalSolicitudes < restaurantes[j + 1].totalSolicitudes) {
                RestauranteInfo temp = restaurantes[j];
                restaurantes[j] = restaurantes[j + 1];
                restaurantes[j + 1] = temp;
            }
        }
    }
    
    // Mostrar top 10
    int topN = min(10, numRestaurantes);
    cout << "\nTop " << topN << " restaurantes:\n" << endl;
    
    for (int i = 0; i < topN; i++) {
        cout << (i + 1) << ". " << grafo[restaurantes[i].id].nombre
             << " - Solicitudes: " << restaurantes[i].totalSolicitudes << endl;
    }
    
    // Guardar en archivo
    ofstream archivo("restaurantes_solicitudes.txt");
    if (archivo.is_open()) {
        for (int i = 0; i < numRestaurantes; i++) {
            archivo << (i + 1) << ". " << grafo[restaurantes[i].id].nombre 
                    << " - Solicitudes: " << restaurantes[i].totalSolicitudes << endl;
        }
        archivo.close();
        cout << "\nLista completa guardada en 'restaurantes_solicitudes.txt'" << endl;
    }
    
    cout << "================================" << endl;
}

// Muestra estadísticas del grafo
void mostrarEstadisticas() {
    cout << "\n=== ESTADÍSTICAS DEL GRAFO BIPARTITO ===" << endl;
    
    int numPlatillos = 0, numRestaurantes = 0;
    int totalConexiones = 0;
    int totalPedidos = 0;
    
    for (int i = 0; i < numNodos; i++) {
        if (grafo[i].tipo == 'P') {
            numPlatillos++;
            NodoAdyacencia* temp = grafo[i].cabezaLista;
            while (temp != nullptr) {
                totalConexiones++;
                totalPedidos += temp->peso;
                temp = temp->siguiente;
            }
        } else if (grafo[i].tipo == 'R') {
            numRestaurantes++;
        }
    }
    
    cout << "Total de nodos: " << numNodos << endl;
    cout << "  - Platillos: " << numPlatillos << endl;
    cout << "  - Restaurantes: " << numRestaurantes << endl;
    cout << "Total de conexiones: " << totalConexiones << endl;
    cout << "Total de pedidos: " << totalPedidos << endl;
    cout << "================================" << endl;
}

void listarRestaurantes() {
    cout << "\n=== LISTA DE RESTAURANTES ===" << endl;
    int contador = 0;
    for (int i = 0; i < numNodos; i++) {
        if (grafo[i].tipo == 'R') {
            cout << "- " << grafo[i].nombre << endl;
            contador++;
        }
    }
    cout << "Total: " << contador << " restaurantes" << endl;
}

// Muestra todas las conexiones del grafo
void mostrarTodasLasConexiones() {
    cout << "\n=== TODAS LAS CONEXIONES DEL GRAFO ===" << endl;
    cout << "Mostrando primeras 30 conexiones...\n" << endl;
    
    int contador = 0;
    int limite = 30;
    
    for (int i = 0; i < numNodos && contador < limite; i++) {
        if (grafo[i].tipo == 'P') {
            NodoAdyacencia* temp = grafo[i].cabezaLista;
            if (temp != nullptr) {
                cout << grafo[i].nombre << " ->" << endl;
                while (temp != nullptr && contador < limite) {
                    cout << "    " << grafo[temp->idDestino].nombre 
                         << " [Pedidos: " << temp->peso << "]" << endl;
                    contador++;
                    temp = temp->siguiente;
                }
                cout << endl;
            }
        }
    }
    
    if (contador >= limite) {
        cout << "... (mostrando solo primeros " << limite << ")" << endl;
    }
    
    // Guardar todas en archivo
    ofstream archivo("todas_conexiones.txt");
    if (archivo.is_open()) {
        for (int i = 0; i < numNodos; i++) {
            if (grafo[i].tipo == 'P') {
                NodoAdyacencia* temp = grafo[i].cabezaLista;
                if (temp != nullptr) {
                    archivo << grafo[i].nombre << " ->" << endl;
                    while (temp != nullptr) {
                        archivo << "    " << grafo[temp->idDestino].nombre 
                                << " [Pedidos: " << temp->peso << "]" << endl;
                        temp = temp->siguiente;
                    }
                    archivo << endl;
                }
            }
        }
        archivo.close();
        cout << "Todas las conexiones guardadas en 'todas_conexiones.txt'" << endl;
    }
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
    cout << "\n=== LISTA DE PLATILLOS ===" << endl;
    int contador = 0;
    for (int i = 0; i < numNodos; i++) {
        if (grafo[i].tipo == 'P') {
            cout << "- " << grafo[i].nombre << endl;
            contador++;
        }
    }
    cout << "Total: " << contador << " platillos" << endl;
}

int main() {
    // Intentar abrir bitacora.txt, si no existe, intentar orders.txt
    ifstream archivo("bitacora.txt");
    if (!archivo.is_open()) {
        archivo.open("orders.txt");
        if (!archivo.is_open()) {
            cout << "Error: No se pudo abrir 'bitacora.txt' ni 'orders.txt'" << endl;
            return 1;
        } else {
            cout << "Nota: Usando 'orders.txt' (no se encontró 'bitacora.txt')" << endl;
        }
    } else {
        cout << "Leyendo archivo 'bitacora.txt'..." << endl;
    }

    cout << "\nConstruyendo grafo bipartito con listas de adyacencia." << endl;
    cout << "Organizando datos por platillo." << endl;
    
    char buffer[MAX_NOMBRE];
    int lineasLeidas = 0;
    while (archivo.getline(buffer, MAX_NOMBRE)) {
        procesarLinea(buffer);
        lineasLeidas++;
    }
    archivo.close();
    
    cout << "✓ Archivo procesado: " << lineasLeidas << " líneas leídas" << endl;
    cout << "✓ Grafo bipartito construido: " << numNodos << " nodos totales" << endl;

    int opcion = 0;
    while (true) {
        cout << "\n=== MENÚ PRINCIPAL ===" << endl;
        cout << "1. Ver lista de platillos" << endl;
        cout << "2. Ver lista de restaurantes" << endl;
        cout << "3. Buscar platillo (BFS)" << endl;
        cout << "4. Restaurantes con mayor cantidad de solicitudes" << endl;
        cout << "5. Mostrar matriz de adyacencia" << endl;
        cout << "6. Mostrar estadísticas del grafo" << endl;
        cout << "7. Mostrar todas las conexiones" << endl;
        cout << "8. Salir" << endl;
        cout << "Seleccione: ";
        
        if (!(cin >> opcion)) {
            cout << "Entrada inválida." << endl;
            cin.clear();
            cin.ignore(10000, '\n'); // hardcoded limit ok for cin ignore
            continue;
        }
        cin.ignore(); 

        if (opcion == 8) break;

        switch(opcion) {
            case 1: listarPlatillos(); break;
            case 2: listarRestaurantes(); break;
            case 3: buscarPlatillo(); break;
            case 4: restaurantesConMasSolicitudes(); break;
            case 5: mostrarMatrizAdyacencia(); break;
            case 6: mostrarEstadisticas(); break;
            case 7: mostrarTodasLasConexiones(); break;
            default: cout << "Opción no válida. Por favor seleccione un número del 1 al 8." << endl;
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