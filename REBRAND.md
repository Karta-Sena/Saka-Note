# Otso Desktop — Rebrand Inventory

Complete audit of every color token and icon asset used in the application.
Use this as the handoff reference before executing the rebrand.

---

## Color System

All colors are defined in `src/modules/design_system.h` under `DesignSystem::Color`.
No hardcoded hex values exist anywhere else in the codebase — every render path
goes through a token or a `ThemeColorXxx()` function.

> **COLORREF format note:** Windows stores colors as `0x00BBGGRR` (BGR byte order).
> The `#RRGGBB` values below are the standard design-tool hex equivalents.

---

### Base Tokens

#### Dark Mode

| Token | COLORREF | `#RRGGBB` | Role |
|---|---|---|---|
| `kDarkBg` | `0x121212` | `#121212` | Editor / menu / status bar background |
| `kDarkInk` | `0xD6D6D6` | `#D6D6D6` | Primary text |
| `kDarkMuted` | `0xABABAB` | `#ABABAB` | Secondary text (inactive tab labels, close button) |
| `kDarkSubtle` | `0x8A8A8A` | `#8A8A8A` | Tertiary text (About manifesto paragraph) |
| `kDarkFaint` | `0x484848` | `#484848` | Quaternary text (About footer "Technical Standard") |
| `kDarkEdge` | `0x8B8B8B` | `#8B8B8B` | Borders, separators, window chrome border |
| `kDarkSurface` | `0x242424` | `#242424` | Elevated surface (About logo container) |

#### Light Mode

| Token | COLORREF | `#RRGGBB` | Role |
|---|---|---|---|
| `kLightBg` | `0xFBFDFD` | `#FDFDFB` | Editor / menu / status bar background (Studio Bone) |
| `kLightInk` | `0x1A1A1A` | `#1A1A1A` | Primary text |
| `kLightMuted` | `0x4D4D4D` | `#4D4D4D` | Secondary text (inactive tab labels, close button) |
| `kLightSubtle` | `0x5C5C5C` | `#5C5C5C` | Tertiary text (About manifesto paragraph) |
| `kLightFaint` | `0xA0A0A0` | `#A0A0A0` | Quaternary text (About footer "Technical Standard") |
| `kLightEdge` | `0x8F8F8F` | `#8F8F8F` | Borders, separators, window chrome border |
| `kLightSurface` | `0xE6E8E8` | `#E8E8E6` | Elevated surface (About logo container) |

#### Accent (Brand Color)

| Token | COLORREF | `#RRGGBB` | Name | Role |
|---|---|---|---|---|
| `kAccent` | `0xE21A00` | `#001AE2` | Blueprint Blue | Active tab text, tab hover tint, menu hover tint |

> **To rebrand:** change only `kAccent` in `design_system.h`.
> Formula: if your new brand color is `#RRGGBB`, the COLORREF value is `0xBBGGRR`.

---

### Derived / Computed Colors

These are not stored as tokens — they are calculated at runtime from the base tokens.
They will change automatically when the base tokens change.

| Surface | Dark result (approx.) | Light result (approx.) | How it's computed |
|---|---|---|---|
| Tab inactive background | `#0E0E0E` | `#EEEEED` | `Blend(kBg, #000000, 20%)` / `Blend(kBg, #000000, 6%)` |
| Tab hover background | `#121213` (blue-tinted) | `#F5F5FC` (blue-tinted) | `Blend(kBg, kAccent, 8%)` / `Blend(kBg, kAccent, 12%)` |
| Menu hover background | `#121212` (accent 4%) | `#FDFCFC` (accent 4%) | `Blend(kBg, kAccent, 4%)` |
| Menu disabled text | `~#727272` | `~#9E9E9F` | `Blend(kInk, kBg, 52%)` / `Blend(kInk, kBg, 48%)` |

---

### Semantic Color Map

Where each token actually appears in the UI:

| UI Surface | Dark token | Light token |
|---|---|---|
| Editor canvas | `kDarkBg` | `kLightBg` |
| Editor text | `kDarkInk` | `kLightInk` |
| Menu bar background | `kDarkBg` | `kLightBg` |
| Menu text | `kDarkInk` | `kLightInk` |
| Menu hover | derived | derived |
| Menu disabled text | derived | derived |
| Status bar background | `kDarkBg` | `kLightBg` |
| Status bar text | `kDarkInk` | `kLightInk` |
| Status bar separator | `kDarkEdge` | `kLightEdge` |
| Title bar background (DWM) | `kDarkBg` | `kLightBg` |
| Title bar text (DWM) | `kDarkInk` | `kLightInk` |
| Window border (DWM) | `kDarkEdge` | `kLightEdge` |
| Tab strip background | `kDarkBg` | `kLightBg` |
| Tab strip border | `kDarkEdge` | `kLightEdge` |
| Tab active background | `kDarkBg` | `kLightBg` |
| Tab inactive background | derived | derived |
| Tab hover background | derived | derived |
| Tab active text | `kAccent` | `kAccent` |
| Tab inactive text | `kDarkMuted` | `kLightMuted` |
| Tab close button | `kDarkMuted` | `kLightMuted` |
| Tab close hover fg | `kDarkInk` | `kLightInk` |
| Command palette background | `kDarkBg` | `kLightBg` |
| Command palette text | `kDarkInk` | `kLightInk` |
| About dialog background | `kDarkBg` | `kLightBg` |
| About logo container fill | `kDarkSurface` | `kLightSurface` |
| About version text | `kDarkInk` | `kLightInk` |
| About manifesto text | `kDarkSubtle` | `kLightSubtle` |
| About footer text | `kDarkFaint` | `kLightFaint` |
| Premium header text | `kDarkInk` | `kLightInk` |
| Background image canvas | `kDarkBg` | `kLightBg` |

---

## Icons & Logos

All icon files live in `src/`. They are embedded into the `.exe` at compile time via `src/notepad.rc`.

### Icon Inventory

| File | Size | Resource ID | Constant | Where it's used |
|---|---|---|---|---|
| `src/app_icon.ico` | 122 KB | `IDI_NOTEPAD` (103) | — | **Taskbar** (big icon, `ICON_BIG`). Permanent — never swapped on theme change. This is the primary brand logo seen in Windows Explorer, Alt+Tab, taskbar. |
| `src/in_app_icon.ico` | 42 KB | `IDI_IN_APP_ICON` (105) | — | **Fallback only** — used in the About dialog if neither themed variant loads. |
| `src/in_app_icon_light.ico` | 42 KB | `IDI_IN_APP_ICON_LIGHT` (106) | — | **Light mode title bar** (`ICON_SMALL`) + About dialog icon in light mode. |
| `src/in_app_icon_dark.ico` | 48 KB | `IDI_IN_APP_ICON_DARK` (107) | — | **Dark mode title bar** (`ICON_SMALL`) + About dialog icon in dark mode. |
| `src/icon.ico` | 122 KB | _(not referenced in .rc)_ | — | Unused / archive copy. Not embedded in the binary. |

### Icon Roles Explained

```
app_icon.ico          →  Taskbar / Explorer / Alt+Tab  (always the "brand logo")
in_app_icon_light.ico →  Title bar monogram when Light Mode is active
in_app_icon_dark.ico  →  Title bar monogram when Dark Mode is active
in_app_icon.ico       →  Fallback (safety net, same as light variant currently)
```

The "surgical icon swap" pattern (in `theme.cpp` → `ApplyTheme()`):

```cpp
// Taskbar stays brand logo — never changes
ICON_BIG  ← IDI_NOTEPAD (app_icon.ico)

// Title bar monogram swaps per theme
ICON_SMALL ← IDI_IN_APP_ICON_DARK  (when dark mode)
           ← IDI_IN_APP_ICON_LIGHT (when light mode)
```

---

## Rebrand Checklist

### Colors

- [ ] Decide new accent `#RRGGBB` → convert to COLORREF → update `kAccent` in `design_system.h`
- [ ] (Optional) update dark/light palette tokens if the full palette changes
- [ ] Build and verify — all surfaces update automatically

### Icons / Logos

- [ ] Replace `src/app_icon.ico` — new brand logo (multi-size .ico: 16, 24, 32, 48, 64, 128, 256 px recommended)
- [ ] Replace `src/in_app_icon_light.ico` — monogram/symbol for light title bar
- [ ] Replace `src/in_app_icon_dark.ico` — monogram/symbol for dark title bar
- [ ] Replace `src/in_app_icon.ico` — same as light variant (fallback)
- [ ] Delete or replace `src/icon.ico` if it was the old logo
- [ ] Rebuild — icons are embedded at compile time, no other code changes needed

---

*Generated 2026-05-05 from source audit of OtsoDesktop main branch.*
