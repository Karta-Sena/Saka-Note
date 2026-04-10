/*
  Technical Standard

  Centralized desktop design-system tokens for visual consistency.
*/

#pragma once

namespace DesignSystem
{
inline constexpr wchar_t kUiFontPrimary[] = L"Akkurat Mono LL";
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
inline constexpr int kTabSeamStrokePx = 1;
inline constexpr int kTabSeparatorInsetYPx = 6;
inline constexpr int kTabSeparatorAlphaPct = 8;
inline constexpr int kTabInnerPaddingHPx = 16;
inline constexpr int kTabInnerPaddingVPx = 6;
inline constexpr int kTabFixedWidthPx = 180;

// Color Tokens (Mobile Parity)
namespace Color
{
inline constexpr unsigned long kDarkBg = 0x01110E;       // #0E1117 (BGR for COLORREF)
inline constexpr unsigned long kDarkInk = 0xFFFFFF;      // #FFFFFF
inline constexpr unsigned long kDarkEdge = 0xA6A6A6;     // #FFFFFF @ 0.65
inline constexpr unsigned long kDarkMuted = 0xCCCCCC;    // #FFFFFF @ 0.80

inline constexpr unsigned long kLightBg = 0xFFFFFF;      // #FFFFFF
inline constexpr unsigned long kLightInk = 0x000000;     // #000000
inline constexpr unsigned long kLightEdge = 0x616161;    // #000000 @ 0.62
inline constexpr unsigned long kLightMuted = 0x3D3D3D;   // #000000 @ 0.76

inline constexpr unsigned long kAccent = 0xFFFFFF;       // Default accent
}
}
