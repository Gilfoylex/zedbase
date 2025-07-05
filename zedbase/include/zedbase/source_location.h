#pragma once

#include <cstddef>

namespace zedbase {

class SourceLocation {
 public:
  SourceLocation(const char* file_, size_t line_) : file(file_), line(line_) {}

  const char* file;
  size_t line;
};

}  // namespace zedbase

#define FROM_HERE          \
  ::zedbase::SourceLocation { \
    __FILE__, __LINE__     \
  }