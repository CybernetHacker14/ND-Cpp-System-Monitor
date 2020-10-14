#ifndef FORMAT_H
#define FORMAT_H

#include <string>

// Function decalrations used to define supplied
// seconds value in a HH::MM::SS format
namespace Format {
std::string ElapsedTime(long times);
std::string FillSpaces(int timeValue, bool addSemicolon = true);
};  // namespace Format

#endif