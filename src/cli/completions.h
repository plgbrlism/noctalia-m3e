#pragma once

#include "cli/schema.h"

#include <string>

namespace vespertine::cli {

  std::string generateBash(const Command& root);
  std::string generateZsh(const Command& root);
  std::string generateFish(const Command& root);

  int runCompletionsCli(int argc, char* argv[]);

} // namespace vespertine::cli
