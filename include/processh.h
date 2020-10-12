#ifndef PROCESSH_H
#define PROCESSH_H

#include <string>
/*
Basic class for Process representation
It contains relevant attributes as shown below

Renamed this file to processh.h cause it was conflicting with process.h found in
Windows Kit
*/
class Process {
 public:
  Process(int pid);

  int Pid();                               // TODO: See src/process.cpp
  std::string User();                      // TODO: See src/process.cpp
  std::string Command();                   // TODO: See src/process.cpp
  float CpuUtilization();                  // TODO: See src/process.cpp
  std::string Ram();                       // TODO: See src/process.cpp
  long int UpTime();                       // TODO: See src/process.cpp
  bool operator<(Process const& a) const;  // TODO: See src/process.cpp

  // TODO: Declare any necessary private members
 private:
  int _pid = 0;
  std::string _user;
  std::string _cmd;
  float _cpuUtilization;
  std::string _ram;
  long int _upTime;
};

#endif