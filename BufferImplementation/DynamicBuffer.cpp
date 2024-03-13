#include "DynamicBuffer.h"
#include <vector>

DynamicBuffer::DynamicBuffer(size_t nVariables, size_t windowSize)
    : nVariables(nVariables), windowSize(windowSize),
      bufferLength(DEFAULT_BUFFER_LENGTH_FACTOR * windowSize * nVariables),
      data(bufferLength, std::nan("")) {}

bool DynamicBuffer::deleteRecord(long timestamp) {
  // Find the index for the given timestamp
  auto it = indexes.find(timestamp);
  if (it == indexes.end()) {
    // Timestamp not found
    return false;
  }

  size_t startIndex = it->second;
  size_t endIndex = startIndex + nVariables;

  // Erase the nVariables from data starting from startIndex
  data.erase(data.begin() + startIndex, data.begin() + endIndex);

  // Remove the timestamp entry from indexes
  indexes.erase(it);

  // Adjust subsequent indexes in the map
  for (auto &entry : indexes) {
    if (entry.second > startIndex) {
      entry.second -= nVariables;
    }
  }

  return true;
}

void DynamicBuffer::addOrUpdateRecord(long timestamp, size_t columnIndex,
                                      double value) {
  if (columnIndex >= nVariables) {
    throw std::invalid_argument("Column index out of range");
  }

  size_t dataIndex;
  // Check if this timestamp already exists.
  auto it = indexes.find(timestamp);
  if (it != indexes.end()) {
    // Timestamp exists: calculate the direct index in the data vector and
    // update the value.
    dataIndex = it->second + columnIndex;
    data[dataIndex] = value;
  } else {
    // Timestamp does not exist: we need to insert a new record.
    if (indexes.empty() || timestamp > indexes.rbegin()->first) {
      // Insert at the end if the map is empty or this is the newest timestamp.
      dataIndex = data.size();
      indexes[timestamp] = dataIndex;
      data.resize(
          dataIndex + nVariables,
          std::nan(
              "")); // Ensure space for nVariables, filling new spots with NaN.
    } else {
      // Find the correct position for this timestamp.
      auto nextTimestampIt = indexes.upper_bound(timestamp);
      dataIndex = nextTimestampIt->second;
      indexes[timestamp] = dataIndex;

      // Shift data for existing timestamps forward by nVariables positions.
      data.insert(data.begin() + dataIndex, nVariables, std::nan(""));

      // Update indexes for all timestamps after this one.
      for (auto updateIt = nextTimestampIt; updateIt != indexes.end();
           ++updateIt) {
        updateIt->second += nVariables;
      }
    }

    // Set the value for the specified column in the new record.
    data[dataIndex + columnIndex] = value;
  }
}

void DynamicBuffer::print() const {
  for (const auto &pair : indexes) {
    long timestamp = pair.first;
    size_t startIndex = pair.second;

    std::cout << timestamp << ": ";

    for (size_t i = 0; i < nVariables; ++i) {
      if (startIndex + i < data.size()) {
        std::cout << data[startIndex + i] << ", ";
      }
    }

    std::cout << std::endl;
  }
}

std::vector<double> DynamicBuffer::getRecordByTimestamp(long timestamp) const {
  auto it = indexes.find(timestamp);
  if (it == indexes.end()) {
    throw std::invalid_argument("Timestamp not found");
  }
  auto index = it->second;
  return std::vector<double>(data.begin() + index,
                             data.begin() + index + nVariables);
}

std::vector<double> DynamicBuffer::getRecordByIndex(size_t index) const {
  if (index >= indexes.size()) {
    throw std::out_of_range("Index out of range");
  }
  return std::vector<double>(data.begin() + index,
                             data.begin() + index + nVariables);
}

const double *DynamicBuffer::getRecordByTimestampPtr(long timestamp,
                                                     size_t &outSize) const {
  auto it = indexes.find(timestamp);
  if (it != indexes.end()) {
    size_t startIndex = it->second;
    // Ensure it doesn't exceed the data vector size
    if (startIndex + nVariables <= data.size()) {
      outSize = nVariables;
      return &data[startIndex];
    }
  }
  outSize = 0;
  return nullptr;
}

const double *DynamicBuffer::getRecordByIndexPtr(size_t index) const {
  size_t startIndex = index * nVariables;

  if (index < (windowSize * DEFAULT_BUFFER_LENGTH_FACTOR) &&
      startIndex + nVariables <= data.size()) {
    return &data[startIndex];
  }

  return nullptr;
}

const double *DynamicBuffer::getSlice(long timestamp, size_t N,
                                      size_t &outSize) const {
  auto it = indexes.find(timestamp);
  if (it != indexes.end()) {
    size_t targetIndex = it->second;

    size_t startIndex = (N > (targetIndex / nVariables + 1))
                            ? 0
                            : targetIndex - (N - 1) * nVariables;

    // Calculate the size of the slice in terms of number of doubles
    outSize = std::min(data.size() - startIndex, N * nVariables);

    // Check if the calculated slice exceeds the bounds of the data vector
    if (startIndex + outSize <= data.size()) {
      return &data[startIndex];
    }
  }
  outSize = 0;
  return nullptr; // Return nullptr if the request cannot be fulfilled
}

size_t DynamicBuffer::getNVariables() const { return nVariables; }
