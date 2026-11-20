#pragma once

#include <string>

namespace vespertine::launcher {

  struct DmenuSocketPathResult {
    std::string path;
    std::string error;
  };

  DmenuSocketPathResult resolveDmenuSocketPath();

} // namespace vespertine::launcher
