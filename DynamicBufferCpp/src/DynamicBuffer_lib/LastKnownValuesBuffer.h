//
// Created by Remy Macherel on 26.03.2024.
//

#ifndef LASTKNOWNVALUESBUFFER_H
#define LASTKNOWNVALUESBUFFER_H
#include "DynamicBuffer.h"

class LastKnownValuesBuffer : public DynamicBuffer {
public:
    LastKnownValuesBuffer(size_t nVariables, size_t windowSize);

    // Method added as it should have some specific behavior
    bool updateLastKnownValue(long timestamp, size_t columnIndex, double value);
};

#endif //LASTKNOWNVALUESBUFFER_H
