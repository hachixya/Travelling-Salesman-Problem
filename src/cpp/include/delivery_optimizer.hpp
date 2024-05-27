#pragma once

#include <random>
#include <numeric>
#include <vector>
#include <string>
#include <set> 
#include <unordered_set>
#include <stdexcept>  // For std::out_of_range exception
#include <cmath>      // For mathematical operations, e.g., sin, cos
#include <algorithm>  // For std algorithms
#include <limits>     // For std::numeric_limits
#include <iostream>   // For input/output operations
#include <iomanip>    // For formatted output

// Structure to store geographical coordinates of locations
struct Location {
    double latitude;
    double longitude;
};

// Class to optimize delivery routes using geographical coordinates
class DeliveryOptimizer {
public:
    // Constructor initializes the optimizer with a list of locations
    DeliveryOptimizer(const std::vector<Location>& locations);

    // Load new delivery locations into the optimizer and compute the cost matrix
    void load_delivery_locations(const std::vector<Location>& new_locations);

    // Compute the cost matrix based on current locations
    void computeCostMatrix();

    // Returns the cost matrix for access by other components or for debugging
    std::vector<std::vector<double>> calculate_cost_matrix();

    // Calculate the geographical distance between two locations
    double distance(int start, int end) const;

    double route_cost(const std::vector<int>& route, const std::vector<std::vector<double>>& costMatrix) const;  // Calculates the cost of a given route

    // Genetic Algorithm for route optimization
    std::vector<int> genetic_algorithm(const std::vector<std::vector<double>>& costMatrix, int population_size, int generations, double mutation_rate, double crossover_rate);

    // Simulated Annealing for route optimization
    std::vector<int> simulated_annealing(const std::vector<std::vector<double>>& costMatrix, double start_temp, double end_temp, double cooling_rate);

    // Determine the optimal route using a given cost matrix, returns the route as a sequence of location indices
    std::vector<int> determine_optimal_route(const std::vector<std::vector<double>>& custom_cost_matrix);

    // Getter to retrieve the current cost matrix
    const std::vector<std::vector<double>>& getCostMatrix() const;

    // Getter to retrieve the list of locations
    const std::vector<Location>& getLocations() const;

private:
    std::vector<Location> locations;       // Stores the delivery locations
    std::vector<std::vector<double>> costMatrix;  // Stores the cost matrix between locations

    // Utility functions
    std::vector<int> random_permutation(size_t n);  // Generates a random permutation of indices
    
    // New utility function to fix duplicates in the route
    void fix_duplicates(std::vector<int>& route);
};
