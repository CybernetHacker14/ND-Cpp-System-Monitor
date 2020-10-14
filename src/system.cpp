#include "system.h"

#include <unistd.h>

#include <cstddef>
#include <iostream>
#include <set>
#include <string>
#include <vector>

#include "linux_parser.h"
#include "processh.h"
#include "processor.h"

// Returns the system's CPU
Processor& System::Cpu() { return cpu_; }

// Returns the system's processes
std::vector<Process>& System::Processes() {
  std::vector<int> processes = LinuxParser::Pids();
  for (int i = 0; i < processes.size(); i++) {
    processes_.push_back(Process(processes[i]));
  }
  std::sort(processes_.begin(), processes_.end());
  return processes_;
}

// Returns the system's kernel identifier
std::string System::Kernel() { return LinuxParser::Kernel(); }

// Returns the system's memory utilization
float System::MemoryUtilization() { return LinuxParser::MemoryUtilization(); }

// Returns the operating system name
std::string System::OperatingSystem() { return LinuxParser::OperatingSystem(); }

// Returns the number of processes actively running on the system
int System::RunningProcesses() { return LinuxParser::RunningProcesses(); }

// Returns the total number of processes on the system
int System::TotalProcesses() { return LinuxParser::TotalProcesses(); }

// Returns the number of seconds since the system started running
long int System::UpTime() { return LinuxParser::UpTime(); }