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
    
    int choice;
    std::cout << "Choose algorithm to solve TSP:\n1. Nearest Neighbor\n2. Genetic Algorithm\n3. Brute Force\n";
    std::cin >> choice;

    if (choice == 1) {
        tsp.solveNearestNeighbor();
    } else if (choice == 2) {
        int populationSize;
        int generations;
        double crossoverRate;
        double mutationRate;

        std::cout << "Enter population size: ";
        std::cin >> populationSize;

        std::cout << "Enter number of generations: ";
        std::cin >> generations;

        std::cout << "Enter crossover rate (0 to 1): ";
        std::cin >> crossoverRate;

        std::cout << "Enter mutation rate (0 to 1): ";
        std::cin >> mutationRate;

        tsp.solveGeneticAlgorithm(populationSize, generations, crossoverRate, mutationRate);
    } else if (choice == 3) {
        tsp.solveBruteForce();
    } else {
        std::cerr << "Invalid choice" << std::endl;
    }

    return 0;
}
