#include "estructuras.cpp" // Incluye el archivo que contiene las estructuras de Point y MTreeNode
#include <vector>
#include <random>
#include <memory>
#include <iostream>
#include <cmath>


class MTree {
public:
    MTreeNode* root = nullptr;
    int B;  // Capacidad máxima de puntos por nodo
    double b;  // Capacidad mínima de puntos por nodo

    MTree(int B) : B(B), b(B * 0.5) {}

    ~MTree() {
        delete root;
    }

    MTreeNode* buildCP(std::vector<Point>& points) {
        if (points.size() <= B) {
            MTreeNode* node = new MTreeNode(Point(0, 0));
            node->setRadius(0);
            for (const auto& point : points) {
                node->addChild(new MTreeNode(point));
                double dist = Point::distance(node->p, point);
                if (dist > node->cr) {
                    node->setRadius(dist);
                }
            }
            return node;
        }

        bool validClusters = false;
        std::vector<MTreeNode*> samples;
        std::vector<std::vector<Point>> clusters;

        int n = points.size();
        int k = std::max(2, std::min(B, static_cast<int>(std::ceil(static_cast<double>(n) / B))));

        while (!validClusters) {
            std::random_device rd;
            std::mt19937 g(rd());
            std::shuffle(points.begin(), points.end(), g);

            samples.clear();
            clusters.clear();
            clusters.resize(k);

            for (int i = 0; i < k; ++i) {
                samples.push_back(new MTreeNode(points[i]));
            }

            for (const auto& point : points) {
                int closestSampleIdx = 0;
                double minDist = Point::distance(point, samples[0]->p);
                for (int i = 1; i < k; ++i) {
                    double dist = Point::distance(point, samples[i]->p);
                    if (dist < minDist) {
                        closestSampleIdx = i;
                        minDist = dist;
                    }
                }
                clusters[closestSampleIdx].push_back(point);
            }

            // Redistribución de puntos
            for (int i = 0; i < k; ++i) {
                if (clusters[i].size() < b) {
                    for (const auto& point : clusters[i]) {
                        int closestSampleIdx = -1;
                        double minDist = std::numeric_limits<double>::max();
                        for (int j = 0; j < k; ++j) {
                            if (i != j && clusters[j].size() > b) {
                                double dist = Point::distance(point, samples[j]->p);
                                if (dist < minDist) {
                                    closestSampleIdx = j;
                                    minDist = dist;
                                }
                            }
                        }
                        if (closestSampleIdx != -1) {
                            clusters[closestSampleIdx].push_back(point);
                        }
                    }
                    clusters[i].clear();
                }
            }

            // Verifica si hay más de un cluster con puntos
            int activeClusters = 0;
            for (const auto& cluster : clusters) {
                if (!cluster.empty()) {
                    activeClusters++;
                }
            }

            validClusters = (activeClusters > 1);
        }

        // Construcción recursiva del árbol para cada cluster
        for (int i = 0; i < k; ++i) {
            if (!clusters[i].empty()) {
                samples[i]->addChild(buildCP(clusters[i])); // Construye un subárbol para cada cluster y lo añade como hijo
            }
        }

         // Determinar la altura mínima h de los subárboles
        std::vector<int> heights(samples.size(), 0);
        for (int i = 0; i < samples.size(); ++i) {
            if (!clusters[i].empty()) {
                heights[i] = calculateHeight(samples[i]);
            }
        }

        int minHeight = *std::min_element(heights.begin(), heights.end());


        //Ahora procedemos a ajustar el arbol de forma que ninguna de sus ramas tenga una altura mayor a h.
        bool needsAdjustment;
        do {
            needsAdjustment = false;  // Flag para detectar si se necesitan más ajustes
            for (int i = 0; i < samples.size(); ++i) {
                if (calculateHeight(samples[i]) > minHeight) {
                    needsAdjustment = true;  // Se necesitarán más ajustes, ya que no sabemos si los hijos aun seran mayores que h
                    // Mover los hijos del nodo sample actual a samples
                    std::vector<MTreeNode*> newSamples;
                    for (auto child : samples[i]->children) {
                        newSamples.push_back(child);  // Preparar para agregar al nivel superior
                    }
                    samples[i]->children.clear();  // Limpiar hijos para evitar su eliminación
                    delete samples[i];  // Eliminar el nodo actual
                    samples.erase(samples.begin() + i);  // Eliminar de samples
                    samples.insert(samples.end(), newSamples.begin(), newSamples.end());  // Añadir los nuevos samples
                    break;  // Romper el ciclo para reiniciar la verificación
                }
            }
        } while (needsAdjustment);  // Continuar mientras se necesiten ajustes


        //En este punto ya tenemos todos los nodos en sample, balanceados y bien construidos, por lo que faltaria es crear un nuevo arbol.


        // Crea un nuevo nodo raíz para este nivel y agrega los samples como hijos
        MTreeNode* node = new MTreeNode(Point(0.5, 0.5)); // Nodo con punto central arbitrario
        for (auto sample : samples) {
            if (!sample->children.empty()) { // Asegura que solo se añadan samples con hijos
                node->addChild(sample);
            } else {
                delete sample; // Elimina los samples sin hijos para evitar fugas de memoria
            }
        }

        return node; // Retorna el nodo raíz del subárbol construido
    }

    int calculateHeight(MTreeNode* node) {
        if (node->children.empty()) return 0;
        int maxChildHeight = 0;
        for (auto child : node->children) {
            maxChildHeight = std::max(maxChildHeight, calculateHeight(child));
        }
        return 1 + maxChildHeight;
    }


};
