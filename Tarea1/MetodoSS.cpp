#include <iostream>
#include <cmath>
#include <algorithm>
#include "estructuras.cpp"
#include <limits>
#include <vector>

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
void findClosestPair(const std::vector<std::vector<Point>>& clusters, int& idx1, int& idx2);
void cluster(const Point* points, int numPoints, std::vector<std::vector<Point>>& clusters, int minSize, int maxSize);
Node* buildMTree(const Point* points, int numPoints, int minSize, int maxSize);
void findMedoidAndRadius(const Point* points, int numPoints, Point& medoid, double& radius);
void printMTree(Node* node, int level = 0);

// Implementación de funciones

void findClosestPair(const std::vector<std::vector<Point>>& clusters, int& idx1, int& idx2) {
    double minDistance = std::numeric_limits<double>::max();
    for (int i = 0; i < clusters.size(); ++i) {
        for (int j = i + 1; j < clusters.size(); ++j) {
            Point medoid1, medoid2;
            double radius1, radius2;
            findMedoidAndRadius(clusters[i].data(), clusters[i].size(), medoid1, radius1);
            findMedoidAndRadius(clusters[j].data(), clusters[j].size(), medoid2, radius2);
            double distance = Point::distance(medoid1, medoid2);
            if (distance < minDistance) {
                minDistance = distance;
                idx1 = i;
                idx2 = j;
            }
        }
    }
}

void cluster(const Point* points, int numPoints, std::vector<std::vector<Point>>& clusters, int minSize, int maxSize) {
    clusters.clear();
    if (numPoints == 0) return;

    // Distribuir los puntos en varios clusters sin exceder maxSize
    std::vector<Point> currentCluster;
    for (int i = 0; i < numPoints; ++i) {
        currentCluster.push_back(points[i]);
        if (currentCluster.size() == maxSize) {
            clusters.push_back(currentCluster);
            currentCluster.clear();
        }
    }
    if (!currentCluster.empty()) {
        clusters.push_back(currentCluster);
    }

    std::cout << "Clusters created: " << clusters.size() << std::endl;
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
    std::cout << "Building MTree with numPoints: " << numPoints << std::endl;
    
    if (numPoints <= maxSize) {
        std::cout << "Creating leaf node for " << numPoints << " points." << std::endl;
        Point medoid;
        double radius;
        findMedoidAndRadius(points, numPoints, medoid, radius);
        return new Node(medoid, radius, points, numPoints);
    }

    // Caso recursivo: si el número de puntos excede el tamaño máximo, se procede a clusterizar.
    std::vector<std::vector<Point>> clusters;
    cluster(points, numPoints, clusters, minSize, maxSize);

    Node* children[MAX_CHILDREN];
    int numChildren = 0;

    std::cout << "Clusters created: " << clusters.size() << std::endl;

    // Construye un árbol para cada cluster
    for (auto& cluster : clusters) {
        children[numChildren++] = buildMTree(cluster.data(), cluster.size(), minSize, maxSize);
    }

    // Calcular el medoide y el radio para el nodo interno usando los medoides de los hijos.
    std::vector<Point> medoids;
    for (int i = 0; i < numChildren; ++i) {
        medoids.push_back(children[i]->medoid);
    }
    Point medoid;
    double radius;
    findMedoidAndRadius(medoids.data(), medoids.size(), medoid, radius);

    std::cout << "Creating internal node with " << numChildren << " children." << std::endl;
    
    return new Node(medoid, radius, children, numChildren);
}

void printMTree(Node* node, int level) {
    if (!node) return;

    std::cout << std::string(level * 2, ' ') << "Nivel " << level << ": ";
    std::cout << "Medoide (" << node->medoid.x << ", " << node->medoid.y << "), ";
    std::cout << "Radio " << node->radius << std::endl;

    if (node->numChildren == 0) {
        std::cout << std::string(level * 2, ' ') << "  Puntos:" << std::endl;
        for (int i = 0; i < node->numPoints; ++i) {
            std::cout << std::string(level * 2 + 2, ' ') << "(" << node->points[i].x << ", " << node->points[i].y << ")" << std::endl;
        }
    } else {
        for (int i = 0; i < node->numChildren; ++i) {
            printMTree(node->children[i], level + 1);
        }
    }
}

int main() {
    Point points[] = {{0, 0}, {1, 1}, {2, 2}, {3, 3}, {4, 4}, {5, 5}, {6, 6}, {7, 7}, {8, 8}, {9, 9}};
    int numPoints = sizeof(points) / sizeof(points[0]);
    int minSize = 2;
    int maxSize = 3;

    Node* root = buildMTree(points, numPoints, minSize, maxSize);

    printMTree(root);

    delete root;

    return 0;
}