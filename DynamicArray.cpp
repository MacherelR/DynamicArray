#include "btree/map.h"
#include <algorithm> // For std::find_if
#include <iostream>
#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>
#include <sstream>
#include <vector>

namespace py = pybind11;

class DynamicArray {
protected:
  btree::map<long, std::vector<double>>
      data;    // 2D array for storing rows of data
  size_t cols; // Number of columns, fixed

public:
  DynamicArray(size_t columns) : cols(columns) {}

  void deleteRow(long int timestamp) {
    auto it = data.find(timestamp);
    if (it != data.end()) {
      data.erase(it);
    } else {
      std::cerr << "Error: Timestamp not found." << std::endl;
    }
  }

  void removeFirstElement() {
    if (!data.empty()) {
      data.erase(data.begin());
    } else {
      std::cerr << "Error: Data is empty." << std::endl;
    }
  }

  void deleteRange(long int timestamp, int numberOfValues) {
    auto it = data.lower_bound(timestamp);
    // Retrieve iterator for item that is numberOfValues before the timestamp
    if (it != data.begin()) {
      std::advance(it, -numberOfValues);
    }
    data.erase(it, data.lower_bound(timestamp));
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

  size_t getNumRows() const { return data.size(); }

  void print() const {
    for (const auto &row : data) {
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

    auto &row =
        data[timestamp]; // This will default-construct a vector if not found
    if (row.size() < cols) {
      row.resize(cols, std::nan("")); // Ensure the row has the correct size,
                                      // fill with NaNs if needed
    }
    row[column_index] = value;
  }

  std::vector<double> getRow(long timestamp) {
    auto it = data.find(timestamp);
    if (it != data.end()) {
      return it->second;
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
    return it->second;       // Return the row at the index
  }

  py::array_t<double> getSlice(long timestamp, size_t numItems) {
    if (data.empty()) {
      std::cerr << "Error: Data is empty." << std::endl;
      return py::array_t<double>(0);
    }

    auto it = data.lower_bound(timestamp);
    if (it == data.end()) {
      it = std::prev(data.end());
    }

    std::vector<std::vector<double>> slice;
    slice.reserve(
        numItems); // Assume numItems is not larger than the size of the data

    for (size_t count = 0; it != data.begin() && count < numItems;
         --it, ++count) {
      slice.push_back({static_cast<double>(it->first)});
      slice.back().insert(slice.back().end(), it->second.begin(),
                          it->second.end());
    }

    if (it == data.begin() && numItems > slice.size()) {
      slice.push_back({static_cast<double>(it->first)});
      slice.back().insert(slice.back().end(), it->second.begin(),
                          it->second.end());
    }

    std::reverse(slice.begin(), slice.end()); // Ensure chronological order

    return to_matrix(slice);
  }

  py::array_t<double> to_matrix(const std::vector<std::vector<double>> &slice) {
    if (slice.empty()) {
      return py::array_t<double>(0);
    }

    const size_t numRows = slice.size();
    const size_t numCols =
        cols + 1; // Assuming 'cols' is the number of columns without the
                  // timestamp, set during construction

    py::array_t<double, py::array::c_style> result({numRows, numCols});
    auto res = result.mutable_unchecked<2>(); // Direct access without bounds
                                              // checking for performance

    for (size_t i = 0; i < numRows; ++i) {
      const std::vector<double> &currentRow = slice[i];
      for (size_t j = 0; j < numCols; ++j) {
        res(i, j) = currentRow[j];
      }
    }

    return result;
  }

  // Get all timestamps (first column) in the array
  std::vector<long> getTimestamps() const {
    std::vector<long> timestamps;
    for (const auto &entry : data) {
      timestamps.push_back(
          entry.first); // entry.first is the key (timestamp) in the map
    }
    return timestamps;
  }
};
