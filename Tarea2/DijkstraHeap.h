#ifndef DIJKSTRA_HEAP_H
#define DIJKSTRA_HEAP_H

#include <vector>
#include <utility>

class Nodo {
public:
    int valor;
    std::vector<std::pair<Nodo*, int>> aristas;
    Nodo(int v); // Constructor
};

class Grafo {
public:
    std::vector<Nodo*> nodos;
    void agregarNodo(int valor);
    void agregarArista(Nodo* nodoOrigen, Nodo* nodoDestino, int peso);
    std::vector<Nodo*> obtenerNodos();
};

class Heap {
public:
    std::vector<std::pair<int, Nodo*>> heap;
    int maxSize;
    int size;
    std::vector<int> nodeToIndex;

    Heap(std::vector<int>& distances);
    bool empty() const;
    void insert(std::pair<int, Nodo*> p);
    std::pair<int, Nodo*> extractMin();
    void decreaseKey(int node, int newDist);

private:
    int parent(int i) const;
    int leftChild(int i) const;
    int rightChild(int i) const;
    void siftUp(int i);
    void siftDown(int i);
    void swap(int i, int j);
};

std::pair<std::vector<int>, std::vector<int>> DijkstraHeap(Grafo& grafo, int raiz);

#endif