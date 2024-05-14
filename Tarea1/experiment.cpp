#include <iostream>
#include <cmath>
#include <vector>
#include <random>
#include <chrono>
#include "MetodoSS.cpp"  
#include "MetodoCP.cpp"  
#include "estructuras.cpp"  


int main() {
    std::cout << "Inicia el experimento" << std::endl;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0, 1);

    for (int i = 10; i <= 25; i++) {
        std::cout << "Probando para N = 2^" << i << std::endl;

        int size = static_cast<int>(pow(2, i));
        std::vector<Point> points;
        points.reserve(size);

        for (int j = 0; j < size; j++) {
            points.emplace_back(dis(gen), dis(gen));
        }

        (const Point*) points.data();
        int numPoints = points.size();

        MTree cpTree(4096);
        auto startCP = std::chrono::high_resolution_clock::now();
        MTreeNode* rootCP = cpTree.buildCP(points);
        auto endCP = std::chrono::high_resolution_clock::now();
        std::cout << "Creation time CP: " << std::chrono::duration_cast<std::chrono::milliseconds>(endCP - startCP).count() << " ms." << std::endl;

        int maxSize = 4096;
        int minSize = maxSize / 2;
        auto startSS = std::chrono::high_resolution_clock::now();
        Node* rootSS = buildMTree(const Point*) points.data(), numPoints, minSize, maxSize);
        auto endSS = std::chrono::high_resolution_clock::now();
        std::cout << "Creation time SS: " << std::chrono::duration_cast<std::chrono::milliseconds>(endSS - startSS).count() << " ms." << std::endl;

        // Inicia las búsquedas
        for (int k = 0; k < 5; k++) {
            Point queryPoint(dis(gen), dis(gen));
            double range = dis(gen) * 0.1;  // Rango de búsqueda hasta 0.1

            // Búsqueda en CP
            int entriesCP = 0;
            auto startSearchCP = std::chrono::high_resolution_clock::now();
            auto resultCP = searchMTreeCP((MTreeNode *) rootCP, queryPoint, range, entriesCP);
            auto endSearchCP = std::chrono::high_resolution_clock::now();

            // Búsqueda en SS
            int entriesSS = 0;
            auto startSearchSS = std::chrono::high_resolution_clock::now();
            auto resultSS = searchMTreeSS(rootSS, queryPoint, range, entriesSS);
            auto endSearchSS = std::chrono::high_resolution_clock::now();

            std::cout << "Query " << k + 1 << ":" << std::endl;
            std::cout << "CP Search - Time: " << std::chrono::duration_cast<std::chrono::milliseconds>(endSearchCP - startSearchCP).count() 
                      << " ms, Accesses: " << resultCP.second << ", Points found: " << resultCP.first.size() << std::endl;
            std::cout << "SS Search - Time: " << std::chrono::duration_cast<std::chrono::milliseconds>(endSearchSS - startSearchSS).count() 
                      << " ms, Accesses: " << entriesSS << ", Points found: " << resultSS.first.size() << std::endl;
        }

        delete rootCP;
        deleteTreeSS(rootSS);
    }

    return 0;
}