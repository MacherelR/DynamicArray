#ifndef DYNAMIC_COUNTER_H
#define DYNAMIC_COUNTER_H

#include "DynamicArrayCython.h"

class DynamicCounterCython : public DynamicArrayCython {
public:
  DynamicCounterCython();

  void updateCounterValue(long timestamp, int diff);
};

#endif // DYNAMIC_COUNTER_H
