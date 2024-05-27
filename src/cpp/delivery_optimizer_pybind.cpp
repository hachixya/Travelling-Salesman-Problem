#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "City.hpp"
#include "TSP.hpp"

namespace py = pybind11;

PYBIND11_MODULE(delivery_optimizer, m) {
    py::class_<City>(m, "City")
        .def(py::init<int, int, int, int, bool>())
        .def("getId", &City::getId)
        .def("getX", &City::getX)
        .def("getY", &City::getY)
        .def("getPosition", &City::getPosition)
        .def("distanceTo", &City::distanceTo);

    py::class_<TSP>(m, "TSP")
        .def(py::init<const char*>())
        .def("solveNearestNeighbor", &TSP::solveNearestNeighbor)
        .def("solveBruteForce", &TSP::solveBruteForce)
        .def("getSolutionDistance", &TSP::getSolutionDistance)
        .def("writeSolution", &TSP::writeSolution);
}
