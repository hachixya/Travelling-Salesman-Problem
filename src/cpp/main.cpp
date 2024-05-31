#include "City.hpp"
#include "TSP.hpp"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <string>

int main(int argc, char *argv[]) {
    const char *defaultFile = "att48.tsp";
    const char *filename;

    if (argc < 2) {
        std::cerr << "No filename provided. Defaulting to " << defaultFile << std::endl;
        std::cerr << "Usage: " << argv[0] << " [filename]" << std::endl;
        filename = defaultFile;
    } else {
        filename = argv[1];
    }

    std::string filepath = std::string("data/") + filename;

    std::srand(std::time(0));

    TSP tsp(filepath.c_str());
    tsp.solveNearestNeighbor();

    return 0;
}
