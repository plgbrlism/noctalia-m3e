#pragma once

#include "core/application_identity.h"

// The shell's library identity: ~/.config/vespertine, vespertine.log, VESPERTINE_* environment variables.
inline ApplicationIdentity shellApplicationIdentity() { return {.name = "vespertine", .envPrefix = "VESPERTINE"}; }
