//
// Created by Ryan McKay on 8/20/20.
//

#include "Buffer.h"
#include <vector>


Buffer::Buffer() {
    values.resize(100);
}

Buffer::Buffer(int numValues) {
    values.resize(numValues);
    maxLength = numValues;
}

Buffer::Buffer(int numValues, int smoothAmt) {
    values.resize(numValues);
    maxLength = numValues;
    smoothAmount = smoothAmt;
}

double Buffer::average() {
    if (values.size() > 10) {
        double sum = 0;
        for (int i : values) {
            sum += i;
        }
        return sum / static_cast<int>(values.size());
    }
    else return 0.0;
}

void Buffer::add(int value) {
    if (values.size() >= maxLength) {
        values.erase(values.begin());
    }
    values.push_back(value);
}