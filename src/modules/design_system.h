/*
  Otso

  Centralized desktop design-system tokens for visual consistency.
*/

#pragma once

#include <windows.h>

namespace DesignSystem
{
inline constexpr wchar_t kUiFontPrimary[] = L"General Sans";
inline constexpr wchar_t kUiFontPrimaryMedium[] = L"General Sans Medium";
inline constexpr wchar_t kUiFontPrimarySemibold[] = L"General Sans Semibold";
inline constexpr wchar_t kUiFontFallback[] = L"Consolas";

inline constexpr int kChromeFontPointSize = 9;
inline constexpr int kChromeBandHeightPx = 34;
inline constexpr int kChromeStrokePx = 1;
inline constexpr int kMenuTextPaddingHPx = 0;
inline constexpr int kMenuTextPaddingVPx = 1;

inline constexpr int kCommandBarPaddingHPx = 10;
inline constexpr int kCommandBarPaddingVPx = 4;
inline constexpr int kCommandBarIndentPx = 0;
inline constexpr int kCommandBarHoverInsetPx = 0;

inline constexpr int kTabTextPaddingHPx = 14;
inline constexpr int kTabSeamStrokePx = 2;
inline constexpr int kTabSeparatorInsetYPx = 6;
inline constexpr int kTabSeparatorAlphaPct = 8;
inline constexpr int kTabInnerPaddingHPx = 16;
inline constexpr int kTabInnerPaddingVPx = 6;
inline constexpr int kTabFixedWidthPx = 180;
inline constexpr int kTabCloseGlyphSizePx = 10;
inline constexpr int kTabCloseRightInsetPx = 10;

inline constexpr int kGlobalMarginPx = 12;
inline constexpr int kEditorInsetPx = 2;

inline float GetDpiScale(HWND hwnd = nullptr)
{
    const HWND ref = hwnd ? hwnd : GetDesktopWindow();
    HDC hdc = GetDC(ref);
    if (!hdc)
        return 1.0f;
    const float scale = static_cast<float>(GetDeviceCaps(hdc, LOGPIXELSX)) / 96.0f;
    ReleaseDC(ref, hdc);
    return scale > 0.0f ? scale : 1.0f;
}

inline int ScalePx(int logicalPx, HWND hwnd = nullptr)
{
    return static_cast<int>(logicalPx * GetDpiScale(hwnd) + 0.5f);
}

// ── Color Tokens ────────────────────────────────────────────────────
//
// All values are Windows COLORREF (0x00BBGGRR — little-endian RGB).
// To convert a standard #RRGGBB hex to COLORREF: reverse the byte pairs.
//   Example: #001AE2  →  COLORREF 0xE21A00
//
// !! REBRANDING NOTE !!
// Only kAccent needs to change for a full rebrand.
// Formula: kAccent = 0x<BB><GG><RR>  where BB/GG/RR are bytes of your #RRGGBB.
//
namespace Color
{
// ── Dark mode ──────────────────────────────────────────────────────
inline constexpr unsigned long kDarkBg      = 0x121212; // #121212  background
inline constexpr unsigned long kDarkInk     = 0xD6D6D6; // #D6D6D6  primary text
inline constexpr unsigned long kDarkMuted   = 0xABABAB; // #ABABAB  secondary text (inactive tabs)
inline constexpr unsigned long kDarkSubtle  = 0x8A8A8A; // #8A8A8A  tertiary text (descriptions)
inline constexpr unsigned long kDarkFaint   = 0x484848; // #484848  quaternary text (footer labels)
inline constexpr unsigned long kDarkEdge    = 0x8B8B8B; // #8B8B8B  borders / separators
inline constexpr unsigned long kDarkSurface = 0x242424; // #242424  elevated surface (cards, containers)

// ── Light mode ─────────────────────────────────────────────────────
inline constexpr unsigned long kLightBg      = 0xFBFDFD; // #FDFDFB  background (Studio Bone)
inline constexpr unsigned long kLightInk     = 0x1A1A1A; // #1A1A1A  primary text
inline constexpr unsigned long kLightMuted   = 0x4D4D4D; // #4D4D4D  secondary text (inactive tabs)
inline constexpr unsigned long kLightSubtle  = 0x5C5C5C; // #5C5C5C  tertiary text (descriptions)
inline constexpr unsigned long kLightFaint   = 0xA0A0A0; // #A0A0A0  quaternary text (footer labels)
inline constexpr unsigned long kLightEdge    = 0x8F8F8F; // #8F8F8F  borders / separators
inline constexpr unsigned long kLightSurface = 0xE6E8E8; // #E8E8E6  elevated surface (cards, containers)

// ── Accent — change this to rebrand ────────────────────────────────
// Current brand: Blueprint Blue #001AE2  →  COLORREF 0xE21A00
inline constexpr unsigned long kAccent = 0xE21A00; // #001AE2  Blueprint Blue
}
}
