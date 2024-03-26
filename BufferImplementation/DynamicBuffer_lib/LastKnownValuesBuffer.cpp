//
// Created by Remy Macherel on 26.03.2024.
//

#include "LastKnownValuesBuffer.h"
#include "DynamicBuffer.h"

const bool DEBUG = false;

LastKnownValuesBuffer::LastKnownValuesBuffer(size_t nVariables, size_t windowSize) : DynamicBuffer(
  nVariables, windowSize) {
}

bool LastKnownValuesBuffer::updateLastKnownValue(long timestamp, size_t columnIndex, double value) {
  bool newEntry = true;
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

  if (it != indexes.end()) {
    newEntry = false;
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

    if (indexes.empty() || timestamp > indexes.rbegin()->first) {
      dataIndex =
          (!indexes.empty()) ? (indexes.rbegin()->second + nVariables) : 0;
    } else {
      auto nextTimestampIt = indexes.upper_bound(timestamp);
      dataIndex =
          (nextTimestampIt != indexes.end()) ? nextTimestampIt->second : 0;

      // Shift existing data to make room for the new record
      std::move_backward(data.begin() + dataIndex, data.end() - nVariables,
                         data.end());

      // Update indexes for all timestamps after the new one
      for (auto iter = nextTimestampIt; iter != indexes.end(); ++iter) {
        iter->second += nVariables;
      }
    }
    // Insert the new value
    size_t previousRowIndex = dataIndex == 0 ? 0 : (dataIndex - nVariables);
    for (size_t i = 0; i < nVariables; ++i) {
      data[dataIndex + i] = data[previousRowIndex + i];
    }
    // std::fill_n(data.begin() + dataIndex, nVariables,
    //             std::nan("")); // Prepare space for new data
    data[dataIndex + columnIndex] = value; // Insert new value at the correct column

    // Update the index for the new timestamp
    indexes[timestamp] = dataIndex;

    // Update the counters appropriately
    size_t rowIndex = dataIndex / nVariables;
    counters.resize(std::max(counters.size(), rowIndex + 1),
                    0); // Ensure counters vector is large enough
    counters[rowIndex] = 1;
  }
  if (DEBUG) {
    std::cout << "Value " << value << " at timestamp " << timestamp
        << " inserted !" << std::endl;
  }

  return newEntry;
}
