# Noctalia v5 to Material 3 Expressive Spec (rev5)

One-line: native C++/GLES M3E conversion of the Noctalia v5 shell, MVP-first (springs, radii morph, color), r(theta) shapes later, no web-only concepts.

| Field | Value |
|---|---|
| Spec ID | NOCTALIA-M3E-001 rev5 |
| Baseline | Noctalia v5 ARCHITECTURE.md + commit 6ef43e2 (re-run VG-00 before forking) |
| Mode | Solo fork, additive changes in `lib/`, behind `[motion].scheme` |
| References | M3 Expressive spec, Compose `MotionScheme.kt` (spring values), matraic/m3e (token and shape-name reference only, not a dependency) |
| Non-goals | Liquid Glass, QML runtime, porting Dux/DMS code, plugin-only implementation, compositor protocol changes, CSS `linear()`/`cubic-bezier()` easing |
| Estimate | MVP 4-6 weeks, full spec 4-6 months (solo, part-time, AI-assisted) |

---

## 0. Invariants

### 0.1 Architecture (kept from upstream docs)

| ID | Rule |
|---|---|
| INV-01 | `lib/` never includes `src/`; `lib/render` draws generic primitives only |
| INV-02 | Shell/widget code never creates raw scene nodes; controls own them |
| INV-03 | Phases strict: tick in PrepareFrame, state sync in Update, geometry in Layout, paint only in Render |
| INV-04 | No layout or scene-graph mutation during Render |
| INV-07 | Config schema in `src/config/config_types.*`; overrides in `config_overrides.cpp`; GUI writes `settings.toml`; runtime state in `state.toml` via StateStore |
| INV-09 | Setters schedule via requestUpdate/requestLayout/requestRedraw/requestFrameTick, never rebuild synchronously |

INV-05, INV-06, INV-08 apply unchanged (review gates only, no extra tests while solo).

### 0.2 M3E invariants

| ID | Rule |
|---|---|
| PHASE-001 | Springs tick in PrepareFrame, retarget in Update, geometry in Layout, shader reads resolved uniforms in Render |
| PHASE-002 | Spatial (position/size/radius/morph amount) -> requestFrameTick; discrete style switch -> requestLayout; color/opacity/elevation -> requestRedraw |
| LIB-001 | MotionScheme, SpringToken, ShapeName, ShapeProfile, ShapeController live in `lib/`; shell config maps tokens, never redefines them |
| SHP-EXT-001 | Shape extends the generic rect primitive in `lib/render/programs/`; no shell-only RenderExtension |
| REF-001 | ShapeName list matches m3e `ShapeName.ts` exactly (35 names, no `scallop`) |
| MOT-SPRING-001 | Springs are damping ratio (zeta) with mass 1; if `AnimationManager` uses absolute damping, convert `c = 2 * zeta * sqrt(k)` at the token boundary |
| MOT-INT-001 | Spring integration is closed-form damped-spring solution (or semi-implicit Euler substepped); no easing-curve sampling |
| MOT-DT-001 | PrepareFrame clamps dt to 32 ms max; stalled frames cannot explode a spring |
| MOT-IDLE-001 | Idle shell has zero wakeups: a spring snaps to target and stops requestFrameTick when `abs(x-target) < eps_x` and `abs(v) < eps_v` |
| MOT-REDUCE-001 | `[motion].reduce = true` swaps each spatial spring for its effects counterpart before resolution and disables choreography delays |
| SHP-CLAMP-001 | Corner radius clamped to `[0, min(w,h)/2]`; morph `amount` clamped to `[0,1]`; only position and size may overshoot |
| SHP-DAMAGE-001 | Damage rect includes spring overshoot beyond the rest rect |
| SHP-SCALE-001 | Radii and shape uniforms are logical px multiplied by output scale; verified at 1.25x and 1.5x |

### 0.3 Settle epsilons (defaults, tunable)

| Quantity | eps_x | eps_v |
|---|---|---|
| Position, size, radius (logical px) | 0.01 | 0.01 px/s |
| Morph amount (normalized) | 0.001 | 0.001 /s |
| Color/opacity (0..1) | 0.002 | 0.002 /s |

---

## 1. Verification gates

| ID | Gate | Method | If it fails |
|---|---|---|---|
| VG-00 | Baseline commit and component names | `git rev-parse HEAD`; inspect ARCHITECTURE.md, AnimationManager, GlesRenderBackend, rect program, CairoTextRenderer, config_types.* | Remap spec IDs |
| VG-01 | Spring syntax, retargeting, and damping convention (ratio vs absolute) | Read AnimationManager; unit harness | Add retargeting as P0; add conversion per MOT-SPRING-001 |
| VG-02 | Rect shader accepts extra uniforms without a new program variant; per-corner radii supported; uniform array vs R32F texture for r(theta) table | Scratch branch with dummy uniform | Re-evaluate SHP-EXT-001 and SHP-004 effort |
| VG-03 | Pango variable axes (wght/wdth/GRAD) | Pango attribute test | Typography descoped with warning |
| VG-04 | Controls consume M3 color roles | Audit Button, Label, panels vs `lib/ui/palette.h` | Color audit expands |
| VG-05 | ShapeController can live in `lib/ui/controls/` | Inspect control base classes | Use smaller control-level hook |
| VG-06 | Plugin surface has no renderer/animation hooks | Audit `luau_host.cpp`, `plugin_bindings.cpp` | Expected closed |
| VG-07 | Token and shape names vs m3e and Compose | Compare `ShapeName.ts`, `ThemeElement.ts`, `MotionScheme.kt` | Rename before M1 |
| VG-08 | Spring numbers vs Compose `MotionScheme.kt` | Diff against section 4 | Section 4 stays Provisional; springs load from one table so a fix is one line |

---

## 2. Gap summary

| Component | Gap | Priority |
|---|---|---|
| AnimationManager | No tokenized schemes, settle detection, dt clamp, closed-form solve, reduced-motion | P0 |
| Rect primitive | No per-corner animated radii path with clamp, overshoot damage, fractional-scale plumbing | P0 |
| Shape system | No ShapeName, no r(theta) profile, no star-convex handling | P2 (post-MVP) |
| Color | No expressive high-chroma alias; role audit incomplete | P0 |
| Controls | Interactive transitions use scalar speed; no press radii morph | P0 |
| Components | No connected button group, split button, wavy progress | P2 |
| Typography | No `font_variations`, no type-scale tokens | P3 |
| Choreography | No stagger/hero-first | P3 |
| Config | No `[motion]`, `[shape]`, `[typography]`, `[choreography]` | P0 (motion only), rest later |
| Plugins | Closed to renderer/animation (confirmed by design) | none |

---

## 3. Shape model (single representation)

| Layer | Decision |
|---|---|
| Phase 1 (MVP) | Per-corner radii `(tl, tr, br, bl)` animated by spatial springs on the existing rect primitive. Press morph = radii change, e.g. round to squarer. Uneven radii allowed (example 4/12/4/12) |
| Phase 2 | Named shapes as polar profiles `r(theta)`, N samples (start 64), baked in `lib/render/core/` |
| Morph | Lerp between two profile arrays by `amount` in `[0,1]`; no neutral-polygon path, no blending arbitrary SDFs |
| Generators | `sides`/`style` are offline or init-time generators for the tables, not runtime uniforms |
| Star-convex | r(theta) is valid only for shapes star-convex about their origin. Candidates that may fail: heart, arch, semicircle, fan, ghost-ish. Per name: use an origin offset, or fall back to the radii path. Record result per name |
| AA | Polar profile is not a true SDF; use fwidth-based approximate distance. Test pixel-triangle steps and boom spikes |
| Fast path | Plain rounded rects keep the existing RectProgram fast path; morph is opt-in per control |

### 3.1 ShapeName (35, REF-001)

4-leaf-clover, 4-sided-cookie, 6-sided-cookie, 7-sided-cookie, 8-leaf-clover, 9-sided-cookie, 12-sided-cookie, arch, arrow, boom, bun, burst, circle, diamond, fan, flower, gem, ghost-ish, heart, hexagon, oval, pentagon, pill, pixel-circle, pixel-triangle, puffy, puffy-diamond, semicircle, slanted, soft-boom, soft-burst, square, sunny, triangle, very-sunny

Cookie shapes are for loaders, avatars, decoration. They are opt-in press targets, not the default.

### 3.2 Widget tiers

| Tier | Widgets | Behavior |
|---|---|---|
| Interactive | buttons, switches, chips, bar capsules, session actions, control-center shortcuts | ShapeController: radii (later shape) morph on press/hover/select |
| Static | clock, sysmon, media art, sticker, audio visualizer, tray icons | Fixed shape, no controller, no spring |

---

## 4. Motion tokens (Provisional until VG-08)

Springs are damping ratio, mass 1. Config keys use lowercase dot form; internal IDs use `M3E-<SCHEME>-<FAMILY>-<SIZE>`.

| Token (config) | Family | Expressive zeta/k | Standard zeta/k | Overshoot |
|---|---|---|---|---|
| spatial.fast | Spatial | 0.6 / 800 | 0.9 / 1400 | expressive yes, standard minimal |
| spatial.default | Spatial | 0.8 / 380 | 0.9 / 700 | same |
| spatial.slow | Spatial | 0.8 / 200 | 0.9 / 300 | same |
| effects.fast | Effects | 1.0 / 3800 | 1.0 / 3800 | none |
| effects.default | Effects | 1.0 / 1600 | 1.0 / 1600 | none |
| effects.slow | Effects | 1.0 / 800 | 1.0 / 800 | none |

| Animation class | Token family | Request |
|---|---|---|
| Position, size, radius, morph amount | spatial | requestFrameTick |
| Discrete shape switch | spatial (retarget) | requestLayout |
| Color, opacity, elevation | effects | requestRedraw |

---

## 5. Work breakdown

Tier: **MVP** = ship first, **P2** = after MVP, **P3** = last, **Opt** = only if upstreaming.

| ID | Requirement | Target | Acceptance | Tier | Effort |
|---|---|---|---|---|---|
| MOT-001 | MotionScheme (standard/expressive) + SpringToken resolver, both schemes populated, expressive default | lib animation core | 12 tokens resolve; unknown token rejected | MVP | M |
| MOT-002 | Closed-form spring integration + dt clamp (MOT-INT-001, MOT-DT-001) | AnimationManager | 200 ms stall does not explode; effects overshoot = 0 | MVP | M |
| MOT-003 | Retargeting preserved with tokens | AnimationManager | Mid-flight retarget does not snap | MVP | M |
| MOT-004 | Wire Button, Switch, Chip to tokens | lib/ui/controls | No scalar-speed M3E transitions | MVP | M |
| MOT-005 | Settle detection with eps table (MOT-IDLE-001) | AnimationManager | Zero requestFrameTick after settle | MVP | S |
| MOT-006 | `[motion].reduce` (MOT-REDUCE-001) | AnimationManager, config | Spatial overshoot gone when true | MVP | S |
| SHP-001 | Animated per-corner radii on rect primitive with clamp and overshoot damage | lib/render/programs, controls | Press morph works; radius in `[0, min(w,h)/2]`; no clip | MVP | M |
| SHP-002 | Fractional-scale plumbing for radii (SHP-SCALE-001) | RenderContext, rect program | Correct at 1.25x and 1.5x | MVP | S |
| COL-001 | Control color-role audit (container/on-container, AA, no hardcoded hex) | controls, palette.h | Contrast AA | MVP | M |
| COL-002 | Expressive high-chroma scheme alias (Material Color Utilities variant) | src/theme | Higher-chroma palette selectable | MVP | S |
| CFG-001 | `[motion]` schema: scheme, reduce, per-token overrides; defaults expressive/false | config_types.* | Invalid token errors, no partial apply | MVP | S |
| COL-003 | Tonal elevation on panels, OSD, notifications (`surface_container_*`) | src/shell panels | Hierarchy without borders | P2 | M |
| SHP-003 | ShapeName enum (35) + r(theta) profiles + rect-program uniforms | lib/render/core, programs | All 35 render; VG-02 passed | P2 | L |
| SHP-004 | Star-convex validation, origin offset or radii fallback per name | shape program | Per-name result documented | P2 | M |
| SHP-005 | fwidth AA for polar profiles | shape program | No stair-stepping on pixel-triangle/boom | P2 | M |
| SHP-006 | ShapeController mixin | lib/ui/controls | Widgets never touch shader | P2 | M |
| SHP-007 | Tier-1 widget wiring via `[widget.<name>]` fields | src/shell, config | Tier-1 morph, tier-2 unaffected | P2 | L |
| CMP-001 | Connected button group | lib/ui/controls | Shared shape, connected corners, press morph | P2 | M |
| CMP-002 | Split button | lib/ui/controls | Chevron morph on menu open | P2 | M |
| CMP-003 | Wavy progress; needs a new generic sine-stroke primitive in `lib/render/programs/` | render + control | Determinate and indeterminate modes, effects tokens | P2 | L |
| TYP-001 | `font_variations` plumbing (gate VG-03) | Label, CairoTextRenderer | Axis applied or explicit warning; fallback fonts safe | P3 | M |
| TYP-002 | M3 type-scale tokens in `lib/ui/style.h`; cap-band centering and fractional positions preserved | style.h, Label | Existing Label tests pass | P3 | M |
| CHO-001 | Stagger utility, default 50 ms tunable, off under reduce | AnimationManager | Interruption-safe, INV-03 compliant | P3 | S |
| CHO-002 | Hero-first, default 125 ms tunable (100-150), off under reduce | AnimationManager | Focal element first; cancel restores state | P3 | M |
| PLG-001 | Plugin token-read API | src/scripting | INV-08 preserved | Opt | M |
| TEST-001 | Golden visual tests | CI | Threshold enforced | Opt | M |
| TEST-002 | RSS and frame-time budgets (RSS delta 10 MB max, 60fps 1080p) | bench | Within budget | Opt | M |

Choreography numbers are tunable defaults, not M3 tokens.

---

## 6. Config

| Section | Field | Default | Validation |
|---|---|---|---|
| `[motion]` | scheme | expressive | standard or expressive |
| `[motion]` | reduce | false | bool |
| `[motion.spatial.<size>]` | damping, stiffness | scheme table | damping > 0, stiffness > 0 |
| `[motion.effects.<size>]` | damping, stiffness | scheme table | damping >= 1.0, stiffness > 0 |
| `[shape]` | press_radii | squarer preset | non-negative radii tuple |
| `[shape]` | press_morph (P2) | unset | must be a ShapeName |
| `[widget.<name>]` | shape, shape_pressed, shape_hover (P2) | pill / inherits | existing ShapeName |
| `[widget.<name>]` | morph_spring | spatial.default | one of the six token strings |
| `[typography]` | font_variations (P3) | empty | valid OpenType tags |
| `[choreography]` | stagger_ms, hero_delay_ms (P3) | 50, 125 | >= 0 |

Runtime widget state goes to `state.toml` via StateStore, never into config sections.

---

## 7. Color rules

| ID | Rule |
|---|---|
| COL-R-01 | Button variants map to M3 container/on-container ColorRole; AA contrast; no hex |
| COL-R-02 | Panels use `surface_container_*` for tonal depth, no borders |
| COL-R-03 | Expressive scheme is selectable with fallback to existing schemes |
| COL-R-04 | OSD and notifications use `surface_container_high` + `on_surface` |
| COL-R-05 | "color role" in UI strings; "theme" means the `[theme]` subsystem |

---

## 8. Tests

Keep these even solo. Everything else is Opt.

| ID | Test | Pass |
|---|---|---|
| AT-MOT-01 | Step response, all effects tokens, both schemes | Overshoot = 0 |
| AT-MOT-02 | Mid-flight retarget | No snap |
| AT-MOT-03 | Standard vs expressive | Measurably different |
| AT-MOT-04 | Idle harness | Zero ticks after settle |
| AT-MOT-05 | Inject 200 ms stall | Stable spring |
| AT-MOT-06 | `reduce = true` | No spatial overshoot, no choreography delay |
| AT-SHP-01 | Force overshoot past half-side | Radius clamped; morph amount in `[0,1]` |
| AT-SHP-02 | Render at 1.25x and 1.5x | Radii correct |
| AT-SHP-03 | Morph with overshoot, incremental render | No clipping |
| AT-SHP-04 | Bar/panel frame time vs baseline | No regression over 5% |
| AT-CFG-01 | Invalid token | Clear error, no partial apply |
| AT-CFG-02 | No `[motion]` section | Expressive applied |
| AT-CFG-03 | Effects damping 1.2 | Accepted |
| AT-PHASE-01 | Debug assertions | No Render-phase mutation; correct request types |
| AT-INV-01 | Include-path audit | No `src/` includes in `lib/` |
| AT-INV-02 | Grep gate | No raw nodes in `src/shell/` |

P2 adds: AT-SHP all-35 render, star-convex per name, AA thin features, cross-profile morph interruptibility, AT-CMP golden checks, AT-COL contrast checker.

---

## 9. Milestones

| Milestone | Scope | Exit | Time |
|---|---|---|---|
| M0 | VG-00 to VG-08 | Baseline, spring convention, uniform path, and numbers confirmed | 2-3 days |
| M1 | MOT-001 to 006, CFG-001 | Tokens live, idle verified, controls migrated | ~1 week |
| M2 | SHP-001, SHP-002 | Press radii morph, clamp, damage, fractional scale | ~1 week |
| M3 | COL-001, COL-002 | Expressive palette, role audit | ~1 week |
| MVP | M0 to M3 | Shippable expressive feel | 4-6 weeks total |
| M4 | COL-003, SHP-003 to 007 | r(theta) shapes (about 10 first, then 35), controller, tier-1 wiring | +4-5 weeks |
| M5 | CMP-001 to 003 | Expressive components | +3 weeks |
| M6 | TYP, CHO | Type scale, variable fonts, choreography | +2-3 weeks |

---

## 10. Risks

| Risk | Mitigation |
|---|---|
| Rect shader needs a new program variant | VG-02 first; revise SHP-EXT-001 explicitly if blocked |
| Spring convention mismatch (ratio vs absolute) | VG-01, MOT-SPRING-001 conversion |
| Spring numbers drift from Material | VG-08; single token table |
| Idle wakeups drain battery | MOT-IDLE-001, AT-MOT-04 |
| Radius or amount overshoot gives inverted shapes | SHP-CLAMP-001, AT-SHP-01 |
| Non-star-convex shapes break r(theta) | Per-name origin offset or radii fallback |
| Polar AA on thin features | fwidth distance, visual test |
| SDF cost on iGPU | Keep fast path; morph opt-in |
| Pango variable-axis limits | VG-03; descope with warning |
| Upstream moves fast | Additive lib changes, rebase often, re-run VG-00 |
| Scope creep (35 shapes, components) | MVP tier gates everything else |

---

## 11. Definition of done

| Tier | Done when |
|---|---|
| MVP | Both schemes live and tunable; reduce works; zero idle wakeups; controls use tokens; press radii morph clamped and damage-correct at fractional scales; expressive palette selectable; MVP tests pass |
| P2 | 35 names render with per-name star-convex result; tier-1 widgets morph via ShapeController; components shipped; tonal panels |
| P3 | Type scale and variable fonts (or descoped with warning); choreography with reduce support |

---

## 12. Revision log

| Rev | Change |
|---|---|
| rev1 to rev3 | Initial spec, architecture alignment, m3e alignment |
| rev4 | Standard springs, idle/dt/reduce rules, clamp/AA/damage/scale rules, components, expanded tests |
| rev5 | Single shape model (radii first, r(theta) later, SHP-007 neutral-polygon removed); web easing (`linear()`, `cubic-bezier()`, AT-MOT-04 sampler) removed; spring convention gate added; press morph defaults to radii; amount clamp and settle epsilons added; duplicate rows merged; wavy progress gets a render-primitive note; MVP tier and timeline added; pipe-corrupted rows fixed; process-heavy tests moved to Opt |
