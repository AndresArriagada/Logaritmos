#include <vector>
#include <unordered_map>
#include <limits>
#include <iostream>
#include "DijkstraFibHeap.h" // Assuming this file contains the necessary grafo structures and methods.
#include "DijkstraHeap.h"
#include "DijkstraHeap.cpp"

using namespace std;

pair<vector<int>, vector<int>> DijkstraFibHeap(Grafo& grafo, int raiz) {
    //obtenemos el numero de nodos
    int n = grafo.nodos.size();
    //Creamos el vector de distancias con todas en el maximo permitido por int
    vector<int> distancias(n, INT_MAX);
    //Inicializamos todos los predecesores a -1
    vector<int> previos(n, -1);

    //Obtenemos los nodos del grafo
    vector<Nodo*> nodos = grafo.obtenerNodos(); 

    //Creamos un map sin orden para agregar los punteros los nodos del FibHeap
    unordered_map<int, FibNode*> node_map;

    //Creamos el FibHeap
    FibHeap fibHeap;

    //definir la raiz, su distancia y agregarla a los arreglos de la solucion 
    distancias[raiz] = 0;

    for (int i = 0; i < n; i++) {
        if (i != raiz) {
            //Para cada nodo que no es la raiz, definimos su distancia como el INT_MAX y su nodo previo como -1 que representa indefinido
            distancias[i] = INT_MAX;
            previos[i] = -1;
            // Agregamos el par a Q
            node_map[i] = fibHeap.insert(make_pair(INT_MAX, grafo.nodos[i]));
        }
    }

    while (!fibHeap.isEmpty()) {
        FibNode* minNode = fibHeap.getMin();
        Nodo* v = minNode->nodo;
        fibHeap.deleteMin();

        // Por cada vecino u del nodo v
        for (auto arista : v->aristas) {
            Nodo* u = arista.first;
            int peso = arista.second;
            if (distancias[u->valor] > distancias[v->valor] + peso) {
                // Guardamos v como el nodo previo de u
                previos[u->valor] = v->valor;
                // Actualizamos la distancia de u
                distancias[u->valor] = distancias[v->valor] + peso;
                // Actualizamos la distancia del par que representa a u en Q
                fibHeap.decreaseKey(node_map[u->valor], distancias[u->valor]);
            }
        }
    }

    return make_pair(previos,distancias);
}
