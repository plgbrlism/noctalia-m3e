# M3E Work Log

Append-only. One line per edit:

`[work-item-id] [file] [what] [test-status]`

Status: `pending-test` | `pass` | `fail` | `reverted`

---
[MOT-001] lib/render/animation/motion_scheme.h add MotionScheme/SpringToken API pass
[MOT-001] lib/render/animation/motion_scheme.cpp add spring table and resolver pass
[MOT-001] tests/motion_scheme_test.cpp add MOT-001 acceptance coverage pass
[MOT-001] meson.build register motion_scheme test pass
[MOT-001] lib/meson.build register motion_scheme.cpp source pass
[MOT-002] lib/render/animation/animation_manager.h add SpringState and animateSpring API pass
[MOT-002] lib/render/animation/animation_manager.cpp add closed-form spring tick pass
[MOT-002] lib/render/animation/animation_manager.cpp reentrancy-safe snapshot iteration pass
[MOT-002] tests/spring_test.cpp add MOT-002 acceptance coverage pass
[MOT-002] meson.build register spring test pass
[MOT-003] lib/render/animation/animation_manager.h add retargetSpringSpec API pass
[MOT-003] lib/render/animation/animation_manager.cpp add retargetSpringSpec impl pass
[MOT-003] tests/spring_test.cpp add target + spec retarget continuity tests pass
[MOT-005] lib/render/animation/animation_manager.h add FrameRequest/setFrameRequest/hasAnyActive pass
[MOT-005] lib/render/animation/animation_manager.cpp add requestFrame lifecycle pass
[MOT-005] tests/idle_test.cpp add idle harness pass
[MOT-005] meson.build register idle test pass
[CFG-001] lib/render/animation/motion_scheme.h add MotionConfig/kSpringTokenCount/config ctor pass
[CFG-001] lib/render/animation/motion_scheme.cpp add MotionConfig ctor/overlay resolve pass
[CFG-001] tests/config_motion_test.cpp add schema coverage pass
[CFG-001] meson.build register config_motion test pass
[CFG-001] src/app/application_services.cpp wire setMotionConfig into applyMotionConfig pass
[MOT-006] lib/render/animation/motion_scheme.cpp add isSpatialToken + reduce branch in resolve() pending-test
[MOT-006] tests/motion_scheme_test.cpp add AT-MOT-06 reduce cases + regression pass pending-test
[MOT-006] lib/render/animation/motion_scheme.cpp add isSpatialToken + reduce branch in resolve() pass
[MOT-006] tests/motion_scheme_test.cpp add AT-MOT-06 reduce cases + regression pass
[MOT-004] lib/render/animation/animation_manager.h add onComplete to SpringState + animateSpring signature pass
[MOT-004] lib/render/animation/animation_manager.cpp update animateSpring signature + fire onComplete on snap pass
[MOT-004] tests/spring_test.cpp update animateSpring call sites + add onComplete cases A-D pass
[MOT-004] tests/idle_test.cpp update animateSpring call sites (insert {}) pass
[MOT-004] lib/render/animation/motion_service.h add resolve(SpringToken) pass
[MOT-004] lib/render/animation/motion_service.cpp implement resolve(SpringToken) pass
[MOT-004] src/shell/bar/bar.cpp add motion_service.h include + migrate 7 animate() sites to animateSpring() pass
[MOT-004] lib/render/animation/animation_manager.cpp extend cancelForOwner to erase springs by owner pass
[MOT-004] tests/spring_test.cpp add cancelForOwner spring cases E-F pass
[MOT-004] lib/render/animation/animation_manager.cpp extend cancel/cancelAll to also erase springs pass
[MOT-004] tests/spring_test.cpp add cancel/cancelAll spring cases G-H pass
[MOT-004] src/shell/dock/dock_instance.cpp add motion_service.h include + migrate 3 animate() sites to animateSpring() pass
[MOT-004] src/shell/dock/dock_items.cpp add motion_service.h include + migrate 2 animate() sites to animateSpring() pass
[MOT-004] src/shell/tooltip/tooltip_manager.cpp add motion_service.h include + migrate 2 animate() sites to animateSpring() pass
[MOT-004] lib/render/animation/animation_manager.cpp uncomment deltaMs in tick(float deltaMs) pass
[MOT-004] lib/render/animation/animation_manager.cpp extend hasActive() to include hasActiveSprings() pass
[MOT-004] lib/render/animation/animation_manager.cpp drive tickSprings from tick(deltaMs) pass
[SHP-001] lib/ui/controls/button.{h,cpp} add press morph radii animation, clamp, scale, and test support pass
[SHP-001] tests/button_shape_test.cpp add AT-SHP-01 clamp and AT-SHP-02 fractional scale tests pass
[SHP-001] lib/wayland/surface.cpp and animation_manager.cpp wire spring tick and frame loop pass
[SHP-002] lib/ui/controls/button.cpp radii scale plumbing satisfied by SHP-001 (renderer.renderScale() in doLayout + m_cachedContentScale) pass
[SHP-002] tests/button_shape_test.cpp AT-SHP-02 covers 1.25x and 1.5x pass
[SHP-001] lib/ui/controls/button.cpp symmetric morph delta (±10px) so both extremes morph visibly pass
[SHP-001] lib/render/animation/motion_service.cpp clamp speed² stiffness multiplier to [0.25, 4.0] pass
[COL-001] lib/ui/controls/ audit — zero hardcoded color violations (3 Color{} hits are role-derived with alpha tweaks only) pass
[COL-001] src/theme/contrast.h — WCAG helpers already present, no new code required, AT-COL-01 satisfied by existing helper pass

## Git history appended (paul, since c4c25c8)
- ffe2cd703 2026-11-10 docs(m3e): rev6 Design System coordinator spec
- b056bdd31 2026-11-10 docs(m3e): rev7 DES-004 preset layer + DES-005 panel motion
- 934382179 2026-11-10 feat(m3e): DES-001+002 Design System Expressive|Custom
- eb1c89f1a 2026-11-10 feat(m3e): DES-004 preset layer
- d127eb777 2026-11-11 fix(m3e): configEqual persistence + Smooth mapping
- efbfd6ac7 2026-11-11 feat(m3e): DES-004 restructure — Expressive bool + 3-preset spring
- b4c9be656 2026-11-11 feat(m3e): DES-005 panel motion — Depth + Fluid, drop Directional

## Git history appended (since 90ce3703b)
- 457e40345 2026-11-11 feat(m3e): Phase G — Corner Style Material grouped-pill rows
- e1786ece0 2026-11-12 feat(m3e): UI-001 tooltip border toggle + UI-001b auto-elevation
- ef74d002e 2026-11-12 fix(m3e): remove segmented separators and fix tab/ghost palettes
- 5fa095f84 2026-11-12 feat(m3e): expand sidebar padding and use ghost buttons
- ce58b46fd 2026-11-12 feat(m3e): replace corner_radius_scale slider with corner_shape preset
- 6b3ed38d0 2026-11-12 fix(m3e): control center sidebar layout and native scrolling
- 2cb97fdc7 2026-11-14 refactor(motion): remove deprecated motion presets and reduce/ripple toggles
- ebb96345e 2026-11-14 feat(config): add density preset rework and migration v22
- 89572d8b5 2026-11-14 feat(ui): unify panel header surfaces and ghost icon buttons
- 928879226 2026-11-14 feat(ui): update toggle rest color and stepper segmented row layout
- 62ad097fc 2026-11-14 docs(m3e): add upstream vs fork diff summary table
- d998410c0 2026-11-16 feat(ui): implement M3 Expressive checkbox with state layer and indeterminate state
- 1bfb7572e 2026-11-16 feat(shell): apply tonal surface and typography to panel headers
- d10812ce1 2026-11-16 refactor(settings): migrate template store to flex list, add built-in modal, and fix hover/click routing
[SLDR-001] lib/ui/controls/slider.{h,cpp} M3E XS 7-Box layout, 6px gaps, press squish, stop dots, scaled geometry pass
[SLDR-001] lib/ui/controls/range_slider.{h,cpp} M3E XS 7-Box layout, 6px gaps, press squish, stop dots, scaled geometry pass
[SLDR-001] lib/ui/builders.cpp propagate trackHeight-derived scale to Slider/RangeSlider setScale pass
[TOG-001] lib/ui/controls/toggle.{h,cpp} animated hover/press halo and centered check glyph pass
[SHP-002] lib/ui/style.{h,cpp} slider/toggle tokens updated for M3E XS scale propagation pass
[CFG-REFACTOR] src/config/config_types.h MotionUiConfig; density→accessibility, corner_*→shell, expressive/panel→motion
[CFG-REFACTOR] src/config/schema/config_schema.{h,cpp} motion/accessibility/shell schemas; spring-token schema removed
[CFG-REFACTOR] src/config/schema/config_sections.cpp motion_preset section removed
[CFG-REFACTOR] src/config/config_migrations.cpp v24 relocates UI keys; v20 message targets shell.corner_shape
[CFG-REFACTOR] src/config/config_overrides.cpp configEqual/change set use single motion struct
[CFG-REFACTOR] src/app/application_services.cpp runtime reads new sections; setMotionConfig dropped
[CFG-REFACTOR] src/shell/panel/panel_manager.cpp motion.panel; snap open/close when animations disabled
[CFG-REFACTOR] src/shell/settings/settings_registry.cpp new paths, motion order, visibility gating
[CFG-REFACTOR] tests/config_* + config_validate/generated-config v24 + new section paths pending-test
- f737e2da2 2026-11-18 feat(icons): draw color-scheme icon themes with the shell palette
- 39058f0c3 2026-11-19 fix(ui): de-tertiary context menu hover
- 70b5e2aee 2026-11-19 feat(ui): dim attached popups on host panels
- 5e11e337b 2026-11-19 test(panel): scale test panel and fill height
- cf196d5f4 2026-11-19 feat(ui): standardize picker and widget dialog headers
- 0e5248564 2026-11-19 feat(settings): color destructive sheet action
- c115c8048 2026-11-19 feat(control-center): redesign screen time tab

## Git history appended (2026-11-20 cleanup)
- 053bcdd06 build(mcu): drop C-only warning flag from C++ args
- d10046ff3 merge m3e flag fix into vespertine
- e311fc33c fix(render): interpolate colors in premultiplied alpha
- 847495a34 fix(render): reapply pending window resizes in makeCurrent()
- d130863c9 perf(process): wait for child exit on a pidfd instead of polling
- e36a59f36 merge upstream fixes from garden into master

[REPO] projects/noctalia moved to source/noctalia; local main only pass
[REPO] noctalia fork/ves remotes cleaned; fork points to vespertine pass
[REPO] vespertine origin renamed to upstream; master tracks origin/master pass
[REPO] garden tracks origin/garden; lazygit pushRemote origin pass
[DOCS] m3e/upstream/*.md refreshed for master/garden topology pass
[DOCS] 00-overview updated with 165/106 raw and 64/38 patch status pass

