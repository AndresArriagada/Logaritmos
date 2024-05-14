#include <iostream>
#include <cmath>
#include <algorithm>
#include "estructuras.cpp"

constexpr int MAX_POINTS = 100;
constexpr int MAX_CLUSTERS = 10;
constexpr int MAX_CHILDREN = 10;

// Nodo para el M-tree
struct Node {
    Point medoid;
    double radius = 0;
    Point points[MAX_POINTS];
    int numPoints = 0;
    Node* children[MAX_CHILDREN] = {nullptr};
    int numChildren = 0;

    // Constructor para nodos hoja
    Node(const Point& medoid, double radius, const Point* pts, int count)
        : medoid(medoid), radius(radius), numPoints(count) {
        std::copy(pts, pts + count, points);
    }

    // Constructor para nodos internos
    Node(const Point& medoid, double radius, Node** ch, int count)
        : medoid(medoid), radius(radius), numChildren(count) {
        std::copy(ch, ch + count, children);
    }

    // Método para imprimir el contenido del nodo
    void print(int level = 0) const {
        std::cout << std::string(level * 2, ' ') << "Medoid: ";
        medoid.print();
        std::cout << ", Radius: " << radius << " ";
        if (numPoints > 0) {
            std::cout << std::string(level * 2, ' ') << "Points: ";
            for (int i = 0; i < numPoints; ++i) {
                std::cout << std::string(level * 2 + 2, ' ');
                points[i].print();
                std::cout << "\n";
            }
        } else {
            std::cout << std::string(level * 2, ' ') << "Children:\n";
            for (int i = 0; i < numChildren; ++i) {
                children[i]->print(level + 1);
            }
        }
    }
};

// Prototipos de funciones
void cluster(const Point* points, int numPoints, Point clusters[MAX_CLUSTERS][MAX_POINTS], int& numClusters, int clusterSizes[MAX_CLUSTERS], int minSize, int maxSize);
Node* buildMTree(const Point* points, int numPoints, int minSize, int maxSize);
void findMedoidAndRadius(const Point* points, int numPoints, Point& medoid, double& radius);

// Implementación de funciones

void cluster(const Point* points, int numPoints, Point clusters[MAX_CLUSTERS][MAX_POINTS], int& numClusters, int clusterSizes[MAX_CLUSTERS], int minSize, int maxSize) {
    // Implementar lógica de clustering basada en la descripción
    // Placeholder simple que coloca todos los puntos en un solo cluster
    numClusters = 1;
    std::copy(points, points + numPoints, clusters[0]);
    clusterSizes[0] = numPoints;
}

void findMedoidAndRadius(const Point* points, int numPoints, Point& medoid, double& radius) {
    // Calcular el medoide primario y el radio de cobertura
    double minDistSum = std::numeric_limits<double>::max();
    for (int i = 0; i < numPoints; ++i) {
        double distSum = 0;
        for (int j = 0; j < numPoints; ++j) {
            distSum += Point::distance(points[i], points[j]);
        }
        if (distSum < minDistSum) {
            minDistSum = distSum;
            medoid = points[i];
        }
    }
    // Calcular el radio de cobertura
    radius = 0;
    for (int i = 0; i < numPoints; ++i) {
        double dist = Point::distance(medoid, points[i]);
        if (dist > radius) {
            radius = dist;
        }
    }
}

Node* buildMTree(const Point* points, int numPoints, int minSize, int maxSize) {
    if (numPoints <= maxSize) {
        Point medoid;
        double radius;
        findMedoidAndRadius(points, numPoints, medoid, radius);
        return new Node(medoid, radius, points, numPoints);
    } else {
        Point clusters[MAX_CLUSTERS][MAX_POINTS];
        int clusterSizes[MAX_CLUSTERS] = {0};
        int numClusters = 0;

        cluster(points, numPoints, clusters, numClusters, clusterSizes, minSize, maxSize);

        Node* children[MAX_CLUSTERS] = {nullptr};
        for (int i = 0; i < numClusters; ++i) {
            children[i] = buildMTree(clusters[i], clusterSizes[i], minSize, maxSize);
        }

        Point medoid;
        double radius;
        Point medoides[MAX_CLUSTERS];
        for (int i = 0; i < numClusters; ++i) {
            medoides[i] = children[i]->medoid;
        }
        findMedoidAndRadius(medoides, numClusters, medoid, radius);

        return new Node(medoid, radius, children, numClusters);
    }
}

void deleteTreeSS(Node* node) {
    if (node == nullptr) return;  // Caso base de la recursión

    for (int i = 0; i < node->numChildren; ++i) {
        deleteTreeSS(node->children[i]);  // Llamada recursiva para cada hijo
    }

    delete node;  // Eliminar el nodo actual después de sus hijos
}


std::pair<std::vector<Point>, int> searchMTreeSS(Node* node, const Point& target, double range, int& entries) {
    std::vector<Point> foundPoints;
    if (node == nullptr) return {foundPoints, entries};

    entries++;  // Incrementar el contador de entradas cada vez que se accede a un nodo

    if (node->numChildren == 0) {  // Si es un nodo hoja
        for (int i = 0; i < node->numPoints; ++i) {
            if (Point::distance(node->points[i], target) <= range) {
                foundPoints.push_back(node->points[i]);  // Añadir punto si está dentro del rango
            }
        }
    } else {  // Si es un nodo interno
        for (int i = 0; i < node->numChildren; ++i) {
            auto [childFound, childEntries] = searchMTreeSS(node->children[i], target, range, entries);
            foundPoints.insert(foundPoints.end(), childFound.begin(), childFound.end());
            entries = childEntries;  // Actualizar el contador de entradas con el valor retornado por la recursión
        }
    }

    return {foundPoints, entries};  // Devolver los puntos encontrados y el contador de entradas
}


