#include "City.hpp"
#include "TSP.hpp"
#include <iostream>
#include <cstdlib>
#include <ctime>

int main(int argc, char *argv[]) {
    const char *defaultFile = "att48.tsp";
    const char *filename;

    if (argc < 2) {
        std::cerr << "No filename provided. Defaulting to " << defaultFile << std::endl;
        filename = defaultFile;
    } else {
        filename = argv[1];
    }

    std::srand(std::time(0));

    TSP tsp(filename);
    tsp.solveNearestNeighbor();

    return 0;
}
