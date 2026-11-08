# 5. Work breakdown — P2

Full table (all tiers) lives in `m3e/spec.md` §5. This file is P2 only.

| ID | Requirement | Target | Acceptance | Tier | Effort |
|---|---|---|---|---|---|
| COL-003 | Tonal elevation on panels, OSD, notifications (`surface_container_*`) | src/shell panels | Hierarchy without borders | P2 | M |
| SHP-003 | ShapeName enum (35) + r(theta) profiles + rect-program uniforms | lib/render/core, programs | All 35 render; VG-02 passed | P2 | L |
| SHP-004 | Star-convex validation, origin offset or radii fallback per name | shape program | Per-name result documented | P2 | M |
| SHP-005 | fwidth AA for polar profiles | shape program | No stair-stepping on pixel-triangle/boom | P2 | M |
| SHP-006 | ShapeController mixin | lib/ui/controls | Widgets never touch shader | P2 | M |
| SHP-007 | Tier-1 widget wiring via `[widget.<name>]` fields | src/shell, config | Tier-1 morph, tier-2 unaffected | P2 | L |
| CMP-001 | Connected button group | lib/ui/controls | Shared shape, connected corners, press morph | P2 | M |
| CMP-002 | Split button | lib/ui/controls | Chevron morph on menu open | P2 | M |
| CMP-003 | Wavy progress; needs a new generic sine-stroke primitive in `lib/render/programs/` | render + control | Determinate and indeterminate modes, effects tokens | P2 | L |
