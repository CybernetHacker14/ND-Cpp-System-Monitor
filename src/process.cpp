#include <unistd.h>

#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"
#include "processh.h"

Process::Process(int pid) { _pid = pid; }

// TODO: Return this process's ID
int Process::Pid() { return _pid; }

// TODO: Return this process's CPU utilization
float Process::CpuUtilization() {
  return (float)LinuxParser::ActiveJiffies(_pid) /
         (float)(LinuxParser::UpTime() - LinuxParser::UpTime(_pid));
}

// TODO: Return the command that generated this process
std::string Process::Command() {
  _cmd = LinuxParser::Command(_pid);
  return _cmd;
}

// TODO: Return this process's memory utilization
std::string Process::Ram() {
  _ram = LinuxParser::Ram(_pid);
  return _ram;
}

// TODO: Return the user (name) that generated this process
std::string Process::User() {
  _user = LinuxParser::User(_pid);
  return _user;
}

// TODO: Return the age of this process (in seconds)
long int Process::UpTime() {
  _upTime = LinuxParser::UpTime(_pid);
  return _upTime;
}

// TODO: Overload the "less than" comparison operator for Process objects
bool Process::operator<(Process const& a) const { return _ram < a._ram; }