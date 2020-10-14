#ifndef PROCESSH_H
#define PROCESSH_H

#include <string>

// Basic class for Process representation
// It contains relevant attributes as shown below

// Renamed this file to processh.h cause it was conflicting with process.h found
// in Windows Kit
class Process {
 public:
  Process(int pid);

  int Pid();
  std::string User();
  std::string Command();
  float CpuUtilization();
  std::string Ram();
  long int UpTime();
  bool operator<(Process const& a) const;

 private:
  int _pid = 0;
  std::string _user;
  std::string _cmd;
  float _cpuUtilization;
  std::string _ram;
  long int _upTime;
};

#endif