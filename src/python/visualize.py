import matplotlib.pyplot as plt
import matplotlib.animation as animation

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

def animate_route(frames, interval=1000):
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

    ani = animation.FuncAnimation(fig, update, frames=frames, init_func=init, blit=True, repeat=False, interval=interval)
    plt.xlabel("X Coordinate")
    plt.ylabel("Y Coordinate")
    plt.grid(True)
    plt.show()
