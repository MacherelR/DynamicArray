#include <pybind11/pybind11.h>
#include <pybind11/stl.h> // To handle conversion of standard data types
#include "DynamicArray.cpp" // Include your original C++ class

namespace py = pybind11;

PYBIND11_MODULE(dynamic_array, m) {
    py::class_<DynamicArray>(m, "DynamicArray")
        .def(py::init<size_t>())
        .def("addRow", &DynamicArray::addRow)
        .def("deleteRows", &DynamicArray::deleteRows)
        .def("update", &DynamicArray::update)
        .def("print", &DynamicArray::print)
        .def("timestampExists", &DynamicArray::timestampExists)
        .def("getLength", &DynamicArray::getNumRows)
        .def("minKey", &DynamicArray::minKey)
        .def("updateRow", &DynamicArray::addOrUpdateRow)
        .def("maxKey", &DynamicArray::maxKey)
        .def("getRow", &DynamicArray::getRow)
        .def("getSlice", &DynamicArray::getSlice)
        .def("getRowIndex", &DynamicArray::getRowIndex)
        .def("getTimestamps", &DynamicArray::getTimestamps);
}
