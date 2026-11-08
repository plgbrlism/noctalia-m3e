# 8. Tests

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
