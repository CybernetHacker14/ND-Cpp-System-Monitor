#ifndef PROCESSOR_H
#define PROCESSOR_H

class Processor {
 public:
  float Utilization();  // TODO: See src/processor.cpp
 private:
  float totalDelta, idleDelta, prevTotal, prevIdle;
};

#endif