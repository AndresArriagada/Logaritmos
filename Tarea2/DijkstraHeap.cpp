#include <vector>
#include <stdexcept>
#include <climits>

using namespace std;

// Definición de la clase Nodo
class Nodo {
public:
    int valor;
    vector<pair<Nodo*, int>> aristas; // Vector de pares (nodo, peso)

    Nodo(int v) : valor(v) {}
};

// Definición de la clase Grafo
class Grafo {
public:
    vector<Nodo*> nodos;

    // Función para agregar un nodo al grafo
    void agregarNodo(int valor) {
        Nodo* nuevoNodo = new Nodo(valor);
        nodos.push_back(nuevoNodo);
    }

    // Función para agregar una arista entre dos nodos con un peso dado
    void agregarArista(Nodo* nodoOrigen, Nodo* nodoDestino, int peso) {
        nodoOrigen->aristas.push_back(make_pair(nodoDestino, peso));
    }

    // Función para obtener la lista de nodos del grafo
    vector<Nodo*> obtenerNodos() {
        return nodos;
    }

    // Función para obtener las aristas de un nodo dado
    vector<pair<Nodo*, int>> obtenerAristasDeNodo(Nodo* nodo) {
        return nodo->aristas;
    }
};

class Heap {
private:
    vector<pair<int, Nodo*>> heap; // (distancia, nodo)
    int maxSize;
    int size;
    vector<int> nodeToIndex; // vector para acceso directo desde el nodo al par que lo representa

    int parent(int i) {
        return i / 2;
    }

    int leftChild(int i) {
        return 2 * i;
    }

    int rightChild(int i) {
        return 2 * i + 1;
    }

    void siftUp(int i) {
        while (i > 1 && heap[parent(i)].first > heap[i].first) {
            swap(i, parent(i));
            i = parent(i);
        }
    }

    void siftDown(int i) {
        int minIndex = i;
        int left = leftChild(i);
        if (left <= size && heap[left].first < heap[minIndex].first) {
            minIndex = left;
        }
        int right = rightChild(i);
        if (right <= size && heap[right].first < heap[minIndex].first) {
            minIndex = right;
        }
        if (i != minIndex) {
            swap(i, minIndex);
            siftDown(minIndex);
        }
    }

    void swap(int i, int j) {
        std::swap(heap[i], heap[j]);
        nodeToIndex[heap[i].second->valor] = i;
        nodeToIndex[heap[j].second->valor] = j;
    }

public:
    Heap(vector<int>& distances) : maxSize(distances.size() + 1), size(distances.size()), nodeToIndex(distances.size() + 1, -1) {
        heap.resize(distances.size() + 1);
        for (int i = 0; i < distances.size(); ++i) {
            heap[i + 1] = make_pair(distances[i], nullptr);
            nodeToIndex[i] = i + 1;
        }
        for (int i = size / 2; i >= 1; --i) {
            siftDown(i);
        }
    }

    bool empty() const {
        return size == 0;
    }

    void insert(pair<int, Nodo*> p) {
        if (size == maxSize - 1) {
            throw runtime_error("Heap is full!");
        }
        size++;
        heap[size] = p;
        nodeToIndex[p.second-> valor] = size;
        siftUp(size);
    }

    pair<int, Nodo*> extractMin() {
        if (size == 0) {
            throw runtime_error("Heap is empty!");
        }
        pair<int, Nodo*> result = heap[1];
        heap[1] = heap[size];
        nodeToIndex[heap[1].second->valor] = 1;
        size--;
        siftDown(1);
        nodeToIndex[result.second->valor] = -1;
        return result;
    }

    void decreaseKey(int node, int newDist) {
        int i = nodeToIndex[node];
        heap[i].first = newDist;
        siftUp(i);
    }
};


pair<vector<int>, vector<int>> DijkstraHeap(Grafo& grafo, int raiz){

    // Obtener el número total de nodos en el grafo
    int V = grafo.nodos.size();

    // Definir los arreglos
    vector<int> distancias(V, INT_MAX); // Inicializar todas las distancias a INT_MAX
    vector<int> previos(V, -1); // Inicializar todos los predecesores a -1
    // Obtener todos los nodos del grafo
    vector<Nodo*> nodos = grafo.obtenerNodos();
    
    // Definir la raíz y agregarla al heap
    distancias[raiz] = 0; // Definir la distancia de la raíz como 0
    previos[raiz] = -1; // Definir el nodo previo de la raíz como -1
    
    Heap heap(distancias);

    // Por cada nodo v distinto de la raíz en el grafo
    for (int i = 0; i < V; i++) {
        if (i != raiz) {
            // Definimos distancias[v] como infinita y previos[v] como indefinido
            distancias[i] = INT_MAX;
            previos[i] = -1;
            // Agregamos el par a Q
            heap.insert(make_pair(INT_MAX, grafo.nodos[i]));
        }
    }

    while (!heap.empty()) {
        pair<int, Nodo*> minPair = heap.extractMin();
        int d = minPair.first;
        Nodo* v = minPair.second;

        // Por cada vecino u del nodo v
        for (auto arista : v->aristas) {
            Nodo* u = arista.first;
            int peso = arista.second;
            // Si la distancia de u es mayor a la distancia de v más el peso de la arista
            if (distancias[u->valor] > distancias[v->valor] + peso) {
                // Guardamos v como el nodo previo de u
                previos[u->valor] = v->valor;
                // Actualizamos la distancia de u
                distancias[u->valor] = distancias[v->valor] + peso;
                // Actualizamos la distancia del par que representa a u en Q
                heap.decreaseKey(u->valor, distancias[u->valor]);
            }
        }
    }
    return make_pair(previos, distancias);
}