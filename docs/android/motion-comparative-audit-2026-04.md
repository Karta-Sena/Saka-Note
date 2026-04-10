# Motion Comparative Audit (2026-04-10)

## Scope
- Local baseline: `.agent/skills/emilkowalski/*` in repository.
- External reference: `https://emilkowal.ski/skill` and package install flow from `npx skills add emilkowalski/skill`.
- App target: `android/app/src/main/java/com/wisesakarta/technicalstandardnote/ui/EditorApp.kt`.

## External Evaluation Run
Command executed in isolated environment (HOME remapped):

```powershell
npx --yes skills add emilkowalski/skill --yes --global
```

Observed external package:
- Skill id: `emil-design-eng`
- Install source: `https://github.com/emilkowalski/skill.git`
- Security summary in installer: `Gen Safe`, `Socket 0 alerts`, `Snyk Low Risk`.

## Comparative Findings

### 1) Physics-based animation quality (spring mechanics)
- Local (before): partial spring use (button press), most transitions used duration-only tweens.
- External: explicit decision framework for spring usage (interruptible gestures, state transitions, velocity continuity).
- Decision: external is architecturally stronger and more systematic.

### 2) Performance discipline (60fps-120fps intent)
- Local (before): already lightweight visual system, but no unified motion contract and no explicit feedback-channel policy.
- External: strict rules for `transform/opacity`, timing bands (<300ms), and reduced-motion fallbacks.
- Decision: external guidance improves performance predictability.

### 3) Gesture quality and interruptibility
- Local (before): no dedicated interruptible drag/swipe transition path for tab navigation.
- External: emphasizes interruptible motion and spring retargeting.
- Decision: external guidance materially better for responsive touch interactions.

## Skill Update Action
Because external architecture is stronger, local skill has been updated:

- Updated file: `.agent/skills/emilkowalski/SKILL.md`
- Method: overwrite local SKILL with externally installed `emil-design-eng` skill body.

## Mobile Motion Implementation (Applied)
Implemented in `EditorApp.kt`:

1. Navigation transitions
- Added spring-based tab content shift when active tab changes.
- Direction-aware transition (left/right) with interruptible `Animatable`.

2. Swipe/drag interaction
- Added horizontal drag gesture on editor canvas to switch tabs.
- Threshold-based tab change and spring settle-back to neutral state.
- Drag transition is interruptible and retargetable.

3. Feedback state micro-interactions
- Added unified animated feedback overlay for `Loading`, `Success`, `Error`.
- Replaced multiple ad-hoc toasts in critical actions (`open`, `save`, `lock`, `find`) with animated feedback channel.
- Added reduced-motion-compatible fallbacks.

4. Motion tokens/system
- Added explicit motion constants and spring presets (`springNav`, `springGesture`, `springPress`).
- Added timing standardization for fast/medium/slow phases.

## Validation
- Build: `:app:assembleDebug` ✅
- Unit tests: `:app:testDebugUnitTest` ✅
- Emulator run: app launches successfully ✅

## Notes on FPS Measurement
A quick `dumpsys gfxinfo` pass on emulator produced inconclusive frame histogram data (0 rendered frames in report despite interaction replay), which can happen on some emulator/render pipeline combinations.

For strict 60fps/120fps verification, use Macrobenchmark + FrameTimingMetric on physical device (release build profile).
