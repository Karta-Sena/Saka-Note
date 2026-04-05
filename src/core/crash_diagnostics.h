#pragma once

#include <string>

// Initializes optional diagnostics controlled by environment variables:
// - TECHNICAL_STANDARD_NOTE_DIAGNOSTICS=1 enables startup-safe logging.
// - TECHNICAL_STANDARD_NOTE_MINIDUMP=1 enables unhandled-exception minidump generation.
void InitializeCrashDiagnostics();

// Appends a short line to diagnostics log when diagnostics are enabled.
void CrashDiagnosticsLog(const std::wstring &message);
