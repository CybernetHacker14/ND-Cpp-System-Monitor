#include "processor.h"

#include "linux_parser.h"

// DONE: Return the aggregate CPU utilization
float Processor::Utilization() {
  totalDelta = LinuxParser::Jiffies() - prevTotal;
  idleDelta = LinuxParser::IdleJiffies() - prevIdle;

  prevTotal = LinuxParser::Jiffies();
  prevIdle = LinuxParser::IdleJiffies();

  return (totalDelta - idleDelta) / totalDelta;
}