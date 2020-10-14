#include <unistd.h>

#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"
#include "processh.h"

Process::Process(int pid) { _pid = pid; }

// Returns this process's id
int Process::Pid() { return _pid; }

// Returns this process's CPU utilization
float Process::CpuUtilization() {
  return (float)LinuxParser::ActiveJiffies(_pid) /
         (float)(LinuxParser::UpTime() - LinuxParser::UpTime(_pid));
}

// Returns the command that generated this process
std::string Process::Command() {
  _cmd = LinuxParser::Command(_pid);
  return _cmd;
}

// Returns this process's memory utilization
std::string Process::Ram() {
  _ram = LinuxParser::Ram(_pid);
  return _ram;
}

// Returns the user (name) that generated this process
std::string Process::User() {
  _user = LinuxParser::User(_pid);
  return _user;
}

// Returns the age of this process (in seconds)
long int Process::UpTime() {
  _upTime = LinuxParser::UpTime(_pid);
  return _upTime;
}

// Overload the "less than" comparison operator for Process objects
bool Process::operator<(Process const& a) const { return _ram < a._ram; }