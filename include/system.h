#ifndef SYSTEM_H
#define SYSTEM_H

#include <string>
#include <vector>

#include "processh.h"
#include "processor.h"

// Class for collecting and storing system data and passing to ncurses_display
// for displaying the resulting output
class System {
 public:
  Processor& Cpu();
  std::vector<Process>& Processes();
  float MemoryUtilization();
  long UpTime();
  int TotalProcesses();
  int RunningProcesses();
  std::string Kernel();
  std::string OperatingSystem();

 private:
  Processor cpu_ = {};
  std::vector<Process> processes_ = {};
};

#endif