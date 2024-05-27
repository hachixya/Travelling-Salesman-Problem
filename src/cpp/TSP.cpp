#include "TSP.hpp"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <csignal>
#include <climits> // Include this header for INT_MAX
#include <chrono>  // Include for measuring execution time

int done = 0;

TSP::TSP(const char *filename) {
    numCities = readFile(filename);
}

TSP::TSP(const TSP &source) {
    for (auto city : source.originalList) {
        originalList.push_back(new City(*city));
    }
    for (auto city : source.solution) {
        solution.push_back(new City(*city));
    }
    numCities = source.numCities;
}

TSP::~TSP() {
    originalList.clear();
    solution.clear();
}

int TSP::readFile(const char *filename) {
    int added = 0;
    int id, x, y;

    originalList.clear();
    std::ifstream file(filename);

    if (!file) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return 0;
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line == "NODE_COORD_SECTION") break;
    }

    while (file >> id >> x >> y) {
        originalList.push_back(new City(id, x, y, id, false));
        ++added;
    }

    file.close();
    return added;
}

void TSP::writeSolution(const char *fileName) {
    int distance = getSolutionDistance();
    std::ofstream file(fileName);

    if (file.is_open()) {
        file << distance << '\n';
    }

    for (auto city : solution) {
        city->writeToFile(file);
    }

    file.close();
}

int TSP::solveBruteForce() {
    if (numCities == 0) return 0;

    auto start = std::chrono::high_resolution_clock::now();

    int distance = 0;
    int minDistance = INT_MAX;
    std::deque<City*> bestPath;

    if (solution.empty()) solveNearestNeighborBasic(0);

    bruteForce(bestPath, minDistance, numCities);
    copyCityDeque(bestPath, solution);

    distance = getSolutionDistance();
    writeSolution("bruteforce.txt");

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;

    std::cout << "Best path distance: " << distance << std::endl;
    std::cout << "Best path: ";
    for (auto city : solution) {
        std::cout << city->getId() << " ";
    }
    std::cout << std::endl;
    std::cout << "Execution time: " << duration.count() << " seconds" << std::endl;

    return distance;
}

void TSP::bruteForce(std::deque<City*> &bestPath, int &minDistance, int citiesLeft) {
    int currentDistance = 0;
    signal(SIGTERM, endOptimization);

    for (int i = 0; !done && i < citiesLeft; ++i) {
        rotateSolution(citiesLeft - 1);
        currentDistance = getSolutionDistance();
        if (currentDistance < minDistance) {
            minDistance = currentDistance;
            std::cout << minDistance << std::endl;
            copyCityDeque(solution, bestPath);
        }
        bruteForce(bestPath, minDistance, citiesLeft - 1);
    }
}

void TSP::rotateSolution(int pos) {
    solution.push_front(solution[pos]);
    solution.erase(solution.begin() + pos + 1);
}

int TSP::solveNearestNeighbor() {
    if (numCities == 0) return 0;

    auto start = std::chrono::high_resolution_clock::now();

    solution.clear();
    int totalDistance = 0;
    int bestStartDistance = INT_MAX;
    int lastRun = 0;

    for (int i = 0; !done && i < numCities; ++i) {
        solveNearestNeighborBasic(i);
        lastRun = optimizeTwoChange();
        if (lastRun < bestStartDistance) {
            bestStartDistance = lastRun;
            std::cout << "Writing solution " << bestStartDistance << std::endl;
            writeSolution(OUTPUT_FN);
        }
    }

    totalDistance = getSolutionDistance();
    if (bestStartDistance > totalDistance) {
        bestStartDistance = totalDistance;
        std::cout << "Writing solution " << totalDistance << std::endl;
        writeSolution(OUTPUT_FN);
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;

    std::cout << "Best path distance: " << bestStartDistance << std::endl;
    std::cout << "Best path: ";
    for (auto city : solution) {
        std::cout << city->getId() << " ";
    }
    std::cout << std::endl;
    std::cout << "Execution time: " << duration.count() << " seconds" << std::endl;

    return bestStartDistance;
}

int TSP::solveNearestNeighborBasic(int startIndex) {
    if (numCities == 0) return 0;

    int citiesAdded = 0;
    int closest = INT_MAX;
    int totalDistance = 0;
    int currentDistance = 0;
    int closestIndex = 0;
    int currentNum = numCities;
    std::deque<City*> temp;
    copyCityDeque(originalList, temp);

    solution.clear();
    solution.push_back(originalList[startIndex]);
    originalList.erase(originalList.begin() + startIndex);
    --currentNum;
    ++citiesAdded;

    while (currentNum != 0) {
        closest = INT_MAX;
        for (int i = 0; i < currentNum; ++i) {
            currentDistance = originalList[i]->distanceTo(solution[citiesAdded - 1]);
            if (currentDistance < closest) {
                closestIndex = i;
                closest = currentDistance;
            }
        }

        totalDistance += closest;
        solution.push_back(originalList[closestIndex]);
        originalList.erase(originalList.begin() + closestIndex);

        --currentNum;
        ++citiesAdded;
    }

    copyCityDeque(temp, originalList);
    return totalDistance + solution[0]->distanceTo(solution[citiesAdded - 1]);
}

int TSP::optimizeTwoChange() {
    if (numCities == 0) return 0;

    std::deque<City*> newPath;
    int minDistance = getSolutionDistance();
    bool startOver = false;

    signal(SIGTERM, endOptimization);

    while (!done) {
        startOver = false;
        for (int i = 1; i < numCities && !startOver; ++i) {
            for (int j = i + 1; j < numCities - 1 && !startOver; ++j) {
                if (solution[i - 1]->distanceTo(solution[j]) + solution[i]->distanceTo(solution[j + 1]) < solution[i - 1]->distanceTo(solution[i]) + solution[j]->distanceTo(solution[j + 1])) {
                    swapCities(i, j);
                    minDistance = getSolutionDistance();
                    startOver = true;
                } else {
                    startOver = false;
                }
            }
        }
        if (!startOver) break;
    }
    return minDistance;
}

int TSP::optimizeTwoOpt() {
    if (numCities == 0) return 0;

    std::deque<City*> newPath;
    int minDistance = getSolutionDistance();
    int k = 0;

    for (int i = 1; i < numCities; ++i) {
        k = 1;
        fixPositions();
        while (k <= 5 && solution[i]->distanceTo(solution[i]->getNeighbor(k)) < solution[i - 1]->distanceTo(solution[i])) {
            swapCities(i, solution[i]->getNeighborPosition(k));
            minDistance = getSolutionDistance();
            ++k;
            std::cout << minDistance << std::endl;
        }
        fixPositions();
    }
    return minDistance;
}

int TSP::swapCities(int i, int k) {
    if (numCities == 0) return 0;

    std::deque<City*> temp;
    int count = 0;

    for (int x = k; x >= i; --x) {
        temp.push_back(solution[x]);
    }

    for (int x = i; x <= k; ++x) {
        solution[x] = temp[count];
        ++count;
    }
    temp.clear();
    return 1;
}

int TSP::getSolutionDistance() const {
    if (numCities == 0) return 0;

    int totalDistance = 0;
    for (int i = 0; i < numCities - 1; ++i) {
        totalDistance += solution[i]->distanceTo(solution[i + 1]);
    }
    totalDistance += solution[0]->distanceTo(solution[numCities - 1]);
    return totalDistance;
}

void TSP::displayNeighborLists() const {
    for (int i = 0; i < numCities; ++i) {
        std::cout << "LIST " << i << std::endl;
        solution[i]->displayNeighborList();
    }
}

void TSP::fixPositions() {
    for (int i = 0; i < numCities; ++i) {
        solution[i]->setPosition(i);
    }
}

void copyCityDeque(std::deque<City*> &source, std::deque<City*> &dest) {
    int length = source.size();
    dest.clear();
    for (int i = 0; i < length; ++i) {
        dest.push_back(new City(*source[i]));
    }
}

void endOptimization([[maybe_unused]] int signum) {
    std::cout << "\nOut of time\n";
    done = 1;
}
