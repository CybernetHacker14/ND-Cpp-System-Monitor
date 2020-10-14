#ifndef PROCESSOR_H
#define PROCESSOR_H

// Function declarations and necessary members to calculate processor
// utilization
class Processor {
 public:
  float Utilization();

 private:
  float totalDelta, idleDelta, prevTotal, prevIdle;
};

#endif