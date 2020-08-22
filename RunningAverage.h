#pragma once
//
//    FILE: RunningAverage.h
//  AUTHOR: Rob.Tillaart@gmail.com
// VERSION: 0.3.1
//    DATE: 2016-dec-01
// PURPOSE: Arduino library to calculate the running average by means of a circular buffer
//     URL: https://github.com/RobTillaart/RunningAverage
//
// HISTORY: See RunningAverage.cpp
//

#define RUNNINGAVERAGE_LIB_VERSION "0.3.1"

#include "Arduino.h"

class RunningAverage
{
public:
  RunningAverage(void) {};
  explicit RunningAverage(const uint8_t size);
  ~RunningAverage();

  void    clear();
  void    addValue(const double);
  void    fillValue(const double, const uint8_t);
  double getValue(const uint8_t);

  double getAverage();      // iterates over all elements.
  double getFastAverage() const;  // reuses previous calculated values.

  // return statistical characteristics of the running average
  double getStandardDeviation() const;
  double getStandardError() const;

  // returns min/max added to the data-set since last clear
  double   getMin() const { return _min; };
  double   getMax() const { return _max; };

  // returns min/max from the values in the internal buffer
  double   getMinInBuffer() const;
  double   getMaxInBuffer() const;

  // return true if buffer is full
  bool    bufferIsFull() const { return _cnt == _size; };

  double   getElement(uint8_t idx) const;

  uint8_t getSize() const { return _size; }
  uint8_t getCount() const { return _cnt; }


protected:
  uint8_t _size;
  uint8_t _cnt;
  uint8_t _idx;
  double   _sum;
  double*  _ar;
  double   _min;
  double   _max;
};

// -- END OF FILE --
