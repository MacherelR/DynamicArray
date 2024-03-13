# distutils: language = c++
# distutils: sources = DynamicArrayCython.cpp

from libcpp.vector cimport vector
from cpython cimport bool, array
import numpy as np
cimport numpy as np

cdef extern from "DynamicArrayCython.h":
    cdef cppclass DynamicArrayCython:
        DynamicArrayCython(size_t columns) except +
        void deleteRow(long timestamp)
        void removeFirstElement()
        void deleteRange(long timestamp, int numberOfValues)
        void deleteRowById(int index)
        bool timestampExists(double timestamp)
        size_t getNumRows() const
        void print() const
        long minKey() const
        long maxKey() const
        void addOrUpdateRow(long timestamp, size_t column_index, double value)
        vector[double] getRow(long timestamp)
        vector[double] getRowByIndex(int index)
        vector[vector[double]] getSlice(long timestamp, size_t numItems)
        vector[double] getFlattenedSlice(long timestamp, size_t numItems)
        vector[long] getTimestamps() const

cdef extern from "DynamicCounterCython.h":
    cdef cppclass DynamicCounterCython(DynamicArrayCython):
        DynamicCounterCython() except +
        void updateCounterValue(long timestamp, int diff)

cdef class PyDynamicArrayCython:
    cdef DynamicArrayCython* thisptr

    def __cinit__(self, size_t columns):
        self.thisptr = new DynamicArrayCython(columns)

    def __dealloc__(self):
        del self.thisptr

    def delete_row(self, long timestamp):
        self.thisptr.deleteRow(timestamp)

    def remove_first_element(self):
        self.thisptr.removeFirstElement()

    def delete_range(self, long timestamp, int numberOfValues):
        self.thisptr.deleteRange(timestamp, numberOfValues)

    def delete_row_by_id(self, int index):
        self.thisptr.deleteRowById(index)

    def timestamp_exists(self, long timestamp):
        return self.thisptr.timestampExists(timestamp)

    def get_num_rows(self):
        return self.thisptr.getNumRows()

    def print(self):
        self.thisptr.print()

    def min_key(self):
        return self.thisptr.minKey()

    def max_key(self):
        return self.thisptr.maxKey()

    def add_or_update_row(self, long timestamp, size_t column_index, double value):
        self.thisptr.addOrUpdateRow(timestamp, column_index, value)

    def get_row(self, long timestamp):
        cdef vector[double] row = self.thisptr.getRow(timestamp)
        return row

    def get_row_by_index(self, int index):
        cdef vector[double] row = self.thisptr.getRowByIndex(index)
        return row

    def get_slice(self, long timestamp, size_t numItems):
        cdef vector[vector[double]] slice = self.thisptr.getSlice(timestamp, numItems)
        # Conversion from C++ vector to Python list
        return [[item for item in inner] for inner in slice]

    def get_flattened_slice(self, long timestamp, size_t numItems):
        cdef vector[double] flat = self.thisptr.getFlattenedSlice(timestamp, numItems)
        return flat

    def get_slice_as_numpy(self, long timestamp, size_t numItems, size_t num_rows, size_t num_cols):
        cdef vector[double] flat = self.thisptr.getFlattenedSlice(timestamp, numItems)
        cdef np.ndarray[np.float64_t, ndim=2] arr = np.asarray(flat).reshape(num_rows, num_cols)
        return arr

    def get_timestamps(self):
        cdef vector[long] timestamps = self.thisptr.getTimestamps()
        return timestamps

cdef class PyDynamicCounterCython(PyDynamicArrayCython):
    def __cinit__(self):
        self.thisptr = new DynamicCounterCython()

    def update_counter_value(self, long timestamp, int diff):
        (<DynamicCounterCython*>self.thisptr).updateCounterValue(timestamp, diff)
