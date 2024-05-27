#pragma once

#include <fstream>
#include <vector>
#include <deque>

class City {
public:
    City(int id, int x, int y, int pos, bool visited);
    City(const City &source);
    ~City();
    int distanceTo(const City *city) const;
    int getX() const;
    int getY() const;
    int getId() const;
    int getPosition() const;
    int getNeighborListSize() const;
    City *& getNeighbor(int index);
    int getNeighborPosition(int index) const;
    void setPosition(int pos);
    void displayCoordinates() const;
    void outputId(std::ostream &output) const;
    int writeToFile(std::ofstream &write) const;
    int copyFrom(const City *city);
    bool isEqual(const City *city) const;
    void buildNeighborList(std::deque<City*> &cities, int numCities);
    void pushToList(City *&toAdd);
    bool isNeighborListEmpty() const;
    void displayNeighborList() const;

private:
    int id;
    int x;
    int y;
    int pos;
    bool visited;
    std::deque<City*> neighborList;
};
