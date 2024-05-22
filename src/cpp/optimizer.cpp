#include "include/delivery_optimizer.hpp"

// Constructor that initializes the list of delivery locations
DeliveryOptimizer::DeliveryOptimizer(const std::vector<Location>& locations)
    : locations(locations) {
    computeCostMatrix();  // Optionally compute the matrix immediately upon initialization
}

// Loads new delivery locations and recomputes the cost matrix
void DeliveryOptimizer::load_delivery_locations(const std::vector<Location>& new_locations) {
    locations = new_locations;
    computeCostMatrix();
}

// Computes the cost matrix for the current set of locations
void DeliveryOptimizer::computeCostMatrix() {
    int n = locations.size();
    costMatrix.resize(n, std::vector<double>(n, 0.0));
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (i != j) {
                costMatrix[i][j] = distance(i, j);
            } else {
                costMatrix[i][j] = 0;  // No cost to travel to the same location
            }
        }
    }
}

// Returns the cost matrix
std::vector<std::vector<double>> DeliveryOptimizer::calculate_cost_matrix() {
    return costMatrix;
}

// Calculates the geographical distance between two locations using their indices
double DeliveryOptimizer::distance(int start, int end) const {
    if (start < 0 || start >= static_cast<int>(locations.size()) || end < 0 || end >= static_cast<int>(locations.size())) {
        throw std::out_of_range("Index out of range");
    }

    const double earthRadiusKm = 6371.0;
    double lat1 = locations[start].latitude * M_PI / 180.0;
    double lon1 = locations[start].longitude * M_PI / 180.0;
    double lat2 = locations[end].latitude * M_PI / 180.0;
    double lon2 = locations[end].longitude * M_PI / 180.0;

    double dLat = lat2 - lat1;
    double dLon = lon2 - lon1;

    double a = std::sin(dLat / 2) * std::sin(dLat / 2) +
               std::cos(lat1) * std::cos(lat2) *
               std::sin(dLon / 2) * std::sin(dLon / 2);
    double c = 2 * std::atan2(std::sqrt(a), std::sqrt(1 - a));

    return earthRadiusKm * c;
}

// Fitness function to calculate the route cost
double DeliveryOptimizer::route_cost(const std::vector<int>& route, const std::vector<std::vector<double>>& costMatrix) const {
    double cost = 0.0;
    for (size_t i = 0; i < route.size() - 1; i++) {
        cost += costMatrix[route[i]][route[i+1]];
    }
    cost += costMatrix[route.back()][route.front()]; // Return to start point
    return cost;
}

// Genetic Algorithm for finding the optimal route
std::vector<int> DeliveryOptimizer::genetic_algorithm(const std::vector<std::vector<double>>& costMatrix, int population_size, int generations, double mutation_rate, double crossover_rate) {
    std::random_device rd;
    std::mt19937 gen(rd());
    int n = costMatrix.size();
    
    // Initialize population
    std::vector<std::vector<int>> population(population_size);
    for (auto& individual : population) {
        individual = random_permutation(n);
    }

    // Function to sort population by increasing route cost
    auto by_cost = [this, &costMatrix](const std::vector<int>& a, const std::vector<int>& b) {
        return this->route_cost(a, costMatrix) < this->route_cost(b, costMatrix);
    };
    for (int generation = 0; generation < generations; ++generation) {
        // Sort population based on their fitness (route cost)
        std::sort(population.begin(), population.end(), by_cost);

        // Selection - Top half survives
        population.resize(population_size / 2);

        // Crossover - Fill the population back to original size
        while (population.size() < static_cast<size_t>(population_size)) {
            int parent1_idx = std::uniform_int_distribution<>(0, population.size() - 1)(gen);
            int parent2_idx = std::uniform_int_distribution<>(0, population.size() - 1)(gen);
            auto& parent1 = population[parent1_idx];
            auto& parent2 = population[parent2_idx];

            // One-point crossover
            if (std::generate_canonical<double, 10>(gen) < crossover_rate) {
                int crossover_point = std::uniform_int_distribution<>(1, n - 1)(gen);
                std::vector<int> child1 = parent1;
                std::vector<int> child2 = parent2;

                // Create child by combining genes from both parents
                std::swap_ranges(child1.begin() + crossover_point, child1.end(), child2.begin() + crossover_point);

                population.push_back(child1);
                if (population.size() < static_cast<size_t>(population_size)) {
                    population.push_back(child2);
                }
            }
        }

        // Mutation
        for (auto& individual : population) {
            if (std::generate_canonical<double, 10>(gen) < mutation_rate) {
                int swap_idx1 = std::uniform_int_distribution<>(0, n - 1)(gen);
                int swap_idx2 = std::uniform_int_distribution<>(0, n - 1)(gen);
                std::swap(individual[swap_idx1], individual[swap_idx2]);
            }
        }
    }

    // Return the best route from the last generation
    return *std::min_element(population.begin(), population.end(), by_cost);
}

// Simulated Annealing to determine the optimal route
std::vector<int> DeliveryOptimizer::simulated_annealing(const std::vector<std::vector<double>>& costMatrix, double start_temp, double end_temp, double cooling_rate) {
    int n = costMatrix.size();
    auto current_route = random_permutation(n);
    auto best_route = current_route;
    double current_cost = route_cost(current_route, costMatrix);
    double best_cost = current_cost;
    double temp = start_temp;

    std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<> dist(0, n - 1);

    while (temp > end_temp) {
        int i = dist(rng), j = dist(rng);
        std::swap(current_route[i], current_route[j]);
        double new_cost = route_cost(current_route, costMatrix);
        if (new_cost < current_cost || exp((current_cost - new_cost) / temp) > std::generate_canonical<double, 10>(rng)) {
            current_cost = new_cost;
            if (current_cost < best_cost) {
                best_route = current_route;
                best_cost = current_cost;
            }
        } else {
            // Undo the swap
            std::swap(current_route[i], current_route[j]);
        }
        temp *= cooling_rate;
    }
    return best_route;
}

// Utility functions needed for genetic algorithm and simulated annealing
std::vector<int> DeliveryOptimizer::random_permutation(int n) {
    std::vector<int> perm(n);
    std::iota(perm.begin(), perm.end(), 0);
    std::shuffle(perm.begin(), perm.end(), std::mt19937{std::random_device{}()});
    return perm;
}

// Getter for the cost matrix
const std::vector<std::vector<double>>& DeliveryOptimizer::getCostMatrix() const {
    return costMatrix;
}

// Getter for the locations
const std::vector<Location>& DeliveryOptimizer::getLocations() const {
    return locations;
}
