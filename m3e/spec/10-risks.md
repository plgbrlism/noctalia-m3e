# 10. Risks

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

# 11. Definition of done

| Tier | Done when |
|---|---|
| MVP | Both schemes live and tunable; reduce works; zero idle wakeups; controls use tokens; press radii morph clamped and damage-correct at fractional scales; expressive palette selectable; MVP tests pass |
| P2 | 35 names render with per-name star-convex result; tier-1 widgets morph via ShapeController; components shipped; tonal panels |
| P3 | Type scale and variable fonts (or descoped with warning); choreography with reduce support |
