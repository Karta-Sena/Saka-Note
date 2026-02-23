#include "versioning.h"

#include <algorithm>
#include <cwctype>
#include <vector>

namespace
{
std::vector<int> ParseVersionNumbers(const std::wstring &version)
{
    std::vector<int> numbers;
    int current = 0;
    bool hasDigit = false;

    for (wchar_t ch : version)
    {
        if (iswdigit(ch))
        {
            hasDigit = true;
            current = (current * 10) + (ch - L'0');
        }
        else if (ch == L'.')
        {
            numbers.push_back(hasDigit ? current : 0);
            current = 0;
            hasDigit = false;
        }
        else
        {
            break;
        }
    }

    if (hasDigit)
        numbers.push_back(current);

    return numbers;
}
}

std::wstring NormalizeVersionTag(const std::wstring &tag)
{
    size_t start = 0;
    while (start < tag.size() && !iswdigit(tag[start]))
        ++start;
    if (start >= tag.size())
        return {};

    size_t end = start;
    while (end < tag.size() && (iswdigit(tag[end]) || tag[end] == L'.'))
        ++end;
    if (end <= start)
        return {};
    return tag.substr(start, end - start);
}

int CompareVersions(const std::wstring &left, const std::wstring &right)
{
    std::vector<int> lv = ParseVersionNumbers(left);
    std::vector<int> rv = ParseVersionNumbers(right);
    const size_t count = (std::max)(lv.size(), rv.size());
    lv.resize(count, 0);
    rv.resize(count, 0);

    for (size_t i = 0; i < count; ++i)
    {
        if (lv[i] < rv[i])
            return -1;
        if (lv[i] > rv[i])
            return 1;
    }
    return 0;
}
