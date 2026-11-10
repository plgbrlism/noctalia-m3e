# 7. Color rules

| ID | Rule |
|---|---|
| COL-R-01 | Button variants map to M3 container/on-container ColorRole; AA contrast; no hex |
| COL-R-02 | Panels use `surface_container_*` for tonal depth, no borders |
| COL-R-03 | Expressive scheme is selectable with fallback to existing schemes |
| COL-R-04 | OSD and notifications use `surface_container_high` + `on_surface` |
| COL-R-05 | "color role" in UI strings; "theme" means the `[theme]` subsystem |
| COL-R-06 | `[theme].design_system` is theme-level. When = expressive, settings GUI recommends `[theme].wallpaper_scheme = "m3-expressive"` (existing user value preserved if set). Palette generation unchanged — UX guidance only. |
