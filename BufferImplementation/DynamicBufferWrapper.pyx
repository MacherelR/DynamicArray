# distutils: language = c++
# distutils: sources = DynamicBuffer_lib/DynamicBuffer.cpp
from libc.stdint cimport int64_t
from libcpp.vector cimport vector
from cpython cimport array
from libcpp cimport bool
import numpy as np
cimport numpy as np

np.import_array()

cdef extern from "DynamicBuffer_lib/DynamicBuffer.h":
    cdef cppclass DynamicBuffer:
        DynamicBuffer(size_t nVariables, size_t windowSize) except +
        bool deleteRecord(long timestamp)
        bool addOrUpdateRecord(long timestamp, size_t column_index, double value)
        void print()
        const double *getRecordByTimestampPtr(long timestamp, size_t &outSize) const
        const double *getSlice(long timestamp, size_t N, size_t &outSize) const
        size_t getNVariables() const
        void removeFront(size_t removeCount)
        long minKey() const
        long maxKey() const
        size_t getNumRows() const
        vector[long] getSliceTimestamps(long timestamp, size_t N) const
        void decrementCounters(const vector[long]& timestamps)
        vector[int] getCounters() const
        void printCounters() const

cdef extern from "DynamicBuffer_lib/LastKnownValuesBuffer.h":
    cdef cppclass LastKnownValuesBuffer(DynamicBuffer):
        LastKnownValuesBuffer(size_t nVariables, size_t windowSize) except +
        bool updateLastKnownValue(long timestamp, size_t column_index, double value)


cdef class PyDynamicBuffer:
    cdef DynamicBuffer *thisptr
    def __cinit__(self, size_t nVariables, size_t windowSize):
        self.thisptr = new DynamicBuffer(nVariables, windowSize)

    def __dealloc__(self):
        del self.thisptr

    def delete_record(self, long timestamp):
        return self.thisptr.deleteRecord(timestamp)

    def add_or_update_record(self, long timestamp, size_t column_index, double value):
        cdef bool res = self.thisptr.addOrUpdateRecord(timestamp, column_index, value)
        return res

    def print(self):
        self.thisptr.print()

    def get_row_as_numpy(self, long timestamp):
        cdef size_t rowSize = 0
        cdef np.npy_intp shape[1]
        cdef const double *row = self.thisptr.getRecordByTimestampPtr(timestamp, rowSize)
        if row is not NULL and rowSize > 0:
            shape[0] = rowSize
            return np.PyArray_SimpleNewFromData(1, shape, np.NPY_DOUBLE, <void*>row)
        else:
            return np.array([])

    def get_slice_as_numpy(self, long timestamp, size_t N):
        cdef size_t sliceSize = 0
        cdef const double *slice = self.thisptr.getSlice(timestamp, N, sliceSize)
        if slice is NULL:
            raise ValueError("Slice cannot be retrieved")

        cdef size_t num_elements = sliceSize  # Total number of double elements in the slice
        cdef size_t num_rows = num_elements // self.thisptr.getNVariables()  # Calculate the number of rows
        cdef np.npy_intp dims[2]
        dims[0] = num_rows
        dims[1] = self.thisptr.getNVariables()

        # Note: Setting mode='c' ensures the NumPy array is C-contiguous
        return np.PyArray_SimpleNewFromData(2, dims, np.NPY_FLOAT64, <void*>slice)

    def get_slice_timestamps(self, long timestamp, size_t N):
        cdef vector[long] timestamps = self.thisptr.getSliceTimestamps(timestamp, N)
        return [timestamp for timestamp in timestamps]

    def remove_front(self, size_t removeCount):
        self.thisptr.removeFront(removeCount)

    def min_key(self):
        return self.thisptr.minKey()

    def max_key(self):
        return self.thisptr.maxKey()

    def get_num_rows(self):
        return self.thisptr.getNumRows()

    def decrement_counters(self, list timestamps):
        cdef vector[long] cpp_timestamps = vector[long]()
        for timestamp in timestamps:
            cpp_timestamps.push_back(timestamp)
        self.thisptr.decrementCounters(cpp_timestamps)

    def get_counters(self):
        cdef vector[int] counters = self.thisptr.getCounters()
        return [counter for counter in counters]

    def print_counters(self):
        self.thisptr.printCounters()


cdef class PyLastKnownValuesBuffer(PyDynamicBuffer):
    def __cinit__(self, size_t nVariables, size_t windowSize):
        self.thisptr = new LastKnownValuesBuffer(nVariables, windowSize)

    def update_last_known_value(self, long timestamp, size_t column_index, double value):
        cdef bool res = (<LastKnownValuesBuffer*>self.thisptr).updateLastKnownValue(timestamp, column_index, value)
        return res