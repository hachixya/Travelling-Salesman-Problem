#pragma once
#include <cmath>
#include <algorithm>
#include <limits>
#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <iomanip>

struct Location {
    double latitude;
    double longitude;
};

class DeliveryOptimizer {
public:
    DeliveryOptimizer(const std::vector<Location>& locations);
    void computeCostMatrix();
    std::vector<int> findOptimalRoute();
    double distance(int start, int end) const;
    const std::vector<std::vector<double>>& getCostMatrix() const;
    const std::vector<Location>& getLocations() const;

private:
    std::vector<Location> locations;       // Stores the delivery locations
    std::vector<std::vector<double>> costMatrix;  // Cost matrix between locations
};
