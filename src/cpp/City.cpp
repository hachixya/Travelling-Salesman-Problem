#include "City.hpp"
#include "Tree.hpp"
#include <cmath>
#include <iostream>

const int LIST_SIZE = 5;

City::City(int idin, int xin, int yin, int posin, bool visitedin)
    : id(idin), x(xin), y(yin), pos(posin), visited(visitedin) {}

City::City(const City &source)
    : id(source.id), x(source.x), y(source.y), pos(source.pos), visited(source.visited), neighborList(source.neighborList) {}

City::~City() {}

int City::distanceTo(const City *city) const {
    if (!city) return 0;
    float dx = static_cast<float>(city->getX() - x);
    float dy = static_cast<float>(city->getY() - y);
    return static_cast<int>(std::round(std::sqrt(dx * dx + dy * dy)));
}

int City::getX() const {
    return x;
}

int City::getY() const {
    return y;
}

int City::getId() const {
    return id;
}

int City::getPosition() const {
    return pos;
}

int City::getNeighborListSize() const {
    return LIST_SIZE;
}

City *& City::getNeighbor(int index) {
    return neighborList[index];
}

int City::getNeighborPosition(int index) const {
    return neighborList[index]->getPosition();
}

void City::setPosition(int pos_in) {
    pos = pos_in;
}

void City::displayCoordinates() const {
    std::cout << id << " " << x << " " << y << std::endl;
}

void City::outputId(std::ostream &output) const {
    output << id;
}

int City::writeToFile(std::ofstream &write) const {
    if (write.is_open()) {
        write << id << '\n';
        return 1;
    }
    return 0;
}

int City::copyFrom(const City *city_in) {
    if (!city_in) {
        return 0;
    }
    id = city_in->id;
    x = city_in->x;
    y = city_in->y;
    pos = city_in->pos;
    visited = city_in->visited;
    neighborList = city_in->neighborList;
    return 1;
}

bool City::isEqual(const City *city_in) const {
    return id == city_in->id && x == city_in->x && y == city_in->y;
}

void City::buildNeighborList(std::deque<City*> &cities, int numCities) {
    Tree sorted(this);
    neighborList.clear();
    for (int i = 0; i < numCities; ++i) {
        sorted += cities[i];
    }
    int size = (LIST_SIZE < numCities) ? LIST_SIZE : numCities;
    sorted.buildNeighborList(neighborList, size);
    sorted.clear();
}

bool City::isNeighborListEmpty() const {
    return neighborList.empty();
}

void City::pushToList(City *&toAdd) {
    neighborList.push_back(new City(*toAdd));
}

void City::displayNeighborList() const {
    for (auto neighbor : neighborList) {
        std::cout << distanceTo(neighbor) << " actual index: " << neighbor->getPosition() << std::endl;
    }
}
