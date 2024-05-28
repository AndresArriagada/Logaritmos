#include "DijkstraHeap.h"
#include <iostream>
#include <chrono>
#include <random>
#include <vector>

using namespace std;
using namespace chrono;

void experimento() {
    vector<int> sizes = {1024, 4096, 16384}; // v = 2^10, 2^12, 2^14
    vector<int> edgesPowers = {16, 17, 18, 19, 20, 21, 22}; // e = 2^16 a 2^22
    int repetitions = 50;

    for (int i : sizes) {
        for (int exp : edgesPowers) {
            int e = 1 << exp; // Calcula 2^j aristas

            for (int r = 0; r < repetitions; ++r) {
                Grafo grafo;
                for (int n = 0; n < i; ++n) {
                    grafo.agregarNodo(n);
                }

                // Generar un grafo conexo, empezando con un árbol de expansión mínima
                random_device rd;
                mt19937 gen(rd());
                uniform_int_distribution<> dist(0, i - 1);

                // Asegurar la conectividad primero
                for (int n = 1; n < i; ++n) {
                    int u = dist(gen) % n; // Conectar con un nodo anterior
                    int peso = dist(gen) % 100 + 1;
                    grafo.agregarArista(grafo.nodos[u], grafo.nodos[n], peso);
                    grafo.agregarArista(grafo.nodos[n], grafo.nodos[u], peso); // Hacerlo no dirigido
                }

                // Añadir aristas adicionales
                for (int j = 0; j < e - (i - 1); ++j) {
                    int u = dist(gen);
                    int w = dist(gen);
                    while (u == w) { w = dist(gen); } // Evitar bucles
                    int peso = dist(gen) % 100 + 1;
                    grafo.agregarArista(grafo.nodos[u], grafo.nodos[w], peso);
                    grafo.agregarArista(grafo.nodos[w], grafo.nodos[u], peso); // Hacerlo no dirigido
                }

                // Medir tiempo de ejecución del algoritmo de Dijkstra
                auto start = high_resolution_clock::now();
                auto resultados = DijkstraHeap(grafo, 0); // Ejecutar Dijkstra desde el nodo 0
                auto stop = high_resolution_clock::now();
                auto duration = duration_cast<milliseconds>(stop - start);

                cout << "Tiempo para 2^" << log2(i) << " nodos y 2^" << exp << " aristas, rep " << (r + 1) << ": " << duration.count() << " milisegundos" << endl;
            }
        }
    }
}

int main() {
    experimento();
    return 0;
}