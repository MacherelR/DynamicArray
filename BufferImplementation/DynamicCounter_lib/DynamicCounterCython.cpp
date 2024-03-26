#include "DynamicCounterCython.h"
#include "DynamicArrayCython.h"
#include <vector>

DynamicCounterCython::DynamicCounterCython() : DynamicArrayCython(1) {}

void DynamicCounterCython::updateCounterValue(long timestamp, int diff) {
  std::vector<double> &counter = data[timestamp];
  if (counter.empty()) {
    counter.push_back(0); // Initialize to zero if not found
  }
  counter[0] += diff;

  if (counter[0] < 0) { // Ensure counter is non-negative
    counter[0] = 0;
  }
}
