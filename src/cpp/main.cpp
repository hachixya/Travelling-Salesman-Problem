#include "include/delivery_optimizer.hpp"

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

        // Compute the cost matrix
        optimizer.computeCostMatrix();

        // Retrieve and display the cost matrix
        auto costMatrix = optimizer.getCostMatrix();
        std::cout << "Cost Matrix:" << std::endl;
        for (const auto& row : costMatrix) {
            for (double cost : row) {
                std::cout << std::setw(10) << std::fixed << std::setprecision(2) << cost << " ";
            }
            std::cout << std::endl;
        }

        // Find and display the optimal route
        auto route = optimizer.findOptimalRoute();
        std::cout << "Optimal Route: ";
        for (int location : route) {
            std::cout << location << " ";
        }
        std::cout << "Home" << std::endl; // Assuming the route ends where it started

    } catch (const std::exception& e) {
        // Handle any exceptions that might be thrown during the optimization process
        std::cerr << "An error occurred: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
