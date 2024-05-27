#pragma once

#include <deque>
#include "City.hpp"

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
    int optimizeTwoOpt();
    int swapCities(int i, int k);
    int getSolutionDistance() const;
    void displayNeighborLists() const;
    void fixPositions();
    int getNumberOfCities() const; // New method to get the number of cities
    City* getCity(int index) const; // New method to get a specific city

private:
    void bruteForce(std::deque<City*>& bestPath, int& minDistance, int citiesLeft);
    void rotateSolution(int pos);

    std::deque<City*> originalList;
    std::deque<City*> solution;
    int numCities;
};

void copyCityDeque(std::deque<City*>& source, std::deque<City*>& dest);
void endOptimization([[maybe_unused]] int signum);
