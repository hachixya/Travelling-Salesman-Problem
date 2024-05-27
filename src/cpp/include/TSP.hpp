#pragma once

#include <deque>
#include <fstream>
#include <vector>
#include <signal.h>
#include "City.hpp"

const char OUTPUT_FN[] = "mysolution.txt";

class TSP {
public:
    TSP(const char *filename);
    TSP(const TSP &source);
    ~TSP();
    int readFile(const char *filename);
    void writeSolution(const char *fileName);
    int solveBruteForce();
    int solveNearestNeighbor();
    int solveNearestNeighborBasic(int startIndex);
    int optimizeTwoChange();
    int optimizeTwoOpt();
    int swapCities(int i, int k);
    int getSolutionDistance() const;
    void displayNeighborLists() const;
    void fixPositions();
    void rotateSolution(int pos);

private:
    void bruteForce(std::deque<City*> &bestPath, int &minDistance, int citiesLeft);
    std::deque<City*> originalList;
    std::deque<City*> solution;
    int numCities;
};

void copyCityDeque(std::deque<City*> &source, std::deque<City*> &dest);
void endOptimization(int signum);
