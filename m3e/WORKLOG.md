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
