import math

def read_tsp_file(file_path):
    cities = {}
    with open(file_path, 'r') as file:
        in_section = False
        for line in file:
            if line.strip() == "NODE_COORD_SECTION":
                in_section = True
                continue
            if in_section:
                parts = line.strip().split()
                if len(parts) < 3:
                    continue
                city_id = int(parts[0])
                x = float(parts[1])
                y = float(parts[2])
                cities[city_id] = (x, y)
    return cities

def read_solution_file(file_path):
    with open(file_path, 'r') as file:
        lines = file.readlines()
        distance = int(lines[0].strip())
        solution = [int(line.strip()) for line in lines[1:]]
    return distance, solution

def calculate_total_distance(cities, solution):
    total_distance = 0
    for i in range(len(solution) - 1):
        city1 = solution[i]
        city2 = solution[i + 1]
        total_distance += calculate_distance(cities[city1], cities[city2])
    # Add distance from last to first to make a round trip
    total_distance += calculate_distance(cities[solution[-1]], cities[solution[0]])
    return total_distance

def calculate_distance(city1, city2):
    return math.sqrt((city1[0] - city2[0]) ** 2 + (city1[1] - city2[1]) ** 2)

def verify_solution(tsp_file, solution_file):
    cities = read_tsp_file(tsp_file)
    expected_distance, solution = read_solution_file(solution_file)
    calculated_distance = calculate_total_distance(cities, solution)
    print(f"Expected Distance: {expected_distance}")
    print(f"Calculated Distance: {calculated_distance}")
    # Return True if the calculated distance matches the expected distance with a tolerance of 5%
    return abs(expected_distance - calculated_distance) < expected_distance * 0.05
