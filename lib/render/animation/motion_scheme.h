#pragma once

#include <optional>
#include <string_view>

enum class MotionScheme {
    Standard,
    Expressive,
};

enum class SpringToken {
    SpatialFast,
    SpatialDefault,
    SpatialSlow,
    EffectsFast,
    EffectsDefault,
    EffectsSlow,
};

struct SpringSpec {
    float dampingRatio;
    float stiffness;
};

constexpr MotionScheme kDefaultMotionScheme = MotionScheme::Expressive;

class SpringTokenResolver {
public:
    explicit SpringTokenResolver(MotionScheme scheme = kDefaultMotionScheme) noexcept;

    MotionScheme scheme() const noexcept;
    void setScheme(MotionScheme scheme) noexcept;

    SpringSpec resolve(SpringToken token) const noexcept;
    std::optional<SpringSpec> tryResolve(std::string_view configKey) const noexcept;

    static std::optional<MotionScheme> parseScheme(std::string_view name) noexcept;
    static std::optional<SpringToken> parseConfigKey(std::string_view configKey) noexcept;
    static std::string_view toString(MotionScheme scheme) noexcept;
    static std::string_view toString(SpringToken token) noexcept;
    static std::string_view toInternalId(MotionScheme scheme, SpringToken token) noexcept;

private:
    MotionScheme scheme_;
};
