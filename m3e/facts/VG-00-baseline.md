# VG-00 — baseline

Verdict: pass
Baseline commit: 8be68ffd6
Date: 2026-11-08

## Evidence
- Branch: m3e, tracking fork/m3e
- Rebased on origin/main at 76637f9d4
- ARCHITECTURE.md present
- `lib/render/animation/animation_manager.h` present
- `lib/render/animation/motion_service.h` present
- `lib/render/programs/` present
- `lib/render/core/` present
- `lib/ui/controls/` present
- `src/config/config_types.*` present
- `lib/render/text/glyph_registry.cpp` present
- `src/scripting/luau_host.cpp` present

## Notes
Baseline recorded post-rebase. All M1-critical components present.

## Amendment (2026-11-17)
Fork head is now d10812ce1 on branch `m3e` (tracking fork/m3e). Upstream merge base with origin/main remains 76637f9d4. Re-run VG-00 after each upstream rebase; component names above are unchanged.

---
