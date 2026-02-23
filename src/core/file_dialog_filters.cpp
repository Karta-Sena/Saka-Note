#include "file_dialog_filters.h"

#include "lang/lang.h"

#include <cwchar>

namespace
{
std::wstring BuildDialogFilter(const std::wstring &labelA,
                               const wchar_t *patternA,
                               const std::wstring &labelB,
                               const wchar_t *patternB)
{
    std::wstring filter;
    filter.reserve(labelA.size() + labelB.size() + wcslen(patternA) + wcslen(patternB) + 5);

    filter += labelA;
    filter.push_back(L'\0');
    filter += patternA;
    filter.push_back(L'\0');
    filter += labelB;
    filter.push_back(L'\0');
    filter += patternB;
    filter.push_back(L'\0');
    filter.push_back(L'\0');

    return filter;
}
}

std::wstring BuildTextDocumentsFilter(const LangStrings &lang)
{
    return BuildDialogFilter(lang.filterTextDocuments, L"*.txt", lang.filterAllFiles, L"*.*");
}

std::wstring BuildIconFilesFilter(const LangStrings &lang)
{
    return BuildDialogFilter(lang.filterIconFiles, L"*.ico", lang.filterAllFiles, L"*.*");
}

std::wstring BuildImageFilesFilter(const LangStrings &lang)
{
    return BuildDialogFilter(lang.filterImageFiles, L"*.png;*.jpg;*.jpeg;*.bmp;*.gif", lang.filterAllFiles, L"*.*");
}
