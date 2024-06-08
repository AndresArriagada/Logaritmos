#ifndef FIBONACCI_HEAP_H
#define FIBONACCI_HEAP_H

#include <unordered_map>
#include <list>
#include <vector>
#include <limits>
#include "DijkstraHeap.h"
#include "DijkstraHeap.cpp"

//Template de como se representan los pares
//Definimos el nodo de la cola de fibonacci
    struct FibNode{
        int key;
        Nodo* nodo;      //Nodo al que se esta representando con FibNode
        FibNode* parent;   // Puntero al nodo padre
        FibNode* child;    // Puntero a uno de los hijos del nodo
        FibNode* left;     // Puntero al nodo izquierdo en la lista de hermanos
        FibNode* right;    // Puntero al nodo derecho en la lista de hermanos
        int degree;     // Grado del nodo (número de hijos que tiene)
        bool mark;      // Indica si el nodo ha perdido un hijo desde la última vez que fue hecho hijo de otro nodo

        FibNode(std::pair<int, Nodo*> p) : key(p.first), nodo(p.second), parent(nullptr), child(nullptr), left(this), right(this), degree(0), mark(false) {}
    };

//Clase que representa la estructura de Cola de Fibonacci
class FibHeap {
public:

    


    //Constructor por defecto de FibHeap
    FibHeap() : minNode(nullptr), nodeCount(0) {}


    //Metodo de insercion:
    FibNode* insert(std::pair<int, Nodo*> p) {
        FibNode* node = new FibNode(p);
        if (!minNode) {
            minNode = node;
        } else {
            minNode->left->right = node;
            node->right = minNode;
            node->left = minNode->left;
            minNode->left = node;
            if (p.first < minNode->key) {
                minNode = node;
            }
        }
        ++nodeCount;
        return node;
    }

    //Metodo para determinar si esta vacio
    bool isEmpty () {return minNode == nullptr;}

    //Metodo que retorna el minimo
    FibNode* getMin() {return minNode;}

    //Metodo que une dos colas de fibonacci
    void merge(FibHeap& other) {
        //Si la otra cola esta vacia, no hace nada
        if (!other.minNode) return;
        if (!minNode) {
            minNode = other.minNode;
        } else {
            minNode->left->right = other.minNode->right;
            other.minNode->right->left = minNode->left;
            minNode->left = other.minNode;
            other.minNode->right = minNode;
            if (other.minNode->key < minNode->key) {
                minNode = other.minNode;
            }
        }
        nodeCount += other.nodeCount;
        other.minNode = nullptr;
        other.nodeCount = 0;
    }

    //Metodo que disminuye la llave para el nodo en caso que la nueva sea menor
    void decreaseKey(FibNode* node, int newKey) {
        if (newKey > node->key) {
            throw std::invalid_argument("New key is greater than current key.");
        }
        node->key = newKey;
        //Ahora verificamos si es menor que el padre, y de serlo debemos hacer la modificacion de cortar para mantener que los hijos sean siempre menores
        FibNode* parent = node->parent;
        if (parent && node->key < parent->key) {
            cut(node, parent);
            cascadingCut(parent);
        }
        if (node->key < minNode->key) {
            minNode = node;
        }
    }

    //Metodo para eliminar el minimo de la cola
    void deleteMin() {
        //Caso de cola vacia
        if (!minNode) return;

        //Si tiene hijos, los movemos todos a la lista de raices
        if (minNode->child) {
            FibNode* child = minNode->child;
            do {
                child->parent = nullptr;
                child = child->right;
            } while (child != minNode->child);

            minNode->left->right = minNode->child->right;
            minNode->child->right->left = minNode->left;
            minNode->child->right = minNode;
            minNode->left = minNode->child->left;
        }
        //Luego de haber sacado todos los hijos, eliminamos el nodo de la lista
        minNode->left->right = minNode->right;
        minNode->right->left = minNode->left;

        //Y finalmente asignamos el nuevo minimo
        if (minNode == minNode->right) {
            delete minNode;
            minNode = nullptr;
        } else { //En caso que no sea el unico nodo, definimos un nuevo minimo y consolidamos el arbol para mantener la estructura
            FibNode* oldMin = minNode;
            minNode = minNode->right;
            delete oldMin;
            consolidate();
        }
        --nodeCount;
    }


private:
    FibNode* minNode;
    int nodeCount;

    //Metodo para cortar un nodo de un arbol y lo mueve a la lista de raices
    void cut(FibNode* node, FibNode* parent) {
        //Verifica si es el unico hijo
        if (node->right == node) {
            parent->child = nullptr;
        } else {//Si no lo es, entonces reconecta entre si los nodos a sus lados
            node->left->right = node->right;
            node->right->left = node->left;
            if (parent->child == node) {//En caso que el parent este apuntando a este, lo apunta al siguiente
                parent->child = node->right;
            }
        }
        parent->degree--;//disminuye en 1 el contador de hijos del padre

        //Preparamos el nodo para agregarlo a la lista de raices
        node->right = node->left = node;
        //Lo agregamos a la lista de raices
        minNode->left->right = node;
        node->right = minNode;
        node->left = minNode->left;
        minNode->left = node;
        //Quitamos el puntero al padre
        node->parent = nullptr;
        //Quitamos la marca
        node->mark = false;
    }

    //A la vez, tenemos el cascadingCut que usaremos como metodo para balancear el arbol. Si un nodo le cortan 2 hijos lo tomaremos como que esta mal balanceado y lo agregaremos a la lista de raices como nuevo arbol. Para eso usaremos el mark para señalar si este nodo ya tuvo un corte en uno de sus hijos
    void cascadingCut(FibNode* node) {
        FibNode* parent = node->parent;
        if (parent) {
            //Si no tiene marca, lo marcamos
            if (!node->mark) {
                node->mark = true;
            } else { //si esta marcado, lo cortamos y procedemos a aplicar cascadingCut al padre
                cut(node, parent);
                cascadingCut(parent);
            }
        }
    }

    //Metodo de consolidacion del heap. Con esto nos aseguramos de mantener la estrcutura y preservar la eficiencia de la misma
    void consolidate() {
        //Creamos una tabla de grados que puede recibir hasta 2^15 nodos
        std::vector<FibNode*> degreeTable(15, nullptr);  
        
        std::list<FibNode*> nodeList;
        FibNode* curr = minNode;
        //En esta parte agregamos todos los nodos a nodeList
        do {
            nodeList.push_back(curr);
            curr = curr->right;
        } while (curr != minNode);

        //Usa la degreeTable para asegurar que no hay 2 arboles del mismo grado
        for (auto& node : nodeList) {
            FibNode* x = node;
            int degree = x->degree;
            while (degreeTable[degree]) {
                FibNode* y = degreeTable[degree];
                if (x->key > y->key) {
                    std::swap(x, y);
                }
                link(y, x);
                degreeTable[degree] = nullptr;
                degree++;
            }
            degreeTable[degree] = x;
        }

        //Finalmente reconstruye la lista de raices
        minNode = nullptr;
        for (auto& node : degreeTable) {
            if (node) {
                if (!minNode) {
                    minNode = node;
                } else {
                    node->left->right = node->right;
                    node->right->left = node->left;

                    minNode->left->right = node;
                    node->right = minNode;
                    node->left = minNode->left;
                    minNode->left = node;

                    if (node->key < minNode->key) {
                        minNode = node;
                    }
                }
            }
        }
    }


    //Permite conectar dos arboles de la lista de raices en uno solo
    void link(FibNode* y, FibNode* x) {
        //Parte quitando al nodo y de la lista de nodos y agregandole X como padre
        y->left->right = y->right;
        y->right->left = y->left;
        y->parent = x;

        //si X no tiene hijos, entonces es el unico hijo de x y se agrega como tal
        if (!x->child) {
            x->child = y;
            y->right = y->left = y;
        } else {//en caso que si tenga hijos, agregamos y a la lista de hijos de x
            y->left = x->child;
            y->right = x->child->right;
            x->child->right->left = y;
            x->child->right = y;
        }
        //aumentamos el conteo de hijos de x y cambiamos la marca y a false
        x->degree++;
        y->mark = false;
    }

};


std::pair<std::vector<int>, std::vector<int>> DijkstraFibHeap(Grafo& grafo, int raiz);

#endif