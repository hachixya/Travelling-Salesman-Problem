#include "include/delivery_optimizer.hpp"


// Constructor that initializes the list of delivery locations
DeliveryOptimizer::DeliveryOptimizer(const std::vector<Location>& locations)
    : locations(locations) {}

// Computes the cost matrix for the given locations
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

// Calculates distances between two points
double DeliveryOptimizer::distance(int start, int end) const {
    if (start < 0 || start >= locations.size() || end < 0 || end >= locations.size()) {
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

// Finds the optimal route using a simple heuristic or an exact method
std::vector<int> DeliveryOptimizer::findOptimalRoute() {
    std::vector<int> route;
    std::vector<bool> visited(locations.size(), false);
    int current = 0;
    route.push_back(current);
    visited[current] = true;

    while (route.size() < locations.size()) {
        double nearest = std::numeric_limits<double>::max();
        int nextLocation = -1;

        for (int j = 0; j < locations.size(); j++) {
            if (!visited[j] && costMatrix[current][j] < nearest) {
                nearest = costMatrix[current][j];
                nextLocation = j;
            }
        }

        if (nextLocation == -1) {
            std::cerr << "Failed to find a complete route. Please check data and parameters." << std::endl;
            break;
        }

        route.push_back(nextLocation);
        visited[nextLocation] = true;
        current = nextLocation;
    }

    return route;
}

// Getter for the cost matrix
const std::vector<std::vector<double>>& DeliveryOptimizer::getCostMatrix() const {
    return costMatrix;
}

// Getter for the locations
const std::vector<Location>& DeliveryOptimizer::getLocations() const {
    return locations;
}
