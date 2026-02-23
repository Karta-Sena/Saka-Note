#pragma once

#include <windows.h>
#include <string>

#include "types.h"

struct SessionDocumentRecord
{
    bool modified = false;
    Encoding encoding = Encoding::UTF8;
    LineEnding lineEnding = LineEnding::CRLF;
    std::wstring filePath;
    std::wstring text;
};

bool SessionWriteAllBytes(HANDLE hFile, const void *data, DWORD bytes);
bool SessionReadAllBytes(HANDLE hFile, void *data, DWORD bytes);

bool SessionWriteUInt32(HANDLE hFile, DWORD value);
bool SessionReadUInt32(HANDLE hFile, DWORD &value);

bool SessionWriteWideString(HANDLE hFile, const std::wstring &value, DWORD maxChars);
bool SessionReadWideString(HANDLE hFile, std::wstring &value, DWORD maxChars);

bool SessionWriteDocumentRecord(HANDLE hFile, const SessionDocumentRecord &record, DWORD maxChars);
bool SessionReadDocumentRecord(HANDLE hFile, SessionDocumentRecord &record, DWORD maxChars);
