#include "linux_parser.h"

#include <dirent.h>
#include <unistd.h>

#include <filesystem>
#include <iostream>
#include <string>
#include <type_traits>
#include <vector>

#include "processor.h"

// DONE: An example of how to read data from the filesystem
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

// DONE: An example of how to read data from the filesystem
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

// DONE: Update this to use std::filesystem
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

// DONE: Read and return the system memory utilization
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

// DONE: Read and return the system uptime
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

// DONE: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { return ActiveJiffies() + IdleJiffies(); }

// TODO: Read and return the number of active jiffies for a PID
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

// DONE: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() {
  return std::stof(LinuxParser::CpuUtilization()[kUser_]) +
         std::stof(LinuxParser::CpuUtilization()[kNice_]) +
         std::stof(LinuxParser::CpuUtilization()[kSystem_]) +
         std::stof(LinuxParser::CpuUtilization()[kIRQ_]) +
         std::stof(LinuxParser::CpuUtilization()[kSoftIRQ_]) +
         std::stof(LinuxParser::CpuUtilization()[kGuest_]) +
         std::stof(LinuxParser::CpuUtilization()[kGuestNice_]);
}

// DONE: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  return std::stof(LinuxParser::CpuUtilization()[kIOwait_]) +
         std::stof(LinuxParser::CpuUtilization()[kIdle_]);
}

// DONE: Read and return CPU utilization
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

// DONE: Read and return the total number of processes
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

// DONE: Read and return the number of running processes
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

// TODO: Read and return the command associated with a process
std::string LinuxParser::Command(int pid) {
  std::string line;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kCmdlineFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
  }
  stream.close();
  return line;
}

// TODO: Read and return the memory used by a process
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

// TODO: Read and return the user ID associated with a process
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

// TODO: Read and return the user associated with a process
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

// DONE: Read and return the uptime of a process
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