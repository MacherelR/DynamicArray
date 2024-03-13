#ifndef DYNAMIC_ARRAY_CYTHON_H
#define DYNAMIC_ARRAY_CYTHON_H

#include <algorithm> // For std::find_if
#include <iostream>
#include <map>
#include <sstream>
#include <vector>

class DynamicArrayCython {
protected:
  std::map<long, std::vector<double>> data; // 2D array for storing rows of data
  size_t cols;                             // Number of columns, fixed

public:
  DynamicArrayCython(size_t columns);

  void deleteRow(long timestamp);
  void removeFirstElement();
  void deleteRange(long timestamp, int numberOfValues);
  void deleteRowById(int index);

  bool timestampExists(long timestamp) const;
  size_t getNumRows() const;
  void print() const;

  long minKey() const;
  long maxKey() const;

  void addOrUpdateRow(long timestamp, size_t column_index, double value);

  std::vector<double> getRow(long timestamp);
  std::vector<double> getRowByIndex(int index);
  std::vector<std::vector<double>> getSlice(long timestamp, size_t numItems);
  std::vector<long> getTimestamps() const;
  std::vector<double> getFlattenedSlice(long timestamp, size_t numItems);
};

#endif // DYNAMIC_ARRAY_CYTHON_H
