#ifndef FORMAT_H
#define FORMAT_H

#include <string>

namespace Format {
std::string ElapsedTime(long times);
std::string FillSpaces(int timeValue, bool addSemicolon = true);
}; // namespace Format

#endif