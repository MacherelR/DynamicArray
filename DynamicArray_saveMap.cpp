#include <iostream>
#include <vector>
#include <algorithm> // For std::find_if
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <sstream>

namespace py = pybind11;

class DynamicArray {
private:
    std::map<long int, std::vector<double>> data; // 2D array for storing rows of data
    size_t cols; // Number of columns, fixed

public:
    DynamicArray(size_t columns) : cols(columns) {}

    // void addRow(const std::vector<double>& newRow) {
    //     if (newRow.size() != cols) {
    //         std::cerr << "Error: newRow must have the correct number of columns." << std::endl;
    //         return;
    //     }
    //     auto it = std::lower_bound(data.begin(), data.end(), newRow,
    //         [](const std::vector<double>& a, const std::vector<double>& b) {
    //             return a.front() < b.front();
    //         });
    //     data.insert(it, newRow); // Insert newRow at the correct position
    // }

    void deleteRow(long int timestamp) {
        auto it = data.find(timestamp);
        if (it != data.end()) {
            data.erase(it);
        } else {
            std::cerr << "Error: Timestamp not found." << std::endl;
        }
    }

    void deleteRowById(int index) {
        if (index < 0 || static_cast<size_t>(index) >= data.size()) {
            std::cerr << "Error: Index out of bounds." << std::endl;
            return;
        }
        auto it = data.begin();
        std::advance(it, index);
        data.erase(it);
    }

    bool timestampExists(double timestamp) const {
        return data.find(timestamp) != data.end();
    }


    size_t getNumRows() const {
        return data.size();
    }

    void print() const {
        for (const auto& row : data) {
            std::cout << row.first << ": ";
            for (double val : row.second) {
                std::cout << val << " ";
            }
            std::cout << std::endl;
        }
    }

    long minKey() const {
        if (data.empty()) {
            std::cerr << "Error: No data available." << std::endl;
            return -1; // Or any other indication of error or empty data
        }
        return data.begin()->first; // The smallest key
    }

    long maxKey() const {
        if (data.empty()) {
            std::cerr << "Error: No data available." << std::endl;
            return -1; // Or any other indication of error or empty data
        }
        return data.rbegin()->first; // The biggest key
    }

    void addOrUpdateRow(long timestamp, size_t column_index, double value) {
        if (column_index >= cols) {
            std::cerr << "Error: Column index out of bounds." << std::endl;
            return;
        }

        // Check if the timestamp exists
        auto rowIt = data.find(timestamp);
        if (rowIt != data.end()) {
            // Timestamp exists, update the existing row
            rowIt->second[column_index] = value;
        } else {
            // Timestamp does not exist, create a new row with NaNs
            std::vector<double> newRow(cols, std::nan("")); // Fill with NaN
            newRow[column_index] = value; // Set the value for the specified column
            data[timestamp] = newRow; // Insert the new row
        }
    }

    std::vector<double> getRow(long timestamp) {
        auto it = data.find(timestamp);
        if (it != data.end()) {
            return it->second; // If found, return the row (vector of doubles)
        }
        // Return an empty vector if no row with the specified timestamp is found
        return {};
    }

    // extract row based on index
    std::vector<double> getRowByIndex(int index) {
        if (index < 0 || static_cast<size_t>(index) >= data.size()) {
            std::cerr << "Error: Index out of bounds." << std::endl;
            return {};
        }
        auto it = data.begin();
        std::advance(it, index); // Advance the iterator to the desired index
        return it->second; // Return the row at the index
    }

    py::array_t<double> getSlice(long timestamp, size_t numItems) {
        // Result container for timestamps and values
        std::vector<std::vector<double>> slice;

        auto it = data.lower_bound(timestamp); // Find the iterator to the element or the next larger element if not found
        if (it == data.end()) {
            // If the timestamp is beyond the last element, adjust it to the last element
            if (!data.empty()) {
                it = std::prev(data.end());
            } else {
                // If the map is empty, return an empty array
                std::cerr << "Error: Data is empty." << std::endl;
                return py::array_t<double>(0);
            }
        }

        // Collect up to numItems rows including and before the found timestamp
        size_t count = 0;
        for (; it != data.begin() && count < numItems; --it, ++count) {
            std::vector<double> row;
            row.reserve(cols + 1); // Reserve space for timestamp + values
            row.push_back(static_cast<double>(it->first)); // Add timestamp as the first column
            row.insert(row.end(), it->second.begin(), it->second.end()); // Add the values
            slice.insert(slice.begin(), row); // Prepend to maintain chronological order
        }

        // Make sure to include the row at data.begin() if it hasn't been included yet
        if (count < numItems && it == data.begin()) {
            std::vector<double> row;
            row.reserve(cols + 1); // Reserve space for timestamp + values
            row.push_back(static_cast<double>(it->first)); // Add timestamp as the first column
            row.insert(row.end(), it->second.begin(), it->second.end()); // Add the values
            slice.insert(slice.begin(), row);
        }

        // Convert the collected slice into a NumPy array
        return to_matrix(slice);
    }

    py::array_t<double> to_matrix(const std::vector<std::vector<double>>& slice) {
        if (slice.empty()) {
            return py::array_t<double>(0);
        }
        
        size_t numRows = slice.size();
        // Adjust numCols to include the timestamp column
        size_t numCols = cols + 1; // Assuming cols is set during construction and represents the number of columns

        py::array_t<double, py::array::c_style> result({numRows, numCols});
        auto res = result.mutable_unchecked<2>();

        for (size_t i = 0; i < numRows; ++i) {
            for (size_t j = 0; j < numCols; ++j) {
                res(i, j) = slice[i][j];
            }
        }

        return result;
    }

    // Get all timestamps (first column) in the array
    std::vector<long> getTimestamps() const {
        std::vector<long> timestamps;
        for (const auto& entry : data) {
            timestamps.push_back(entry.first); // entry.first is the key (timestamp) in the map
        }
        return timestamps;
    }    
};

