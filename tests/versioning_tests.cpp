#include "core/versioning.h"

#include <iostream>
#include <windows.h>

namespace
{
bool ExpectEqual(const wchar_t *name, int actual, int expected)
{
    if (actual == expected)
        return true;

    std::wcerr << L"[FAIL] " << name
               << L": expected " << expected
               << L", got " << actual << L"\n";
    return false;
}

bool ExpectStringEqual(const wchar_t *name, const std::wstring &actual, const std::wstring &expected)
{
    if (actual == expected)
        return true;

    std::wcerr << L"[FAIL] " << name
               << L": expected \"" << expected
               << L"\", got \"" << actual << L"\"\n";
    return false;
}
}

int RunVersioningTests()
{
    bool ok = true;

    ok = ExpectStringEqual(L"NormalizeVersionTag strips prefix", NormalizeVersionTag(L"v1.3.0"), L"1.3.0") && ok;
    ok = ExpectStringEqual(L"NormalizeVersionTag keeps numeric tag", NormalizeVersionTag(L"1.3.0"), L"1.3.0") && ok;
    ok = ExpectStringEqual(L"NormalizeVersionTag handles garbage", NormalizeVersionTag(L"release"), L"") && ok;
    ok = ExpectStringEqual(L"NormalizeVersionTag stops at suffix", NormalizeVersionTag(L"v1.3.0-beta"), L"1.3.0") && ok;

    ok = ExpectEqual(L"CompareVersions equal", CompareVersions(L"1.3.0", L"1.3.0"), 0) && ok;
    ok = ExpectEqual(L"CompareVersions less", CompareVersions(L"1.2.9", L"1.3.0"), -1) && ok;
    ok = ExpectEqual(L"CompareVersions greater", CompareVersions(L"2.0.0", L"1.9.9"), 1) && ok;
    ok = ExpectEqual(L"CompareVersions with trailing zeros", CompareVersions(L"1.3", L"1.3.0"), 0) && ok;
    ok = ExpectEqual(L"CompareVersions lexical trap", CompareVersions(L"1.10.0", L"1.2.0"), 1) && ok;
    ok = ExpectEqual(L"CompareVersions uneven parts", CompareVersions(L"1.0.0.1", L"1"), 1) && ok;

    if (!ok)
        return 1;

    std::wcout << L"[PASS] versioning tests\n";
    return 0;
}

int main()
{
    return RunVersioningTests();
}

int WINAPI wWinMain(HINSTANCE, HINSTANCE, PWSTR, int)
{
    return RunVersioningTests();
}
