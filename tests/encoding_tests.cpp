#include "core/text_codec.h"

#include <windows.h>
#include <iostream>
#include <vector>

namespace
{
bool ExpectTrue(const wchar_t *name, bool condition)
{
    if (condition)
        return true;
    std::wcerr << L"[FAIL] " << name << L"\n";
    return false;
}

bool ExpectEqualEncoding(const wchar_t *name, Encoding actual, Encoding expected)
{
    if (actual == expected)
        return true;
    std::wcerr << L"[FAIL] " << name << L": unexpected encoding\n";
    return false;
}

bool ExpectEqualLineEnding(const wchar_t *name, LineEnding actual, LineEnding expected)
{
    if (actual == expected)
        return true;
    std::wcerr << L"[FAIL] " << name << L": unexpected line ending\n";
    return false;
}

bool ExpectEqualString(const wchar_t *name, const std::wstring &actual, const std::wstring &expected)
{
    if (actual == expected)
        return true;
    std::wcerr << L"[FAIL] " << name << L": expected [" << expected << L"], got [" << actual << L"]\n";
    return false;
}
}

int RunEncodingTests()
{
    bool ok = true;

    {
        const std::vector<BYTE> data = {0xEF, 0xBB, 0xBF, 'A', '\n'};
        auto [enc, le] = DetectEncoding(data);
        ok = ExpectEqualEncoding(L"Detect UTF8 BOM", enc, Encoding::UTF8BOM) && ok;
        ok = ExpectEqualLineEnding(L"Detect LF", le, LineEnding::LF) && ok;
    }

    {
        const std::vector<BYTE> data = {0xFF, 0xFE, 'A', 0x00, 'B', 0x00};
        auto [enc, _] = DetectEncoding(data);
        ok = ExpectEqualEncoding(L"Detect UTF16LE", enc, Encoding::UTF16LE) && ok;
        const std::wstring decoded = DecodeText(data, Encoding::UTF16LE);
        ok = ExpectEqualString(L"Decode UTF16LE", decoded, L"AB") && ok;
    }

    {
        const std::wstring src = L"a\r\nb\nc\rd";
        const auto bytes = EncodeText(src, Encoding::UTF8, LineEnding::LF);
        auto [enc, le] = DetectEncoding(bytes);
        const std::wstring decoded = DecodeText(bytes, enc);
        ok = ExpectEqualEncoding(L"Encode UTF8 detect", enc, Encoding::UTF8) && ok;
        ok = ExpectEqualLineEnding(L"Normalize LF", le, LineEnding::LF) && ok;
        ok = ExpectEqualString(L"Encode normalize line ending", decoded, L"a\nb\nc\nd") && ok;
    }

    {
        const std::wstring src = L"hello";
        const auto bytes = EncodeText(src, Encoding::UTF16BE, LineEnding::CRLF);
        ok = ExpectTrue(L"Encode UTF16BE BOM", bytes.size() >= 2 && bytes[0] == 0xFE && bytes[1] == 0xFF) && ok;
        const std::wstring decoded = DecodeText(bytes, Encoding::UTF16BE);
        ok = ExpectEqualString(L"Roundtrip UTF16BE", decoded, src) && ok;
    }

    if (!ok)
        return 1;
    std::wcout << L"[PASS] encoding tests\n";
    return 0;
}

int main()
{
    return RunEncodingTests();
}

int WINAPI wWinMain(HINSTANCE, HINSTANCE, PWSTR, int)
{
    return RunEncodingTests();
}
