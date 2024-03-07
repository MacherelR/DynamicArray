#include <pybind11/pybind11.h>
#include <pybind11/stl.h> // To handle conversion of standard data types
#include "DynamicArray.cpp" // Include your original C++ class
#include "DynamicCounter.cpp" // Include your original C++ class

namespace py = pybind11;

PYBIND11_MODULE(dynamic_array, m) {
    // Binding for DynamicArray class
    py::class_<DynamicArray>(m, "DynamicArray")
        .def(py::init<size_t>())
        .def("deleteRow", &DynamicArray::deleteRow)
        .def("deleteRange", &DynamicArray::deleteRange)
        .def("deleteRowById", &DynamicArray::deleteRowById)
        .def("print", &DynamicArray::print)
        .def("timestampExists", &DynamicArray::timestampExists)
        .def("getLength", &DynamicArray::getNumRows)
        .def("minKey", &DynamicArray::minKey)
        .def("updateRow", &DynamicArray::addOrUpdateRow)
        .def("maxKey", &DynamicArray::maxKey)
        .def("getRow", &DynamicArray::getRow)
        .def("getRowByIndex", &DynamicArray::getRowByIndex)
        .def("getSlice", &DynamicArray::getSlice)
        .def("getTimestamps", &DynamicArray::getTimestamps);
    
    // Binding for DynamicCounter class
    py::class_<DynamicCounter, DynamicArray>(m, "DynamicCounter")
        .def(py::init<>())
        .def("updateCounterValue", &DynamicCounter::updateCounterValue);
}
