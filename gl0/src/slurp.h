#pragma once
#include "headers.h"
namespace slurp {
  std::stringstream GetFileContents(const std::string);

  bool CheckFileExist(const std::string path,
    const std::string fname,
    const std::string ext);
} // namespace slurp
