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
    size_t n = locations.size();  // Use size_t for sizes
    costMatrix.resize(n, std::vector<double>(n, 0.0));
    for (size_t i = 0; i < n; i++) {
        for (size_t j = 0; j < n; j++) {
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
    const double EARTH_RADIUS_KM = 6371.0;
    double lat1 = locations[start].latitude * M_PI / 180.0;
    double lon1 = locations[start].longitude * M_PI / 180.0;
    double lat2 = locations[end].latitude * M_PI / 180.0;
    double lon2 = locations[end].longitude * M_PI / 180.0;

    double dlat = lat2 - lat1;
    double dlon = lon2 - lon1;

    double a = std::sin(dlat / 2) * std::sin(dlat / 2) +
               std::cos(lat1) * std::cos(lat2) * std::sin(dlon / 2) * std::sin(dlon / 2);
    double c = 2 * std::atan2(std::sqrt(a), std::sqrt(1 - a));

    return EARTH_RADIUS_KM * c;
}

// Calculates the cost of a given route
double DeliveryOptimizer::route_cost(const std::vector<int>& route, const std::vector<std::vector<double>>& costMatrix) const {
    double total_cost = 0.0;
    for (size_t i = 0; i < route.size() - 1; ++i) {  // Use size_t for indices
        total_cost += costMatrix[route[i]][route[i + 1]];
    }
    total_cost += costMatrix[route.back()][route.front()]; // Return to the start
    return total_cost;
}

// Genetic Algorithm to determine the optimal route
std::vector<int> DeliveryOptimizer::genetic_algorithm(const std::vector<std::vector<double>>& costMatrix, int population_size, int generations, double mutation_rate, double crossover_rate) {
    std::cout << "Starting genetic_algorithm" << std::endl;

    std::random_device rd;
    std::mt19937 gen(rd());
    size_t n = costMatrix.size();

    // Initialize population
    std::vector<std::vector<int>> population(population_size);
    for (auto& individual : population) {
        individual = random_permutation(n);
    }

    //std::cout << "Initialized population with size: " << population.size() << std::endl;

    // Function to sort population by increasing route cost
    auto by_cost = [this, &costMatrix](const std::vector<int>& a, const std::vector<int>& b) {
        return this->route_cost(a, costMatrix) < this->route_cost(b, costMatrix);
    };

    for (int generation = 0; generation < generations; ++generation) {
        //std::cout << "Generation: " << generation << std::endl;

        // Sort population based on their fitness (route cost)
        std::sort(population.begin(), population.end(), by_cost);

        // Selection - Top half survives
        population.resize(population_size / 2);

        //std::cout << "Population resized to: " << population.size() << std::endl;

        // Crossover - Fill the population back to original size
        size_t old_population_size = population.size();
        while (population.size() < static_cast<size_t>(population_size)) {
            int parent1_idx = std::uniform_int_distribution<>(0, old_population_size - 1)(gen);
            int parent2_idx = std::uniform_int_distribution<>(0, old_population_size - 1)(gen);
            auto& parent1 = population[parent1_idx];
            auto& parent2 = population[parent2_idx];

            // One-point crossover
            if (std::generate_canonical<double, 10>(gen) < crossover_rate) {
                int crossover_point = std::uniform_int_distribution<>(1, n - 1)(gen);
                std::vector<int> child1 = parent1;
                std::vector<int> child2 = parent2;

                // Create child by combining genes from both parents
                std::swap_ranges(child1.begin() + crossover_point, child1.end(), child2.begin() + crossover_point);

                // Correct duplicates
                fix_duplicates(child1);
                fix_duplicates(child2);

                population.push_back(child1);
                if (population.size() < static_cast<size_t>(population_size)) {
                    population.push_back(child2);
                }
            }
        }

        // Mutation
        for (auto& individual : population) {
            if (std::generate_canonical<double, 10>(gen) < mutation_rate) {
                size_t swap_idx1 = std::uniform_int_distribution<>(0, n - 1)(gen);  // Use size_t
                size_t swap_idx2 = std::uniform_int_distribution<>(0, n - 1)(gen);  // Use size_t
                std::swap(individual[swap_idx1], individual[swap_idx2]);
            }
        }

        // Debugging: Print the best route in the current generation
        auto best_route = *std::min_element(population.begin(), population.end(), by_cost);
        // std::cout << "Generation " << generation << " Best route: ";
        // for (int location : best_route) {
        //     std::cout << location << " ";
        // }
        // std::cout << "\nCost: " << route_cost(best_route, costMatrix) << std::endl;
    }

    // Return the best route from the last generation
    auto best_route = *std::min_element(population.begin(), population.end(), by_cost);

    // Debugging: Print the best route and its cost
    std::cout << "Best route: ";
    for (int location : best_route) {
        std::cout << location << " ";
    }
    std::cout << "\nCost: " << route_cost(best_route, costMatrix) << std::endl;

    return best_route;
}

// Simulated Annealing to determine the optimal route
std::vector<int> DeliveryOptimizer::simulated_annealing(const std::vector<std::vector<double>>& costMatrix, double start_temp, double end_temp, double cooling_rate) {
    size_t n = costMatrix.size();
    auto current_route = random_permutation(n);
    auto best_route = current_route;
    double current_cost = route_cost(current_route, costMatrix);
    double best_cost = current_cost;
    double temp = start_temp;

    std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<> dist(0, n - 1);

    while (temp > end_temp) {
        size_t i = dist(rng);  // Use size_t
        size_t j = dist(rng);  // Use size_t
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
std::vector<int> DeliveryOptimizer::random_permutation(size_t n) {  // Use size_t
    std::vector<int> perm(n);
    std::iota(perm.begin(), perm.end(), 0);
    std::shuffle(perm.begin(), perm.end(), std::mt19937{std::random_device{}()});
    return perm;
}

// Fix duplicates in the route generated by crossover
void DeliveryOptimizer::fix_duplicates(std::vector<int>& route) {
    std::unordered_set<int> visited;
    for (size_t i = 0; i < route.size(); ++i) {  // Use size_t
        if (visited.find(route[i]) != visited.end()) {
            // Find a replacement
            for (size_t j = 0; j < route.size(); ++j) {  // Use size_t
                if (visited.find(j) == visited.end()) {
                    route[i] = j;
                    visited.insert(j);
                    break;
                }
            }
        } else {
            visited.insert(route[i]);
        }
    }
}

// Getter for the cost matrix
const std::vector<std::vector<double>>& DeliveryOptimizer::getCostMatrix() const {
    return costMatrix;
}

// Getter for the locations
const std::vector<Location>& DeliveryOptimizer::getLocations() const {
    return locations;
}
