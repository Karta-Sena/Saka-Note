#pragma once

#include <string>

// Returns numeric dotted version part, skipping optional prefix such as "v".
std::wstring NormalizeVersionTag(const std::wstring &tag);

// Compares two dotted numeric versions (e.g. 1.2.10 vs 1.2.3).
// Returns -1 if left < right, 1 if left > right, or 0 when equal.
int CompareVersions(const std::wstring &left, const std::wstring &right);
