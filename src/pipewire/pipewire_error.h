#pragma once

#include <cstdint>

namespace vespertine::pipewire {

  enum class ErrorDisposition : std::uint8_t {
    Reconnect,
    StaleObject,
    Report,
  };

  ErrorDisposition classifyError(std::uint32_t objectId, int result) noexcept;

} // namespace vespertine::pipewire
