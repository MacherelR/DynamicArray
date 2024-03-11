#include "DynamicCounterCython.h"
#include "DynamicArrayCython.h"

DynamicCounterCython::DynamicCounterCython() : DynamicArrayCython(1) {}

void DynamicCounterCython::updateCounterValue(long timestamp, int diff) {
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
