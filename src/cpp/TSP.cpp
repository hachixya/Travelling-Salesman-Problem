#include "TSP.hpp"
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <csignal>
#include <climits>
#include <chrono>
#include <algorithm>
#include <random>

#ifdef BUILD_PYBIND_MODULE
#include <pybind11/pybind11.h>
#endif
#include <unordered_set>

int done = 0;

std::function<void(int, TSP&)> visualizationCallback = nullptr;

void setVisualizationCallback(std::function<void(int, TSP&)> callback) {
    visualizationCallback = callback;
}

TSP::TSP(const char* filename) {
    numCities = readFile(filename);
}

TSP::TSP(const TSP& source) {
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

int TSP::readFile(const char* filename) {
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

void TSP::writeSolution(const char* fileName) {
    int distance = getSolutionDistance();

    std::cout << "Writing solution to " << fileName << std::endl;
    std::cout << "Distance: " << distance << std::endl;
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

void TSP::bruteForce(std::deque<City*>& bestPath, int& minDistance, int citiesLeft) {
    int currentDistance = 0;
    signal(SIGTERM, endOptimization);

    for (int i = 0; !done && i < citiesLeft; ++i) {
        rotateSolution(citiesLeft - 1);
        currentDistance = getSolutionDistance();
        if (currentDistance < minDistance) {
            minDistance = currentDistance;
            std::cout << minDistance << std::endl;
            copyCityDeque(solution, bestPath);
            #ifdef BUILD_PYBIND_MODULE
            if (visualizationCallback) {
                std::cout << "Visualization callback bruteforce" << std::endl;
                visualizationCallback(currentDistance, *this);
            }
            #endif
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
    int bestStartDistance = INT_MAX;
    int lastRun = 0;

    for (int i = 0; !done && i < numCities; ++i) {
        solveNearestNeighborBasic(i);
        lastRun = optimizeTwoChange();
        if (lastRun < bestStartDistance) {
            bestStartDistance = lastRun;
            std::cout << "Writing solution " << bestStartDistance << std::endl;
            writeSolution("nearest_neighbor_solution.txt");

            // Only update visualization when running as a Python module
            #ifdef BUILD_PYBIND_MODULE
            if (visualizationCallback) {
                std::cout << "Visualization callback neighbor" << std::endl;
                visualizationCallback(bestStartDistance, *this);
            }
            #endif
            bestStartDistance = optimizeTwoOpt(bestStartDistance);
        }
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

int TSP::optimizeTwoOpt(int currentBestDistance) {
    if (numCities == 0) return 0;

    int bestDistance = currentBestDistance;
    bool improvement = true;
    std::cout << "Initial distance: " << bestDistance << std::endl;

    while (improvement) {
        improvement = false;
        for (int i = 1; i < numCities - 1; ++i) {
            for (int j = i + 1; j < numCities; ++j) {
                if (j - i == 1) continue;  // Skip adjacent edges

                int newDistance = bestDistance - solution[i - 1]->distanceTo(solution[i])
                                  - solution[j]->distanceTo(solution[(j + 1) % numCities])
                                  + solution[i - 1]->distanceTo(solution[j])
                                  + solution[i]->distanceTo(solution[(j + 1) % numCities]);

                if (newDistance < bestDistance) {
                    std::reverse(solution.begin() + i, solution.begin() + j + 1);
                    bestDistance = newDistance;
                    improvement = true;

                    // Debug output to see improvement steps
                    if (improvement) {
                        std::cout << "Improved distance: " << bestDistance << std::endl;
                        writeSolution("2opt_improved_solution.txt");
                        #ifdef BUILD_PYBIND_MODULE
                        if (visualizationCallback) {
                            std::cout << "Visualization callback two opt" << std::endl;
                            visualizationCallback(bestDistance, *this);
                        }
                        #endif
                    }
                }
            }
        }
    }

    return bestDistance;
}

int TSP::solveGeneticAlgorithm(int populationSize, int generations, double crossoverRate, double mutationRate) {
    if (numCities == 0) return 0;

    auto start = std::chrono::high_resolution_clock::now();
    std::vector<std::deque<City*>> population;
    generateInitialPopulation(population, populationSize);

    int bestDistance = INT_MAX;
    std::deque<City*> bestSolution;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, 1.0);

    for (int generation = 0; generation < generations; ++generation) {
        std::vector<std::deque<City*>> newPopulation;

        for (int i = 0; i < populationSize / 2; ++i) {
            std::deque<City*> parent1 = selectParent(population);
            std::deque<City*> parent2 = selectParent(population);

            std::deque<City*> offspring1 = parent1;
            std::deque<City*> offspring2 = parent2;

            if (dis(gen) < crossoverRate) {
                offspring1 = crossover(parent1, parent2);
                offspring2 = crossover(parent2, parent1);
            }

            if (dis(gen) < mutationRate) {
                mutate(offspring1);
            }

            if (dis(gen) < mutationRate) {
                mutate(offspring2);
            }

            newPopulation.push_back(offspring1);
            newPopulation.push_back(offspring2);
        }

        population = newPopulation;

        for (const auto& individual : population) {
            int distance = calculateFitness(individual);
            if (distance < bestDistance) {
                bestDistance = distance;
                bestSolution = individual;

                std::cout << "New best distance: " << bestDistance << std::endl;
                solution = bestSolution;
                writeSolution("genetic_algorithm_solution.txt");

                #ifdef BUILD_PYBIND_MODULE
                if (visualizationCallback) {
                    visualizationCallback(bestDistance, *this);
                }
                #endif
            }
        }
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;

    std::cout << "Best path distance: " << bestDistance << std::endl;
    std::cout << "Best path: ";
    for (auto city : bestSolution) {
        std::cout << city->getId() << " ";
    }
    std::cout << std::endl;
    std::cout << "Execution time: " << duration.count() << " seconds" << std::endl;

    return bestDistance;
}

void TSP::generateInitialPopulation(std::vector<std::deque<City*>>& population, int populationSize) {
    std::random_device rd;
    std::mt19937 gen(rd());

    for (int i = 0; i < populationSize; ++i) {
        std::deque<City*> individual = originalList;
        std::shuffle(individual.begin(), individual.end(), gen);
        population.push_back(individual);
    }
}


std::deque<City*> TSP::selectParent(const std::vector<std::deque<City*>>& population) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, population.size() - 1);

    std::deque<City*> best = population[dis(gen)];
    for (int i = 0; i < 2; ++i) {
        std::deque<City*> candidate = population[dis(gen)];
        if (calculateFitness(candidate) < calculateFitness(best)) {
            best = candidate;
        }
    }
    return best;
}

std::deque<City*> TSP::crossover(const std::deque<City*>& parent1, const std::deque<City*>& parent2) {
    std::deque<City*> child;
    std::unordered_set<int> visited;

    // Randomly select a subset from parent1 to initialize the child
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, parent1.size() - 1);

    int start = dis(gen);
    int end = dis(gen);
    if (start > end) std::swap(start, end);

    std::deque<City*>::size_type start_idx = static_cast<std::deque<City*>::size_type>(start);
    std::deque<City*>::size_type end_idx = static_cast<std::deque<City*>::size_type>(end);

    for (std::deque<City*>::size_type i = start_idx; i <= end_idx; ++i) {
        child.push_back(parent1[i]);
        visited.insert(parent1[i]->getId());
    }

    // Fill the remaining positions with the cities from parent2
    for (std::deque<City*>::size_type i = 0; i < parent2.size(); ++i) {
        if (visited.find(parent2[i]->getId()) == visited.end()) {
            child.push_back(parent2[i]);
        }
    }

    return child;
}



void TSP::mutate(std::deque<City*>& individual) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, individual.size() - 1);

    int index1 = dis(gen);
    int index2 = dis(gen);
    std::swap(individual[index1], individual[index2]);
}


int TSP::calculateFitness(const std::deque<City*>& individual) {
    int totalDistance = 0;
    for (size_t i = 0; i < individual.size() - 1; ++i) {
        totalDistance += individual[i]->distanceTo(individual[i + 1]);
    }
    totalDistance += individual.back()->distanceTo(individual.front());
    return totalDistance;
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

int TSP::getNumberOfCities() const {
    return numCities;
}

City* TSP::getCity(int index) const {
    if (index >= 0 && index < numCities) {
        return solution[index];
    }
    return nullptr;
}

void copyCityDeque(std::deque<City*>& source, std::deque<City*>& dest) {
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
