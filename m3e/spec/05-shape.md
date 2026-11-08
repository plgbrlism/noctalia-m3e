# 3. Shape model (single representation)

| Layer | Decision |
|---|---|
| Phase 1 (MVP) | Per-corner radii `(tl, tr, br, bl)` animated by spatial springs on the existing rect primitive. Press morph = radii change, e.g. round to squarer. Uneven radii allowed (example 4/12/4/12) |
| Phase 2 | Named shapes as polar profiles `r(theta)`, N samples (start 64), baked in `lib/render/core/` |
| Morph | Lerp between two profile arrays by `amount` in `[0,1]`; no neutral-polygon path, no blending arbitrary SDFs |
| Generators | `sides`/`style` are offline or init-time generators for the tables, not runtime uniforms |
| Star-convex | r(theta) is valid only for shapes star-convex about their origin. Candidates that may fail: heart, arch, semicircle, fan, ghost-ish. Per name: use an origin offset, or fall back to the radii path. Record result per name |
| AA | Polar profile is not a true SDF; use fwidth-based approximate distance. Test pixel-triangle steps and boom spikes |
| Fast path | Plain rounded rects keep the existing RectProgram fast path; morph is opt-in per control |

## 3.1 ShapeName (35, REF-001)

4-leaf-clover, 4-sided-cookie, 6-sided-cookie, 7-sided-cookie, 8-leaf-clover, 9-sided-cookie, 12-sided-cookie, arch, arrow, boom, bun, burst, circle, diamond, fan, flower, gem, ghost-ish, heart, hexagon, oval, pentagon, pill, pixel-circle, pixel-triangle, puffy, puffy-diamond, semicircle, slanted, soft-boom, soft-burst, square, sunny, triangle, very-sunny

Cookie shapes are for loaders, avatars, decoration. They are opt-in press targets, not the default.

## 3.2 Widget tiers

| Tier | Widgets | Behavior |
|---|---|---|
| Interactive | buttons, switches, chips, bar capsules, session actions, control-center shortcuts | ShapeController: radii (later shape) morph on press/hover/select |
| Static | clock, sysmon, media art, sticker, audio visualizer, tray icons | Fixed shape, no controller, no spring |
