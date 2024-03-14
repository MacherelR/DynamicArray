#ifndef DYNAMIC_COUNTER_H
#define DYNAMIC_COUNTER_H

#include "DynamicBuffer.h"
#include "constants.h"

class DynamicCounter : public DynamicBuffer {
public:
  DynamicCounter(size_t windowSize);

  void updateCounterValue(long timestamp, int diff);

  size_t countSubsequentZeros() const;
};

#endif // DYNAMIC_COUNTER_H
