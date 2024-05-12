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
        std::cout << ", Radius: " << radius << "\n";
        if (numPoints > 0) {
            std::cout << std::string(level * 2, ' ') << "Points:\n";
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

int main() {
    std::cout << "Starting program...\n";

    Point points[] = {{0, 0}, {1, 1}};
    int numPoints = sizeof(points) / sizeof(points[0]);

    // Crear un nodo con los puntos
    Node node(Point(0.5, 0.5), 1.0, points, numPoints);

    // Llamar al método print para mostrar el contenido del nodo
    node.print();

    std::cout << "Program finished.\n";
    return 0;
}