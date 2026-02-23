#pragma once

#include <string>

struct LangStrings;

std::wstring BuildTextDocumentsFilter(const LangStrings &lang);
std::wstring BuildIconFilesFilter(const LangStrings &lang);
std::wstring BuildImageFilesFilter(const LangStrings &lang);
