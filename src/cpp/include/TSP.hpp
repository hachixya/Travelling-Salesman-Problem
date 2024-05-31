#pragma once

#include <deque>
#include <vector>
#include "City.hpp"
#include <functional>

class TSP {
public:
    TSP(const char* filename);
    TSP(const TSP& source);
    ~TSP();
    int readFile(const char* filename);
    void writeSolution(const char* fileName);
    int solveBruteForce();
    int solveNearestNeighbor();
    int solveGeneticAlgorithm(int populationSize, int generations, double crossoverRate, double mutationRate);
    int solveNearestNeighborBasic(int startIndex);
    int optimizeTwoChange();
    int optimizeTwoOpt(int bestDistance);
    int swapCities(int i, int k);
    int getSolutionDistance() const;
    void displayNeighborLists() const;
    void fixPositions();
    int getNumberOfCities() const;
    City* getCity(int index) const;

private:
    void bruteForce(std::deque<City*>& bestPath, int& minDistance, int citiesLeft);
    void rotateSolution(int pos);
    void generateInitialPopulation(std::vector<std::deque<City*>>& population, int populationSize);
    std::deque<City*> selectParent(const std::vector<std::deque<City*>>& population);
    std::deque<City*> crossover(const std::deque<City*>& parent1, const std::deque<City*>& parent2);
    void mutate(std::deque<City*>& individual);
    int calculateFitness(const std::deque<City*>& individual);

    std::deque<City*> originalList;
    std::deque<City*> solution;
    int numCities;
};

void copyCityDeque(std::deque<City*>& source, std::deque<City*>& dest);
void endOptimization([[maybe_unused]] int signum);
void setVisualizationCallback(std::function<void(int, TSP&)> callback);
