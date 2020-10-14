#include "format.h"

#include <iomanip>
#include <sstream>
#include <string>

// Converts seconds into HH::MM::SS format
std::string Format::ElapsedTime(long seconds) {
  int hour, min, sec;
  hour = seconds / 3600;
  min = (seconds % 3600) / 60;
  sec = (seconds % 3600) % 60;

  return FillSpaces(hour) + FillSpaces(min) + FillSpaces(sec, false);
}

// Helper function to fill empty spaces with 0s
std::string Format::FillSpaces(int value, bool addSemicolon) {
  std::ostringstream stream;
  stream << std::setw(2) << std::setfill('0') << std::to_string(value)
         << (addSemicolon ? ':' : ' ');
  return stream.str();
}