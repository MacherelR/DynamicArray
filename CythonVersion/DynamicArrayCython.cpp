#include "DynamicArrayCython.h"

DynamicArrayCython::DynamicArrayCython(size_t columns) : cols(columns) {}

void DynamicArrayCython::deleteRow(long timestamp) {
  auto it = data.find(timestamp);
  if (it != data.end()) {
    data.erase(it);
  } else {
    std::cerr << "Error: Timestamp not found." << std::endl;
  }
}

void DynamicArrayCython::removeFirstElement() {
  if (!data.empty()) {
    data.erase(data.begin());
  } else {
    std::cerr << "Error: Data is empty." << std::endl;
  }
}

void DynamicArrayCython::deleteRange(long timestamp, int numberOfValues) {
  auto it = data.lower_bound(timestamp);
  // Retrieve iterator for item that is numberOfValues before the timestamp
  if (it != data.begin()) {
    std::advance(it, -numberOfValues);
  }
  data.erase(it, data.lower_bound(timestamp));
}

void DynamicArrayCython::deleteRowById(int index) {
  if (index < 0 || static_cast<size_t>(index) >= data.size()) {
    std::cerr << "Error: Index out of bounds." << std::endl;
    return;
  }
  auto it = data.begin();
  std::advance(it, index);
  data.erase(it);
}

bool DynamicArrayCython::timestampExists(long timestamp) const {
  return data.find(timestamp) != data.end();
}

size_t DynamicArrayCython::getNumRows() const { return data.size(); }

void DynamicArrayCython::print() const {
  for (const auto &row : data) {
    std::cout << row.first << ": ";
    for (double val : row.second) {
      std::cout << val << " ";
    }
    std::cout << std::endl;
  }
}

long DynamicArrayCython::minKey() const {
  if (data.empty()) {
    std::cerr << "Error: No data available." << std::endl;
    return -1; // Or any other indication of error or empty data
  }
  return data.begin()->first; // The smallest key
}

long DynamicArrayCython::maxKey() const {
  if (data.empty()) {
    std::cerr << "Error: No data available." << std::endl;
    return -1; // Or any other indication of error or empty data
  }
  return data.rbegin()->first; // The biggest key
}

void DynamicArrayCython::addOrUpdateRow(long timestamp, size_t column_index,
                                        double value) {
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

std::vector<double> DynamicArrayCython::getRow(long timestamp) {
  auto it = data.find(timestamp);
  if (it != data.end()) {
    return it->second;
  }
  // Return an empty vector if no row with the specified timestamp is found
  return {};
}

// extract row based on index
std::vector<double> DynamicArrayCython::getRowByIndex(int index) {
  if (index < 0 || static_cast<size_t>(index) >= data.size()) {
    std::cerr << "Error: Index out of bounds." << std::endl;
    return {};
  }
  auto it = data.begin();
  std::advance(it, index); // Advance the iterator to the desired index
  return it->second;       // Return the row at the index
}

std::vector<std::vector<double>> DynamicArrayCython::getSlice(long timestamp,
                                                              size_t numItems) {
  if (data.empty()) {
    std::cerr << "Error: Data is empty." << std::endl;
    return {};
  }

  auto it = data.lower_bound(timestamp);
  if (it == data.end()) {
    it = std::prev(data.end());
  }

  std::vector<std::vector<double>> slice;
  slice.reserve(numItems);

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

  return slice;
}

// Get all timestamps (first column) in the array
std::vector<long> DynamicArrayCython::getTimestamps() const {
  std::vector<long> timestamps;
  for (const auto &entry : data) {
    timestamps.push_back(
        entry.first); // entry.first is the key (timestamp) in the map
  }
  return timestamps;
}

std::vector<double> DynamicArrayCython::getFlattenedSlice(long timestamp,
                                                          size_t numItems) {
  auto slice = getSlice(timestamp, numItems);
  std::vector<double> flattenedSlice;
  for (const auto &row : slice) {
    flattenedSlice.insert(flattenedSlice.end(), row.begin(), row.end());
  }
  return flattenedSlice;
}
