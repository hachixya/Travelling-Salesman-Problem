import delivery_optimizer
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import numpy as np

# Global variable to store the frames for animation
frames = []

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
    x_coords = [city.getX() for city in route]
    y_coords = [city.getY() for city in route]
    frames.append((x_coords, y_coords, [city.getId() for city in route], distance))

def main():
    print("Welcome to the TSP Optimizer")
    dataset = input("Enter the TSP dataset file name (e.g., att48.tsp): ")

    tsp = delivery_optimizer.TSP(dataset)
    delivery_optimizer.setVisualizationCallback(update_visualization)

    print("Solving TSP using Nearest Neighbor algorithm...")
    tsp.solveNearestNeighbor()

    animate_route(frames)

def animate_route(frames):
    fig, ax = plt.subplots(figsize=(10, 6))
    line, = ax.plot([], [], 'bo-', markersize=5)
    text = ax.text(0.02, 0.95, '', transform=ax.transAxes)
    annotation = []

    def init():
        ax.set_xlim(min(min(x) for x, y, ids, d in frames) - 10, max(max(x) for x, y, ids, d in frames) + 10)
        ax.set_ylim(min(min(y) for x, y, ids, d in frames) - 10, max(max(y) for x, y, ids, d in frames) + 10)
        return line, text

    def update(frame):
        x, y, ids, distance = frame
        line.set_data(x, y)
        text.set_text(f"Optimized Route (Distance: {distance})")
        
        # Remove old annotations
        for ann in annotation:
            ann.remove()
        annotation.clear()

        # Add new annotations
        for i, city_id in enumerate(ids):
            ann = ax.annotate(f"{city_id}", (x[i], y[i]), textcoords="offset points", xytext=(0,10), ha='center')
            annotation.append(ann)

        return line, text, *annotation

    ani = animation.FuncAnimation(fig, update, frames=frames, init_func=init, blit=True, repeat=False, interval=1000)
    plt.xlabel("X Coordinate")
    plt.ylabel("Y Coordinate")
    plt.grid(True)
    plt.show()

if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        print("Program interrupted")
    finally:
        delivery_optimizer.setVisualizationCallback(None)
