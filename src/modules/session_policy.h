/*
  Otso

  Shared session persistence policy constants and helpers.
*/

#pragma once

#include <windows.h>
#include <algorithm>

#include "core/types.h"

namespace SessionPolicy
{
constexpr DWORD kMagic = 0x4C4E5331u; // "LNS1"
constexpr DWORD kVersion = 1u;
constexpr DWORD kMaxStringChars = 8u * 1024u * 1024u;
constexpr DWORD kMaxFileBytes = 64u * 1024u * 1024u;
constexpr UINT_PTR kAutosaveTimerId = 0x4C4E01u;
constexpr UINT kAutosaveIntervalMs = 1500u;
constexpr DWORD kRetryBackoffMs = 10000u;

inline DWORD MaxDocuments(const AppState &state)
{
    return std::clamp(state.sessionMaxDocuments, SESSION_MAX_DOCUMENTS_MIN, SESSION_MAX_DOCUMENTS_MAX);
}
} // namespace SessionPolicy

