#include "DynamicCounter.h"
#include "DynamicBuffer.h"
#include <stdexcept>

DynamicCounter::DynamicCounter(size_t windowSize)
    : DynamicBuffer(1, windowSize) {}

void DynamicCounter::updateCounterValue(long timestamp, int diff) {
  size_t dataIndex;
  auto it = indexes.find(timestamp);
  auto nextTimestampIt = indexes.end();

  if (it != indexes.end()) {
    dataIndex = it->second;
    if (dataIndex < bufferLength) {
      this->data[dataIndex] += diff;
      this->data[dataIndex] < 0 ? this->data[dataIndex] = 0
                                : this->data[dataIndex];
    } else {
      throw std::out_of_range("Attempting to write beyond buffer length");
    }
  } else {
    if (indexes.empty() || timestamp > indexes.rbegin()->first) {
      dataIndex = (!indexes.empty()) ? indexes.rbegin()->second + 1 : 0;
    } else {
      nextTimestampIt = indexes.upper_bound(timestamp);
      dataIndex =
          (nextTimestampIt != indexes.end()) ? nextTimestampIt->second : 0;
    }

    // Update indexes and data
    indexes[timestamp] = dataIndex;

    if (dataIndex < bufferLength) {
      this->data[dataIndex] = diff;
      this->data[dataIndex] < 0 ? this->data[dataIndex] = 0
                                : this->data[dataIndex];

      if (nextTimestampIt != indexes.end()) {
        for (auto updateIt = nextTimestampIt; updateIt != indexes.end();
             ++updateIt) {
          updateIt->second++;
        }
      }
    } else {
      throw std::out_of_range("Attempting to write beyond buffer length");
    }
  }
}

size_t DynamicCounter::countSubsequentZeros() const {
  size_t count = 0;
  for (auto value : data) {
    if (value == 0) {
      ++count;
    } else {
      break;
    }
  }
  return count;
}
