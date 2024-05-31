#pragma once

#include <deque>
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

    std::deque<City*> originalList;
    std::deque<City*> solution;
    int numCities;
};

void copyCityDeque(std::deque<City*>& source, std::deque<City*>& dest);
void endOptimization([[maybe_unused]] int signum);
void setVisualizationCallback(std::function<void(int, TSP&)> callback);
