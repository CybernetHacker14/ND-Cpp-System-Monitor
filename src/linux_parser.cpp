#include "linux_parser.h"

#include <dirent.h>
#include <unistd.h>

#include <filesystem>
#include <iostream>
#include <string>
#include <type_traits>
#include <vector>

#include "processor.h"

// Parses data regarding Operating system
std::string LinuxParser::OperatingSystem() {
  std::string line, key, value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  filestream.close();
  return value;
}

// Parses data regarding Kernel
std::string LinuxParser::Kernel() {
  std::string os, version, kernel, line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  stream.close();
  return kernel;
}

// Parses directories and returns a vector of all processes
std::vector<int> LinuxParser::Pids() {
  std::vector<int> pids;
  for (const auto& entry :
       std::filesystem::directory_iterator(kProcDirectory.c_str())) {
    auto file = entry.path().filename().string();
    if (std::filesystem::is_directory(entry)) {
      if (std::all_of(file.begin(), file.end(), isdigit)) {
        pids.push_back(std::stoi(file));
      }
    }
  }
  return pids;
}

// Parses data and returns the memory utilization
float LinuxParser::MemoryUtilization() {
  std::string line, key, value;
  float totalMemory, freeMemory, buffers;
  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::stringstream linestream(line);
      linestream >> key >> value;
      if (key == "MemTotal:") {
        totalMemory = std::stof(value);
      }
      if (key == "MemFree:") {
        freeMemory = std::stof(value);
      }
      if (key == "Buffers:") {
        buffers = std::stof(value);
        linestream.clear();
        break;  // Remaining entries aren't needed
      }
    }
    // This needs to be outside the above while loop, so that both the
    // totalMemory and freeMemory values get populated before the calculation
    return 1.0 - (freeMemory / (totalMemory - buffers));
  }
  stream.close();
  return 0.0;
}

// Returns the up-time of the system
long LinuxParser::UpTime() {
  std::string line;
  long upTime;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::stringstream linestream(line);
    linestream >> upTime;
  }
  stream.close();
  return upTime;
}

// Returns the total jiffies of the system
long LinuxParser::Jiffies() { return ActiveJiffies() + IdleJiffies(); }

// Returns the active jiffies for a process
long LinuxParser::ActiveJiffies(int pid) {
  std::string line, value;
  std::vector<std::string> data;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);

  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      std::istream_iterator<std::string> begin(linestream), end;
      std::vector<std::string> values(begin, end);
      return (std::stof(values[13]) + std::stof(values[14]) +
              std::stof(values[15]) + std::stof(values[16])) /
             sysconf(_SC_CLK_TCK);
    }
  }
  stream.close();
  return 0;
}

// Returns the active jiffies of the system
long LinuxParser::ActiveJiffies() {
  return std::stof(LinuxParser::CpuUtilization()[kUser_]) +
         std::stof(LinuxParser::CpuUtilization()[kNice_]) +
         std::stof(LinuxParser::CpuUtilization()[kSystem_]) +
         std::stof(LinuxParser::CpuUtilization()[kIRQ_]) +
         std::stof(LinuxParser::CpuUtilization()[kSoftIRQ_]) +
         std::stof(LinuxParser::CpuUtilization()[kGuest_]) +
         std::stof(LinuxParser::CpuUtilization()[kGuestNice_]);
}

// Returns the idle jiffies of the system
long LinuxParser::IdleJiffies() {
  return std::stof(LinuxParser::CpuUtilization()[kIOwait_]) +
         std::stof(LinuxParser::CpuUtilization()[kIdle_]);
}

// Returns the CPU utilization
std::vector<std::string> LinuxParser::CpuUtilization() {
  std::string line;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::stringstream linestream(line);
      if (line.find("cpu") == 0) {
        std::istream_iterator<std::string> begin(linestream), end;
        begin++;
        std::vector<std::string> cpuData(begin, end);
        return cpuData;
      }
    }
  }
  stream.close();
  return std::vector<std::string>();
}

// Returns the total number of processes
int LinuxParser::TotalProcesses() {
  std::string line, key, value;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "processes") {
          return std::stoi(value);
        }
      }
    }
  }
  stream.close();
  return std::stoi(value);
}

// Returns the total number of running processes
int LinuxParser::RunningProcesses() {
  std::string line, key, value;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "procs_running") {
          return std::stoi(value);
        }
      }
    }
  }
  stream.close();
  return std::stoi(value);
}

// Returns the command associated with a process
std::string LinuxParser::Command(int pid) {
  std::string line;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kCmdlineFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
  }
  stream.close();
  return line;
}

// Returns the memory used by a process
std::string LinuxParser::Ram(int pid) {
  std::string line, key, value;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "VmSize:") {
          int size = int(std::stof(value) / float(1024));
          return std::to_string(size);
        }
      }
    }
  }
  stream.close();
  return std::string();
}

// Returns the User ID associated with a process
std::string LinuxParser::Uid(int pid) {
  std::string line;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      if (line.find("Uid:") == 0) {
        std::istream_iterator<std::string> begin(linestream), end;
        std::vector<std::string> data(begin, end);
        return data[2];
      }
    }
  }
  stream.close();
  return std::string();
}

// Returns the user associated with the process
std::string LinuxParser::User(int pid) {
  std::string line, key, value;
  std::ifstream stream(kPasswordPath);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    if (line.find("x:" + Uid(pid)) != line.npos) {
      return line.substr(0, line.find(":"));
    }
  }
  stream.close();
  return std::string();
}

// Returns the uptime of the process
long LinuxParser::UpTime(int pid) {
  std::string line;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);

  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream stream(line);
    std::istream_iterator<std::string> begin(stream), end;
    std::vector<std::string> values(begin, end);
    return long(std::stof(values[21]) / sysconf(_SC_CLK_TCK));
  }
  stream.close();
  return 0;
}