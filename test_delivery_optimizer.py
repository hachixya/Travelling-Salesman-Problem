import pytest
from delivery_optimizer import DeliveryOptimizer, Location
import math

from itertools import permutations

def calculate_exact_cost(locations):
    def distance(loc1, loc2):
        from math import radians, sin, cos, sqrt, atan2
        R = 6371.0
        lat1, lon1 = radians(loc1.latitude), radians(loc1.longitude)
        lat2, lon2 = radians(loc2.latitude), radians(loc2.longitude)
        dlat = lat2 - lat1
        dlon = lon2 - lon1
        a = sin(dlat / 2) * sin(dlat / 2) + cos(lat1) * cos(lat2) * sin(dlon / 2) * sin(dlon / 2)
        c = 2 * atan2(sqrt(a), sqrt(1 - a))
        return R * c
    
    n = len(locations)
    min_cost = float('inf')
    best_route = None
    for perm in permutations(range(n)):
        cost = sum(distance(locations[perm[i]], locations[perm[i+1]]) for i in range(n-1))
        cost += distance(locations[perm[-1]], locations[perm[0]])  # Return to start
        if cost < min_cost:
            min_cost = cost
            best_route = perm
    return min_cost, best_route

locations = [
    Location(37.7749, -122.4194),  # San Francisco
    Location(35.6895, 139.6917),   # Tokyo
    Location(37.5665, 126.9780),   # Seoul
    Location(-33.8688, 151.2093),  # Sydney
    Location(34.0522, -118.2437)   # Los Angeles
]

exact_cost, best_route = calculate_exact_cost(locations)
print(f"Exact cost: {exact_cost}")
print(f"Best route: {best_route}")



def test_compute_cost_matrix():
    locations = [
        Location(37.7749, -122.4194),  # San Francisco
        Location(35.6895, 139.6917),   # Tokyo
        Location(37.5665, 126.9780),   # Seoul
        Location(-33.8688, 151.2093),  # Sydney
        Location(34.0522, -118.2437)   # Los Angeles
    ]
    optimizer = DeliveryOptimizer(locations)
    cost_matrix = optimizer.calculate_cost_matrix()
    
    assert len(cost_matrix) == len(locations)
    for row in cost_matrix:
        assert len(row) == len(locations)

def test_distance():
    locations = [
        Location(37.7749, -122.4194),  # San Francisco
        Location(35.6895, 139.6917)    # Tokyo
    ]
    optimizer = DeliveryOptimizer(locations)
    dist = optimizer.distance(0, 1)
    
    # Reference distance in kilometers between San Francisco and Tokyo
    reference_distance = 8280  # Approximate distance in km
    tolerance = 50  # Tolerance in km

    assert math.isclose(dist, reference_distance, abs_tol=tolerance)

def test_genetic_algorithm():
    locations = [
        Location(37.7749, -122.4194),  # San Francisco
        Location(35.6895, 139.6917),   # Tokyo
        Location(37.5665, 126.9780),   # Seoul
        Location(-33.8688, 151.2093),  # Sydney
        Location(34.0522, -118.2437)   # Los Angeles
    ]
    optimizer = DeliveryOptimizer(locations)
    cost_matrix = optimizer.calculate_cost_matrix()
    route = optimizer.genetic_algorithm(cost_matrix, 50, 1000, 0.05, 0.7)
    
    # Check if the route includes all locations exactly once
    assert len(route) == len(locations)
    assert set(route) == set(range(len(locations)))
    
    # Calculate the total cost of the route
    total_cost = optimizer.route_cost(route, cost_matrix)
    # Reference cost for the given locations (using a known optimal or near-optimal solution)
    reference_cost = exact_cost
    tolerance = 500  # Tolerance in km

    assert total_cost <= reference_cost + tolerance

def test_simulated_annealing():
    locations = [
        Location(37.7749, -122.4194),  # San Francisco
        Location(35.6895, 139.6917),   # Tokyo
        Location(37.5665, 126.9780),   # Seoul
        Location(-33.8688, 151.2093),  # Sydney
        Location(34.0522, -118.2437)   # Los Angeles
    ]
    optimizer = DeliveryOptimizer(locations)
    cost_matrix = optimizer.calculate_cost_matrix()
    route = optimizer.simulated_annealing(cost_matrix, 10000.0, 1.0, 0.995)

    # Check if the route includes all locations exactly once
    assert len(route) == len(locations)
    assert set(route) == set(range(len(locations)))

    # Calculate the total cost of the route
    total_cost = optimizer.route_cost(route, cost_matrix)
    # Use the calculated exact cost as the reference
    reference_cost = exact_cost 
    tolerance = 500  # Tolerance in km

    assert total_cost <= reference_cost + tolerance


def test_large_dataset():
    # Generate a large dataset of random locations
    import random
    random.seed(42)
    locations = [Location(random.uniform(-90, 90), random.uniform(-180, 180)) for _ in range(100)]
    optimizer = DeliveryOptimizer(locations)
    cost_matrix = optimizer.calculate_cost_matrix()
    
    # Test genetic algorithm on large dataset
    route_ga = optimizer.genetic_algorithm(cost_matrix, 100, 2000, 0.1, 0.8)
    assert len(route_ga) == len(locations)
    assert set(route_ga) == set(range(len(locations)))
    
    # Test simulated annealing on large dataset
    route_sa = optimizer.simulated_annealing(cost_matrix, 50000.0, 1.0, 0.999)
    assert len(route_sa) == len(locations)
    assert set(route_sa) == set(range(len(locations)))

def test_edge_cases():
    # Test with no locations
    optimizer = DeliveryOptimizer([])
    cost_matrix = optimizer.calculate_cost_matrix()
    assert cost_matrix == []
    
    # Test with a single location
    optimizer = DeliveryOptimizer([Location(0, 0)])
    cost_matrix = optimizer.calculate_cost_matrix()
    assert cost_matrix == [[0.0]]

def test_route_integrity():
    locations = [
        Location(37.7749, -122.4194),  # San Francisco
        Location(35.6895, 139.6917),   # Tokyo
        Location(37.5665, 126.9780),   # Seoul
        Location(-33.8688, 151.2093),  # Sydney
        Location(34.0522, -118.2437)   # Los Angeles
    ]
    optimizer = DeliveryOptimizer(locations)
    cost_matrix = optimizer.calculate_cost_matrix()
    
    # Check genetic algorithm route integrity
    route_ga = optimizer.genetic_algorithm(cost_matrix, 50, 1000, 0.05, 0.7)
    assert len(route_ga) == len(locations)
    assert set(route_ga) == set(range(len(locations)))
    
    # Check simulated annealing route integrity
    route_sa = optimizer.simulated_annealing(cost_matrix, 10000.0, 1.0, 0.995)
    assert len(route_sa) == len(locations)
    assert set(route_sa) == set(range(len(locations)))
