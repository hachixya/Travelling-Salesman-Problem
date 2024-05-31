import os
import delivery_optimizer
from visualize import update_visualization, animate_route, frames
from place_points import place_points, save_points_to_tsp

def main():
    print("Welcome to the TSP Optimizer")
    choice = input("Do you want to place points manually? (yes/no): ").strip().lower()

    if choice == "yes":
        points = place_points()
        save_points_to_tsp(points, os.path.join("data", "custom.tsp"))
        dataset = "custom.tsp"
    else:
        dataset = input("Enter the TSP dataset file name (e.g., att48.tsp): ").strip()

    dataset_path = os.path.join("data", dataset)
    if not os.path.isfile(dataset_path):
        print(f"Dataset file {dataset_path} not found.")
        return

    tsp = delivery_optimizer.TSP(dataset_path)
    delivery_optimizer.setVisualizationCallback(update_visualization)
    if choice == "yes":
        algorithm = input("Choose algorithm to solve TSP:\n1. Nearest Neighbor\n2. Genetic Algorithm\n3. Brute Force\n").strip()
    else:
        algorithm = input("Choose algorithm to solve TSP:\n1. Nearest Neighbor\n2. Genetic Algorithm\n").strip()
    
    if algorithm == "1":
        print("Solving TSP using Nearest Neighbor algorithm...")
        tsp.solveNearestNeighbor()
    elif algorithm == "2":
        print("Solving TSP using Genetic Algorithm...")
        population_size = int(input("Enter population size: ").strip())
        generations = int(input("Enter number of generations: ").strip())
        crossover_rate = float(input("Enter crossover rate (0 to 1): ").strip())
        mutation_rate = float(input("Enter mutation rate (0 to 1): ").strip())
        tsp.solveGeneticAlgorithm(population_size, generations, crossover_rate, mutation_rate)
    elif algorithm == "3" and choice == "yes":
        print("Solving TSP using Brute Force algorithm...")
        tsp.solveBruteForce()
    else:
        print("Invalid choice or Brute Force is not available for pre-defined datasets. Exiting.")
        return

    distance = tsp.getSolutionDistance()
    print(f"Optimized route distance: {distance}")

    if algorithm == "2":
        animate_route(frames, interval=10)  # Faster animation for genetic algorithm
    else:
        animate_route(frames)

if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        print("Program interrupted")
    finally:
        delivery_optimizer.setVisualizationCallback(None)
