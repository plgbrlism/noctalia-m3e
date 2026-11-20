#pragma once

#include "cli/schema.h"

#include <string>
#include <string_view>

namespace vespertine::cli {

  std::string renderHelp(const Command& command, std::string_view path);
  std::string renderArgsSpec(const Command& command);

} // namespace vespertine::cli
