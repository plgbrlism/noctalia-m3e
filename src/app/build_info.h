#pragma once

#include <string>
#include <string_view>

namespace vespertine::build_info {

  std::string_view version() noexcept;

  std::string_view revision() noexcept;

  std::string displayVersion();

} // namespace vespertine::build_info
