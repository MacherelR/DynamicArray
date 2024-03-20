#ifndef DYNAMIC_BUFFER_H
#define DYNAMIC_BUFFER_H

#include "constants.h"
#include <algorithm> // For std::find_if
#include <iostream>
#include <map>
#include <sstream>
#include <stdexcept>
#include <vector>

class DynamicBuffer {
protected:
  // map storing the indexes (adresses) of the rows
  std::map<long, size_t> indexes;
  size_t nVariables; // Number of columns (variables), fixed
  size_t windowSize; // Number of rows (time steps) to keep in memory
  size_t bufferLength;
  std::vector<double> data; // Array containing the values
  std::vector<int> counters;

public:
  DynamicBuffer(size_t nVariables, size_t windowSize);

  bool deleteRecord(long timestamp);

  void addOrUpdateRecord(long timestamp, size_t column_index, double value);

  double &at(size_t row, size_t col);

  const double &at(size_t row, size_t col) const;

  void print() const;

  std::vector<double> getRecordByTimestamp(long timestamp) const;
  std::vector<double> getRecordByIndex(size_t index) const;

  const double *getRecordByTimestampPtr(long timestamp, size_t &outSize) const;
  const double *getRecordByIndexPtr(size_t index) const;

  const double *getSlice(long timestamp, size_t N, size_t &outSize) const;
  std::vector<long> getSliceTimestamps(long timestamp, size_t N) const;

  std::vector<double> getSliceByTimestamp(long start, long end) const;
  std::vector<double> getSliceByIndex(size_t start, size_t end) const;

  size_t getNVariables() const;

  void removeFront(size_t removeCount);

  long minKey() const;
  long maxKey() const;
  size_t getNumRows() const;

  bool hasEnoughRoomForNewRecord();
  size_t countSubsequentZerosCounters();
  void removeZeroCount();
  void decrementCounters(const std::vector<long> &timestamps);

  void printCounters() const;
  std::vector<int> getCounters() const;
  void printIndexes() const;
};

#endif // DYNAMIC_BUFFER_H
