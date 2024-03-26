#include <algorithm> // For std::find_if
#include <iostream>
#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>
#include <sstream>
#include <vector>

class DynamicCounter : public DynamicArray {
public:
  DynamicCounter() : DynamicArray(1) {}

  void updateCounterValue(long timestamp, int diff) {
    auto it = data.find(timestamp);
    if (it != data.end()) {
      it->second[0] += diff;
      if (it->second[0] < 0) {
        it->second[0] = 0;
      }
    } else {
      std::cerr << "Error: Timestamp not found." << std::endl;
    }
  }
};
