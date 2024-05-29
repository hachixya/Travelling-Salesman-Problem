import matplotlib.pyplot as plt
import numpy as np

def onclick(event, points):
    if event.inaxes:
        points.append((event.xdata, event.ydata))
        event.inaxes.plot(event.xdata, event.ydata, 'bo')
        plt.draw()

def place_points():
    points = []
    fig, ax = plt.subplots()
    ax.set_title('Click to place points')
    ax.set_xlim(0, 100)
    ax.set_ylim(0, 100)
    fig.canvas.mpl_connect('button_press_event', lambda event: onclick(event, points))
    plt.show()
    return points

def save_points_to_tsp(points, filename="custom.tsp"):
    with open(filename, "w") as f:
        f.write("NAME : custom\n")
        f.write("COMMENT : Custom points\n")
        f.write("TYPE : TSP\n")
        f.write(f"DIMENSION : {len(points)}\n")
        f.write("EDGE_WEIGHT_TYPE : EUC_2D\n")
        f.write("NODE_COORD_SECTION\n")
        for i, (x, y) in enumerate(points, start=1):
            f.write(f"{i} {int(x)} {int(y)}\n")
        f.write("EOF\n")

if __name__ == "__main__":
    points = place_points()
    save_points_to_tsp(points)
    print(f"Points saved to custom.tsp")
