#include "session_io.h"

bool SessionWriteAllBytes(HANDLE hFile, const void *data, DWORD bytes)
{
    const BYTE *cursor = reinterpret_cast<const BYTE *>(data);
    DWORD remaining = bytes;
    while (remaining > 0)
    {
        DWORD written = 0;
        if (!WriteFile(hFile, cursor, remaining, &written, nullptr))
            return false;
        if (written == 0)
            return false;
        cursor += written;
        remaining -= written;
    }
    return true;
}

bool SessionReadAllBytes(HANDLE hFile, void *data, DWORD bytes)
{
    BYTE *cursor = reinterpret_cast<BYTE *>(data);
    DWORD remaining = bytes;
    while (remaining > 0)
    {
        DWORD read = 0;
        if (!ReadFile(hFile, cursor, remaining, &read, nullptr))
            return false;
        if (read == 0)
            return false;
        cursor += read;
        remaining -= read;
    }
    return true;
}

bool SessionWriteUInt32(HANDLE hFile, DWORD value)
{
    return SessionWriteAllBytes(hFile, &value, sizeof(value));
}

bool SessionReadUInt32(HANDLE hFile, DWORD &value)
{
    return SessionReadAllBytes(hFile, &value, sizeof(value));
}

bool SessionWriteWideString(HANDLE hFile, const std::wstring &value, DWORD maxChars)
{
    if (value.size() > maxChars)
        return false;

    const DWORD charCount = static_cast<DWORD>(value.size());
    if (!SessionWriteUInt32(hFile, charCount))
        return false;
    if (charCount == 0)
        return true;
    return SessionWriteAllBytes(hFile, value.data(), charCount * sizeof(wchar_t));
}

bool SessionReadWideString(HANDLE hFile, std::wstring &value, DWORD maxChars)
{
    DWORD charCount = 0;
    if (!SessionReadUInt32(hFile, charCount))
        return false;
    if (charCount > maxChars)
        return false;

    value.clear();
    if (charCount == 0)
        return true;

    value.resize(charCount);
    return SessionReadAllBytes(hFile, value.data(), charCount * sizeof(wchar_t));
}

bool SessionWriteDocumentRecord(HANDLE hFile, const SessionDocumentRecord &record, DWORD maxChars)
{
    const DWORD modifiedFlag = record.modified ? 1u : 0u;
    if (!SessionWriteUInt32(hFile, modifiedFlag))
        return false;
    if (!SessionWriteUInt32(hFile, static_cast<DWORD>(record.encoding)))
        return false;
    if (!SessionWriteUInt32(hFile, static_cast<DWORD>(record.lineEnding)))
        return false;
    if (!SessionWriteWideString(hFile, record.filePath, maxChars))
        return false;
    if (!SessionWriteWideString(hFile, record.text, maxChars))
        return false;
    return true;
}

bool SessionReadDocumentRecord(HANDLE hFile, SessionDocumentRecord &record, DWORD maxChars)
{
    DWORD modifiedFlag = 0;
    DWORD encodingValue = 0;
    DWORD lineEndingValue = 0;
    if (!SessionReadUInt32(hFile, modifiedFlag))
        return false;
    if (!SessionReadUInt32(hFile, encodingValue))
        return false;
    if (!SessionReadUInt32(hFile, lineEndingValue))
        return false;
    if (!SessionReadWideString(hFile, record.filePath, maxChars))
        return false;
    if (!SessionReadWideString(hFile, record.text, maxChars))
        return false;

    record.modified = (modifiedFlag != 0);
    if (encodingValue <= static_cast<DWORD>(Encoding::ANSI))
        record.encoding = static_cast<Encoding>(encodingValue);
    else
        record.encoding = Encoding::UTF8;

    if (lineEndingValue <= static_cast<DWORD>(LineEnding::CR))
        record.lineEnding = static_cast<LineEnding>(lineEndingValue);
    else
        record.lineEnding = LineEnding::CRLF;

    return true;
}
