import delivery_optimizer
import matplotlib.pyplot as plt
import folium
import numpy as np

def plot_route(cities, route, title="Optimized Route"):
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

def visualize_route_on_map(cities, route, title="Optimized Route"):
    m = folium.Map(location=[np.mean([city.getY() for city in cities]), np.mean([city.getX() for city in cities])], zoom_start=6)
    for city in cities:
        folium.Marker([city.getY(), city.getX()], popup=f"City {city.getId()}").add_to(m)
    path = [(city.getY(), city.getX()) for city in route]
    folium.PolyLine(path, color="blue", weight=2.5, opacity=1).add_to(m)
    m.save(f"{title}.html")

def main():
    print("Welcome to the TSP Optimizer")
    dataset = input("Enter the TSP dataset file name (e.g., att48.tsp): ")

    tsp = delivery_optimizer.TSP(dataset)

    print("Choose an algorithm to solve TSP:")
    print("1. Nearest Neighbor")
    print("2. Brute Force")
    choice = int(input("Enter your choice (1 or 2): "))

    if choice == 1:
        print("Solving TSP using Nearest Neighbor algorithm...")
        tsp.solveNearestNeighbor()
    elif choice == 2:
        print("Solving TSP using Brute Force algorithm...")
        tsp.solveBruteForce()
    else:
        print("Invalid choice. Exiting.")
        return

    distance = tsp.getSolutionDistance()
    print(f"Optimized route distance: {distance}")

    route = [tsp.getCity(i) for i in range(tsp.getNumberOfCities())]

    print("Choose a visualization method:")
    print("1. Plot route using Matplotlib")
    print("2. Visualize route on interactive map using Folium")
    vis_choice = int(input("Enter your choice (1 or 2): "))

    if vis_choice == 1:
        plot_route([tsp.getCity(i) for i in range(tsp.getNumberOfCities())], route)
    elif vis_choice == 2:
        visualize_route_on_map([tsp.getCity(i) for i in range(tsp.getNumberOfCities())], route)
    else:
        print("Invalid choice. Exiting.")

if __name__ == "__main__":
    main()
