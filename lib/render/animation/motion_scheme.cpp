#include "render/animation/motion_scheme.h"

#include <array>
#include <cstddef>

namespace {

constexpr std::size_t schemeIndex(MotionScheme scheme) noexcept
{
    return scheme == MotionScheme::Standard ? 0U : 1U;
}

constexpr std::size_t tokenIndex(SpringToken token) noexcept
{
    return static_cast<std::size_t>(token);
}

constexpr std::array<std::string_view, 6> kConfigKeys = {
    "spatial.fast",
    "spatial.default",
    "spatial.slow",
    "effects.fast",
    "effects.default",
    "effects.slow",
};

constexpr std::array<std::array<SpringSpec, 6>, 2> kSprings = {{
    {{
        {0.9F, 1400.0F},
        {0.9F, 700.0F},
        {0.9F, 300.0F},
        {1.0F, 3800.0F},
        {1.0F, 1600.0F},
        {1.0F, 800.0F},
    }},
    {{
        {0.6F, 800.0F},
        {0.8F, 380.0F},
        {0.8F, 200.0F},
        {1.0F, 3800.0F},
        {1.0F, 1600.0F},
        {1.0F, 800.0F},
    }},
}};

constexpr std::array<std::array<std::string_view, 6>, 2> kInternalIds = {{
    {{
        "M3E-STANDARD-SPATIAL-FAST",
        "M3E-STANDARD-SPATIAL-DEFAULT",
        "M3E-STANDARD-SPATIAL-SLOW",
        "M3E-STANDARD-EFFECTS-FAST",
        "M3E-STANDARD-EFFECTS-DEFAULT",
        "M3E-STANDARD-EFFECTS-SLOW",
    }},
    {{
        "M3E-EXPRESSIVE-SPATIAL-FAST",
        "M3E-EXPRESSIVE-SPATIAL-DEFAULT",
        "M3E-EXPRESSIVE-SPATIAL-SLOW",
        "M3E-EXPRESSIVE-EFFECTS-FAST",
        "M3E-EXPRESSIVE-EFFECTS-DEFAULT",
        "M3E-EXPRESSIVE-EFFECTS-SLOW",
    }},
}};

} // namespace

SpringTokenResolver::SpringTokenResolver(MotionScheme scheme) noexcept
    : scheme_(scheme)
{
}

MotionScheme SpringTokenResolver::scheme() const noexcept
{
    return scheme_;
}

void SpringTokenResolver::setScheme(MotionScheme scheme) noexcept
{
    scheme_ = scheme;
}

SpringSpec SpringTokenResolver::resolve(SpringToken token) const noexcept
{
    return kSprings[schemeIndex(scheme_)][tokenIndex(token)];
}

std::optional<SpringSpec> SpringTokenResolver::tryResolve(std::string_view configKey) const noexcept
{
    const auto token = parseConfigKey(configKey);
    if (!token) {
        return std::nullopt;
    }
    return resolve(*token);
}

std::optional<MotionScheme> SpringTokenResolver::parseScheme(std::string_view name) noexcept
{
    if (name == "standard") {
        return MotionScheme::Standard;
    }
    if (name == "expressive") {
        return MotionScheme::Expressive;
    }
    return std::nullopt;
}

std::optional<SpringToken> SpringTokenResolver::parseConfigKey(std::string_view configKey) noexcept
{
    for (std::size_t i = 0; i < kConfigKeys.size(); ++i) {
        if (configKey == kConfigKeys[i]) {
            return static_cast<SpringToken>(i);
        }
    }
    return std::nullopt;
}

std::string_view SpringTokenResolver::toString(MotionScheme scheme) noexcept
{
    return scheme == MotionScheme::Standard ? "standard" : "expressive";
}

std::string_view SpringTokenResolver::toString(SpringToken token) noexcept
{
    return kConfigKeys[tokenIndex(token)];
}

std::string_view SpringTokenResolver::toInternalId(MotionScheme scheme, SpringToken token) noexcept
{
    return kInternalIds[schemeIndex(scheme)][tokenIndex(token)];
}
