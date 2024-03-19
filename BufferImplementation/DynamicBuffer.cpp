#include "DynamicBuffer.h"
#include "constants.h"
#include <unistd.h>
#include <vector>

const bool DEBUG = false;

DynamicBuffer::DynamicBuffer(size_t nVariables, size_t windowSize)
    : nVariables(nVariables), windowSize(windowSize),
      bufferLength(DEFAULT_BUFFER_LENGTH_FACTOR * windowSize * nVariables),
      data(bufferLength, std::nan("")),
      counters((DEFAULT_BUFFER_LENGTH_FACTOR * windowSize), 0) {}

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
  if (DEBUG) {
    std::cout << "Inserting or updating data. Size is : " << indexes.size()
              << ", Timestamp is : " << timestamp << ", value is : " << value
              << std::endl;
  }

  size_t dataIndex;
  auto it = indexes.find(timestamp);
  auto nextTimestampIt = indexes.end(); // Initialize to end as a default

  if (it != indexes.end()) {
    // Timestamp exists: update the value directly.
    dataIndex = it->second + columnIndex;
    if (dataIndex < bufferLength) {
      data[dataIndex] = value;
      size_t rowIndex = dataIndex / nVariables;
      counters[rowIndex]++;
    } else {
      throw std::out_of_range("Attempting to write beyond the buffer length");
    }
  } else {
    // Check if there is enough room for a new record
    if (!hasEnoughRoomForNewRecord()) {
      // Remove all rows with zero counters
      removeZeroCount();
      if (!hasEnoughRoomForNewRecord()) {
        throw std::out_of_range("Buffer is full and can't be emptied further.");
      }
    }

    // Timestamp does not exist: determine insertion point.
    if (indexes.empty() || timestamp > indexes.rbegin()->first) {
      dataIndex =
          (!indexes.empty()) ? (indexes.rbegin()->second + nVariables) : 0;
    } else {
      nextTimestampIt = indexes.upper_bound(timestamp);
      dataIndex =
          (nextTimestampIt != indexes.end()) ? nextTimestampIt->second : 0;
    }

    // Update indexes and data.
    indexes[timestamp] = dataIndex;
    // Update indexes map:
    for (auto iter = nextTimestampIt; iter != indexes.end(); ++iter) {
      iter->second += nVariables;
    }

    if (dataIndex + columnIndex < bufferLength) {
      data[dataIndex + columnIndex] = value;
      // Shift data or update indexes for all timestamps after this one, if
      // necessary.
      if (nextTimestampIt != indexes.end()) {
        for (auto updateIt = nextTimestampIt; updateIt != indexes.end();
             ++updateIt) {
          updateIt->second += nVariables;
        }
      }
      size_t rowIndex = dataIndex / nVariables;
      counters[rowIndex] = 1;
    } else {
      throw std::out_of_range("Attempting to write beyond the buffer length");
    }
  }
  if (DEBUG) {
    std::cout << "Value " << value << " at timestamp " << timestamp
              << " inserted !" << std::endl;
  }
}

void DynamicBuffer::print() const {
  // Debug method to print the contents of the buffer
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

std::vector<long> DynamicBuffer::getSliceTimestamps(long timestamp,
                                                    size_t N) const {
  std::vector<long> timestamps;
  auto it = indexes.find(timestamp);
  if (it != indexes.end()) {
    size_t targetIndex = it->second;
    size_t startIndex = (N > (targetIndex / nVariables + 1))
                            ? 0
                            : targetIndex - (N - 1) * nVariables;
    size_t endIndex = std::min(startIndex + N * nVariables, data.size());

    for (auto &pair : indexes) {
      if (pair.second >= startIndex && pair.second < endIndex) {
        timestamps.push_back(pair.first);
      }
    }
  }
  return timestamps;
}

size_t DynamicBuffer::getNVariables() const { return nVariables; }

void DynamicBuffer::removeFront(size_t removeCount) {
  if (DEBUG) {
    std::cout << "Removing " << removeCount << " elements from the front"
              << std::endl;
    std::cout << "Indexes size before removing: " << indexes.size()
              << std::endl;
  }
  size_t originalSize = data.size();
  size_t elementsToRemove = removeCount * nVariables;
  if (removeCount >= originalSize) {
    // Clear the vector if removing more or equal elements than contained
    data.clear();
    counters.clear();
    // Fill the entire vector with NaN, maintaining original size
    data.resize(originalSize, std::nan(""));
    counters.resize((DEFAULT_BUFFER_LENGTH_FACTOR * windowSize), 0);
    // Clear indexes map as all elements are removed
    indexes.clear();
  } else {
    // Move the remaining elements to the beginning
    std::move(data.begin() + elementsToRemove, data.end(), data.begin());
    // Resize the vector down, then back up to original size, filling with NaNs
    data.resize(originalSize - elementsToRemove);
    std::fill_n(std::back_inserter(data), elementsToRemove, std::nan(""));

    std::move(counters.begin() + removeCount, counters.end(), counters.begin());
    counters.resize((DEFAULT_BUFFER_LENGTH_FACTOR * windowSize), 0);
    // Adjust the indexes map:
    // Create a new map to store updated indexes
    std::map<long, size_t> updatedIndexes;
    for (const auto &pair : indexes) {
      if (pair.second >= elementsToRemove) {
        updatedIndexes[pair.first] = pair.second - elementsToRemove;
      }
    }
    // Swap the updated map with the old one
    indexes.swap(updatedIndexes);
  }
  if (DEBUG) {
    std::cout << "Front elements removed" << std::endl;
    std::cout << "Indexes size after removing: " << indexes.size() << std::endl;
  }
}

long DynamicBuffer::minKey() const {
  if (indexes.empty()) {
    std::cerr << "Error: No data available." << std::endl;
    return -1;
  }
  return indexes.begin()->first;
}
long DynamicBuffer::maxKey() const {
  if (indexes.empty()) {
    std::cerr << "Error: No data available." << std::endl;
    return -1;
  }
  return indexes.rbegin()->first;
}

size_t DynamicBuffer::getNumRows() const { return indexes.size(); }

bool DynamicBuffer::hasEnoughRoomForNewRecord() {
  if (DEBUG) {
    std::cout << "In hasEnoughRoom.. indexes size : " << indexes.size()
              << " pid is : " << getpid() << std::endl;
  }

  if (indexes.size() < (DEFAULT_BUFFER_LENGTH_FACTOR * windowSize))
    return true;

  if (DEBUG) {
    std::cout << "Buffer is full, removing oldest record" << std::endl;
  }

  return false;
}

void DynamicBuffer::removeZeroCount() {
  int nZeros = countSubsequentZerosCounters();
  if (DEBUG) {
    std::cout << "Removing " << nZeros << " zero counters" << std::endl;
  }

  if (nZeros > 0) {
    removeFront(nZeros);
  }
}

size_t DynamicBuffer::countSubsequentZerosCounters() {
  size_t count = 0;
  for (auto value : counters) {
    if (value == 0) {
      ++count;
    } else {
      break;
    }
  }
  return count;
}

void DynamicBuffer::decrementCounters(const std::vector<long> &timestamps) {
  for (long timestamp : timestamps) {
    auto it = indexes.find(timestamp);
    if (it != indexes.end()) {
      size_t index = it->second;
      size_t counterIndex = index / nVariables;
      if (counterIndex < counters.size() && counters[counterIndex] > 0) {
        counters[counterIndex]--;
      }
    }
  }
}

void DynamicBuffer::printCounters() const {
  std::cout << "Counters: ";
  for (auto value : counters) {
    std::cout << value << " ";
  }
  std::cout << std::endl;
}

std::vector<int> DynamicBuffer::getCounters() const { return counters; }
