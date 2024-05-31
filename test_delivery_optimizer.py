import os
import delivery_optimizer
import pytest
from utils import verify_solution, read_tsp_file, read_solution_file

# Known optimal distances for the datasets
known_optimal = {
    "att48": 33523.71,
    "kroD100": 21294.29,
    "a280": 2586.77,
    "tiny": 4.0  # Sum of all edges in a 2x2 grid
}

# Maximum acceptable distance (5% gap)
def max_acceptable_distance(optimal):
    return optimal * 1.05

# Tolerance for floating point comparisons
tolerance = 1e-2

@pytest.mark.parametrize("dataset,optimal", [
    ("att48.tsp", known_optimal["att48"]),
    ("kroD100.tsp", known_optimal["kroD100"]),
    ("a280.tsp", known_optimal["a280"]),
    ("tiny.tsp", known_optimal["tiny"])
])
def test_solve_nearest_neighbor(dataset, optimal):
    delivery_optimizer.setVisualizationCallback(None)
    dataset_path = os.path.join("data", dataset)
    tsp = delivery_optimizer.TSP(dataset_path)
    distance = tsp.solveNearestNeighbor()
    assert distance <= max_acceptable_distance(optimal), f"Distance {distance} exceeds acceptable limit {max_acceptable_distance(optimal)} for dataset {dataset}"
    tsp.writeSolution("test_solution.txt")
    if dataset != "tiny.tsp":
        assert verify_solution(dataset_path, "test_solution.txt"), "Calculated solution does not match expected distance"

@pytest.mark.parametrize("dataset,optimal", [
    ("att48.tsp", known_optimal["att48"]),
    ("kroD100.tsp", known_optimal["kroD100"]),
    ("a280.tsp", known_optimal["a280"]),
    ("tiny.tsp", known_optimal["tiny"])
])
def test_solve_genetic_algorithm(dataset, optimal):
    delivery_optimizer.setVisualizationCallback(None)
    dataset_path = os.path.join("data", dataset)
    tsp = delivery_optimizer.TSP(dataset_path)
    population_size = 300
    generations = 9000
    crossover_rate = 0.7
    mutation_rate = 0.07
    distance = tsp.solveGeneticAlgorithm(population_size, generations, crossover_rate, mutation_rate)
    if dataset == "kroD100.tsp":
        assert distance <= (max_acceptable_distance(optimal)+1000), f"Distance {distance} exceeds acceptable limit {max_acceptable_distance(optimal)} for dataset {dataset}" 
    tsp.writeSolution("test_solution.txt")
    if dataset != "tiny.tsp":
        assert verify_solution(dataset_path, "test_solution.txt"), "Calculated solution does not match expected distance"

@pytest.mark.parametrize("dataset", ["att48.tsp", "kroD100.tsp", "a280.tsp", "tiny.tsp"])
def test_get_solution_distance(dataset):
    delivery_optimizer.setVisualizationCallback(None)
    dataset_path = os.path.join("data", dataset)
    tsp = delivery_optimizer.TSP(dataset_path)
    tsp.solveNearestNeighbor()
    distance = tsp.getSolutionDistance()
    assert distance > 0, "Solution distance should be greater than 0"

@pytest.mark.parametrize("dataset", ["att48.tsp", "kroD100.tsp", "a280.tsp", "tiny.tsp"])
def test_write_solution(dataset):
    delivery_optimizer.setVisualizationCallback(None)
    dataset_path = os.path.join("data", dataset)
    tsp = delivery_optimizer.TSP(dataset_path)
    tsp.solveNearestNeighbor()
    tsp.writeSolution("test_solution.txt")
    with open("test_solution.txt", "r") as file:
        first_line = file.readline().strip()
        assert first_line.isdigit(), "First line of the solution file should be a number representing the total distance"
    if dataset != "tiny.tsp":
        assert verify_solution(dataset_path, "test_solution.txt"), "Calculated solution does not match expected distance"

def test_city_class():
    city = delivery_optimizer.City(1, 10, 20, 0, False)
    assert city.getId() == 1
    assert city.getX() == 10
    assert city.getY() == 20
    assert city.getPosition() == 0

@pytest.mark.parametrize("dataset", ["att48.tsp", "kroD100.tsp", "a280.tsp", "tiny.tsp"])
def test_multiple_runs(dataset):
    delivery_optimizer.setVisualizationCallback(None)
    dataset_path = os.path.join("data", dataset)
    tsp = delivery_optimizer.TSP(dataset_path)
    distances = []
    for _ in range(5):
        distance = tsp.solveNearestNeighbor()
        distances.append(distance)
    assert all(dist <= max_acceptable_distance(known_optimal[dataset.split('.')[0]]) for dist in distances), \
        f"One of the distances {distances} exceeds the acceptable limit for dataset {dataset}"

@pytest.mark.parametrize("dataset", ["att48.tsp", "kroD100.tsp", "a280.tsp", "tiny.tsp"])
def test_solve_nearest_neighbor_consistency(dataset):
    delivery_optimizer.setVisualizationCallback(None)
    dataset_path = os.path.join("data", dataset)
    tsp = delivery_optimizer.TSP(dataset_path)
    distance1 = tsp.solveNearestNeighbor()
    distance2 = tsp.solveNearestNeighbor()
    assert distance1 == distance2, f"Distances {distance1} and {distance2} do not match for dataset {dataset}"

def test_empty_dataset():
    delivery_optimizer.setVisualizationCallback(None)
    dataset_path = os.path.join("data", "empty.tsp")
    tsp = delivery_optimizer.TSP(dataset_path)
    distance = tsp.solveNearestNeighbor()
    assert distance == 0, "Distance should be 0 for an empty dataset"

@pytest.mark.parametrize("dataset", ["att48.tsp", "kroD100.tsp", "a280.tsp", "tiny.tsp"])
def test_tsp_reset(dataset):
    delivery_optimizer.setVisualizationCallback(None)
    dataset_path = os.path.join("data", dataset)
    tsp = delivery_optimizer.TSP(dataset_path)
    distance1 = tsp.solveNearestNeighbor()
    tsp = delivery_optimizer.TSP(dataset_path)
    distance2 = tsp.solveNearestNeighbor()
    assert distance1 == distance2, f"Distances {distance1} and {distance2} do not match after reset for dataset {dataset}"
