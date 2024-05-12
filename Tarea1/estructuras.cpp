#include <vector>
#include <cmath>
#include <algorithm>
#include <random>
#include <iostream>

struct Point {
    double x, y;

    // Constructor por defecto
    Point() : x(0), y(0) {}

    // Constructor con parmetros
    Point(double x, double y) : x(x), y(y) {}

    // Calcular la distancia euclidiana entre dos puntos
    static double distance(const Point& a, const Point& b) {
        return std::sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
    }

    // Método para imprimir un punto
    void print() const {
        std::cout << "(" << x << ", " << y << ")";
    }
};

// Calcular la distancia euclidiana entre dos puntos
    static double distance(const Point& a, const Point& b) {
        return std::sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
    }


struct MTreeNode {
    Point p;               // Punto que representa el nodo
    double cr = 0;     // Radio cobertor
    std::vector<MTreeNode*> children;  // Hijos del nodo

    MTreeNode(Point p) : p(p) {}

    // Destructor que libera la memoria de los nodos hijos
    ~MTreeNode() {
        for (auto child : children)
            delete child;
    }

    // Función para añadir un hijo al nodo
    void addChild(MTreeNode* child) {
        children.push_back(child);
    }

    // Función opcional para establecer el radio cobertor del nodo
    void setRadius(double r) {
        cr = r;
    }
};


