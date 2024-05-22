#include "include/delivery_optimizer.hpp"
#include <iostream>
#include <iomanip> // For formatted output

int main() {
    try {
        // Example set of locations (should ideally be fetched from data or user input)
        std::vector<Location> locations = {
            {37.7749, -122.4194},  // San Francisco
            {35.6895, 139.6917},   // Tokyo
            {37.5665, 126.9780},   // Seoul
            {-33.8688, 151.2093},  // Sydney
            {34.0522, -118.2437},  // Los Angeles
            {41.8781, -87.6298},   // Chicago
            {40.7128, -74.0060},   // New York
            {51.5074, -0.1278},    // London
            {48.8566, 2.3522},     // Paris
            {55.7558, 37.6176}     // Moscow
        };

        // Create an instance of DeliveryOptimizer
        DeliveryOptimizer optimizer(locations);

        // Calculate and retrieve the cost matrix
        auto costMatrix = optimizer.calculate_cost_matrix();
        std::cout << "Cost Matrix:" << std::endl;
        for (const auto& row : costMatrix) {
            for (double cost : row) {
                std::cout << std::setw(10) << std::fixed << std::setprecision(2) << cost << " ";
            }
            std::cout << std::endl;
        }

        // Configuration parameters for genetic algorithm
        int population_size = 50;
        int generations = 1000;
        double mutation_rate = 0.05;
        double crossover_rate = 0.7;

        // Running the genetic algorithm
        auto ga_route = optimizer.genetic_algorithm(costMatrix, population_size, generations, mutation_rate, crossover_rate);
        std::cout << "Optimal Route by Genetic Algorithm: ";
        for (int location : ga_route) {
            std::cout << location << " -> ";
        }
        std::cout << "End (GA)" << std::endl;

        // Configuration parameters for simulated annealing
        double start_temp = 10000.0;
        double end_temp = 1.0;
        double cooling_rate = 0.995;

        // Running the simulated annealing
        auto sa_route = optimizer.simulated_annealing(costMatrix, start_temp, end_temp, cooling_rate);
        std::cout << "Optimal Route by Simulated Annealing: ";
        for (int location : sa_route) {
            std::cout << location << " -> ";
        }
        std::cout << "End (SA)" << std::endl;

    } catch (const std::exception& e) {
        // Handle any exceptions that might be thrown during the optimization process
        std::cerr << "An error occurred: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}