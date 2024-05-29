import delivery_optimizer
import matplotlib.pyplot as plt
import folium
import numpy as np

def plot_route(route, title="Optimized Route"):
    x_coords = [city.getX() for city in route]
    y_coords = [city.getY() for city in route]
    plt.figure(figsize=(10, 6))
    plt.plot(x_coords, y_coords, 'bo-', markersize=5)
    for i, city in enumerate(route):
        plt.text(city.getX(), city.getY(), f"{city.getId()}", fontsize=12)
    plt.title(title)
    plt.xlabel("X Coordinate")
    plt.ylabel("Y Coordinate")
    plt.grid(True)
    plt.show()

def update_visualization(distance, tsp):
    print(f"Updating visualization for distance: {distance}")
    route = [tsp.getCity(i) for i in range(tsp.getNumberOfCities())]
    plot_route(route, f"Optimized Route (Distance: {distance})")

def main():
    print("Welcome to the TSP Optimizer")
    dataset = input("Enter the TSP dataset file name (e.g., att48.tsp): ")

    tsp = delivery_optimizer.TSP(dataset)
    delivery_optimizer.setVisualizationCallback(update_visualization)

    print("Solving TSP using Nearest Neighbor algorithm...")
    tsp.solveNearestNeighbor()

if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        print("Program interrupted")
    finally:
        delivery_optimizer.setVisualizationCallback(None)
