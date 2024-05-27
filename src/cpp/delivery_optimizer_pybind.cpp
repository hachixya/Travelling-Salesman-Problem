#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "include/delivery_optimizer.hpp"

namespace py = pybind11;

PYBIND11_MODULE(delivery_optimizer, m) {
    py::class_<Location>(m, "Location")
        .def(py::init<double, double>())
        .def_readwrite("latitude", &Location::latitude)
        .def_readwrite("longitude", &Location::longitude);

    py::class_<DeliveryOptimizer>(m, "DeliveryOptimizer")
        .def(py::init<const std::vector<Location>&>())
        .def("load_delivery_locations", &DeliveryOptimizer::load_delivery_locations)
        .def("calculate_cost_matrix", &DeliveryOptimizer::calculate_cost_matrix)
        .def("distance", &DeliveryOptimizer::distance)
        .def("genetic_algorithm", &DeliveryOptimizer::genetic_algorithm)
        .def("simulated_annealing", &DeliveryOptimizer::simulated_annealing)
        .def("route_cost", &DeliveryOptimizer::route_cost)
        .def("getCostMatrix", &DeliveryOptimizer::getCostMatrix)
        .def("getLocations", &DeliveryOptimizer::getLocations);
}
