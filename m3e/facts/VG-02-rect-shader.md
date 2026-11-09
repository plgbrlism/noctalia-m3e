# VG-02 — Rect shader uniform capacity and shape extension
Verdict: pass
Baseline commit: ed7a4f167
Date: 2026-11-09

## Evidence
- `u_radii` is already a `vec4` (tl, tr, br, bl) supporting 4 per-corner radii — `lib/render/programs/rect_program.cpp:50`
- `u_corner_shapes` is already a `vec4` (tl, tr, br, bl) supporting convex/concave per corner — `lib/render/programs/rect_program.cpp:48`
- Extra uniforms (such as shape morph amount `u_morph_amount` as a float) can be added directly to `RectProgram` and fragment shader without a new program variant because OpenGL ES 2.0 implementations guarantee support for at least 128 vec4 uniform vectors in fragment shaders, and current usage is well below this limit.
- For P2 SHP-003 (`r(theta)` shape tables), an R32F texture slot is required rather than a uniform array due to vertex/fragment uniform component limitations across older hardware and uniform vector count limits.

## MVP impact (SHP-001)
SHP-001 needs animated per-corner radii on rect primitive with clamp and overshoot damage. Since `u_radii` (vec4) and per-corner shapes already exist in `RectProgram`, SHP-001 can wire directly into existing uniforms without creating a new program variant.

## P2 impact (SHP-003)
SHP-003 (`r(theta)` profiles) will require an R32F texture slot for storing shape boundary lookup tables because uniform arrays would exceed ES2 uniform limits for 36+ angle points.
