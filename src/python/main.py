import delivery_optimizer
from visualize import update_visualization, animate_route, frames
from place_points import place_points, save_points_to_tsp

def main():
    print("Welcome to the TSP Optimizer")
    choice = input("Do you want to place points manually? (yes/no): ").strip().lower()

    if choice == "yes":
        points = place_points()
        save_points_to_tsp(points)
        dataset = "custom.tsp"
    else:
        dataset = input("Enter the TSP dataset file name (e.g., att48.tsp): ").strip()

    tsp = delivery_optimizer.TSP(dataset)
    delivery_optimizer.setVisualizationCallback(update_visualization)

    print("Solving TSP using Nearest Neighbor algorithm...")
    tsp.solveNearestNeighbor()

    distance = tsp.getSolutionDistance()
    print(f"Optimized route distance: {distance}")

    animate_route(frames)

if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        print("Program interrupted")
    finally:
        delivery_optimizer.setVisualizationCallback(None)
