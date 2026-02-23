#include "text_codec.h"

#include <windows.h>
#include <algorithm>

std::pair<Encoding, LineEnding> DetectEncoding(const std::vector<BYTE> &data)
{
    Encoding enc = Encoding::UTF8;
    if (data.size() >= 3 && data[0] == 0xEF && data[1] == 0xBB && data[2] == 0xBF)
        enc = Encoding::UTF8BOM;
    else if (data.size() >= 2 && data[0] == 0xFF && data[1] == 0xFE)
        enc = Encoding::UTF16LE;
    else if (data.size() >= 2 && data[0] == 0xFE && data[1] == 0xFF)
        enc = Encoding::UTF16BE;
    else
    {
        int result = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS,
                                         reinterpret_cast<const char *>(data.data()), static_cast<int>(data.size()), nullptr, 0);
        if (result == 0 && GetLastError() == ERROR_NO_UNICODE_TRANSLATION)
            enc = Encoding::ANSI;
    }
    LineEnding le = LineEnding::CRLF;
    for (size_t i = 0; i < data.size(); ++i)
    {
        if (data[i] == '\r')
        {
            le = (i + 1 < data.size() && data[i + 1] == '\n') ? LineEnding::CRLF : LineEnding::CR;
            break;
        }
        if (data[i] == '\n')
        {
            le = LineEnding::LF;
            break;
        }
    }
    return {enc, le};
}

std::wstring DecodeText(const std::vector<BYTE> &data, Encoding enc)
{
    std::wstring result;
    size_t skip = 0;
    UINT codepage = CP_UTF8;
    switch (enc)
    {
    case Encoding::UTF8BOM:
        skip = 3;
        break;
    case Encoding::UTF16LE:
    {
        skip = 2;
        if (data.size() < skip)
            return L"";
        const wchar_t *wptr = reinterpret_cast<const wchar_t *>(data.data() + skip);
        result = std::wstring(wptr, (data.size() - skip) / 2);
        std::replace(result.begin(), result.end(), L'\0', L' ');
        return result;
    }
    case Encoding::UTF16BE:
    {
        skip = 2;
        if (data.size() < skip)
            return L"";
        result.reserve((data.size() - skip) / 2);
        for (size_t i = skip; i + 1 < data.size(); i += 2)
            result += static_cast<wchar_t>((data[i] << 8) | data[i + 1]);
        std::replace(result.begin(), result.end(), L'\0', L' ');
        return result;
    }
    case Encoding::ANSI:
        codepage = CP_ACP;
        break;
    default:
        break;
    }
    const char *ptr = reinterpret_cast<const char *>(data.data() + skip);
    int len = static_cast<int>(data.size() - skip);
    if (len <= 0)
        return L"";
    int wlen = MultiByteToWideChar(codepage, 0, ptr, len, nullptr, 0);
    if (wlen <= 0)
        return L"";
    result.assign(wlen, 0);
    MultiByteToWideChar(codepage, 0, ptr, len, &result[0], wlen);
    std::replace(result.begin(), result.end(), L'\0', L' ');
    return result;
}

std::vector<BYTE> EncodeText(const std::wstring &text, Encoding enc, LineEnding le)
{
    std::wstring converted;
    converted.reserve(text.size() * 2);
    for (size_t i = 0; i < text.size(); ++i)
    {
        wchar_t c = text[i];
        if (c == L'\r' || c == L'\n')
        {
            if (c == L'\r' && i + 1 < text.size() && text[i + 1] == L'\n')
                ++i;
            switch (le)
            {
            case LineEnding::CRLF:
                converted += L"\r\n";
                break;
            case LineEnding::LF:
                converted += L'\n';
                break;
            case LineEnding::CR:
                converted += L'\r';
                break;
            }
        }
        else
            converted += c;
    }
    std::vector<BYTE> result;
    switch (enc)
    {
    case Encoding::UTF8BOM:
        result.push_back(0xEF);
        result.push_back(0xBB);
        result.push_back(0xBF);
        [[fallthrough]];
    case Encoding::UTF8:
    {
        int len = WideCharToMultiByte(CP_UTF8, 0, converted.c_str(), -1, nullptr, 0, nullptr, nullptr);
        if (len > 1)
        {
            size_t offset = result.size();
            result.resize(offset + len - 1);
            WideCharToMultiByte(CP_UTF8, 0, converted.c_str(), -1,
                                reinterpret_cast<char *>(result.data() + offset), len, nullptr, nullptr);
        }
        break;
    }
    case Encoding::UTF16LE:
        result.push_back(0xFF);
        result.push_back(0xFE);
        for (wchar_t c : converted)
        {
            result.push_back(static_cast<BYTE>(c & 0xFF));
            result.push_back(static_cast<BYTE>((c >> 8) & 0xFF));
        }
        break;
    case Encoding::UTF16BE:
        result.push_back(0xFE);
        result.push_back(0xFF);
        for (wchar_t c : converted)
        {
            result.push_back(static_cast<BYTE>((c >> 8) & 0xFF));
            result.push_back(static_cast<BYTE>(c & 0xFF));
        }
        break;
    case Encoding::ANSI:
    {
        int len = WideCharToMultiByte(CP_ACP, 0, converted.c_str(), -1, nullptr, 0, nullptr, nullptr);
        if (len > 1)
        {
            result.resize(len - 1);
            WideCharToMultiByte(CP_ACP, 0, converted.c_str(), -1,
                                reinterpret_cast<char *>(result.data()), len, nullptr, nullptr);
        }
        break;
    }
    }
    return result;
}
