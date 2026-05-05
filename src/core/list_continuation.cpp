#include "list_continuation.h"

#include <cwctype>
#include <limits>
#include <vector>

namespace
{
bool IsListSpacingChar(wchar_t ch)
{
    return ch == L' ' || ch == L'\t';
}

size_t CountListIndentLength(const std::wstring &lineText)
{
    size_t indentLength = 0;
    while (indentLength < lineText.size() && IsListSpacingChar(lineText[indentLength]))
        ++indentLength;
    return indentLength;
}

bool IsOrderedListContinuationLine(const std::wstring &lineText, size_t sequenceIndent)
{
    if (lineText.empty())
        return true;

    const size_t indentLength = CountListIndentLength(lineText);
    if (indentLength >= lineText.size())
        return true;

    return indentLength > sequenceIndent;
}

std::wstring ContinuationSpacing(const std::wstring &body, size_t spacingStart, size_t spacingEnd)
{
    if (spacingEnd > spacingStart)
        return body.substr(spacingStart, spacingEnd - spacingStart);
    return L" ";
}

bool IsBulletMarker(wchar_t ch)
{
    return ch == L'-' || ch == L'*' || ch == L'+' || ch == L'\x2022';
}

struct OrderedListMarker
{
    bool matched = false;
    size_t indentLength = 0;
    size_t numberStart = 0;
    size_t numberEnd = 0;
    size_t digitsWidth = 0;
    unsigned long long number = 0;
    wchar_t delimiter = L'.';
    bool zeroPadded = false;
};

bool TryParseOrderedListMarker(const std::wstring &lineText, OrderedListMarker &outMarker)
{
    outMarker = {};

    const size_t indentLength = CountListIndentLength(lineText);

    if (indentLength >= lineText.size())
        return false;

    size_t digitsEnd = indentLength;
    while (digitsEnd < lineText.size() && iswdigit(static_cast<wint_t>(lineText[digitsEnd])))
        ++digitsEnd;
    if (digitsEnd == indentLength || digitsEnd >= lineText.size())
        return false;

    const wchar_t delimiter = lineText[digitsEnd];
    if (delimiter != L'.' && delimiter != L')')
        return false;

    unsigned long long number = 0;
    for (size_t i = indentLength; i < digitsEnd; ++i)
    {
        const unsigned long long digit = static_cast<unsigned long long>(lineText[i] - L'0');
        if (number > (std::numeric_limits<unsigned long long>::max() - digit) / 10)
            return false;
        number = (number * 10) + digit;
    }

    outMarker.matched = true;
    outMarker.indentLength = indentLength;
    outMarker.numberStart = indentLength;
    outMarker.numberEnd = digitsEnd;
    outMarker.digitsWidth = digitsEnd - indentLength;
    outMarker.number = number;
    outMarker.delimiter = delimiter;
    outMarker.zeroPadded = (outMarker.digitsWidth > 1 && lineText[indentLength] == L'0');
    return true;
}

std::wstring FormatOrderedListNumber(unsigned long long value, size_t width, bool zeroPadded)
{
    std::wstring formatted = std::to_wstring(value);
    if (zeroPadded && width > formatted.size())
        formatted.insert(0, width - formatted.size(), L'0');
    return formatted;
}

struct PasteLine
{
    std::wstring content;
    std::wstring ending;
};

std::vector<PasteLine> SplitLinesWithEndings(const std::wstring &text)
{
    std::vector<PasteLine> lines;
    if (text.empty())
        return lines;

    size_t cursor = 0;
    while (cursor < text.size())
    {
        const size_t contentStart = cursor;
        while (cursor < text.size() && text[cursor] != L'\r' && text[cursor] != L'\n')
            ++cursor;

        PasteLine line{};
        line.content = text.substr(contentStart, cursor - contentStart);

        if (cursor < text.size())
        {
            if (text[cursor] == L'\r' && (cursor + 1) < text.size() && text[cursor + 1] == L'\n')
            {
                line.ending = L"\r\n";
                cursor += 2;
            }
            else
            {
                line.ending.assign(1, text[cursor]);
                ++cursor;
            }
        }

        lines.push_back(std::move(line));
    }

    return lines;
}

std::wstring JoinLinesWithEndings(const std::vector<PasteLine> &lines)
{
    std::wstring text;
    for (const PasteLine &line : lines)
    {
        text += line.content;
        text += line.ending;
    }
    return text;
}
}

ListContinuationPlan BuildListContinuationPlan(const std::wstring &lineText, size_t caretOffsetInLine)
{
    ListContinuationPlan plan{};
    if (caretOffsetInLine > lineText.size())
        return plan;

    size_t indentLength = 0;
    while (indentLength < lineText.size() && IsListSpacingChar(lineText[indentLength]))
        ++indentLength;

    if (indentLength >= lineText.size())
        return plan;

    const std::wstring body = lineText.substr(indentLength);
    if (body.empty())
        return plan;

    if (IsBulletMarker(body[0]))
    {
        size_t spacingStart = 1;
        size_t spacingEnd = spacingStart;
        while (spacingEnd < body.size() && IsListSpacingChar(body[spacingEnd]))
            ++spacingEnd;

        const std::wstring itemContent = body.substr(spacingEnd);
        if (itemContent.empty())
        {
            if (caretOffsetInLine != lineText.size())
                return plan;
            plan.matched = true;
            plan.exitListMode = true;
            return plan;
        }

        if (caretOffsetInLine < (indentLength + spacingEnd))
            return plan;

        plan.matched = true;
        plan.continuationPrefix = lineText.substr(0, indentLength);
        plan.continuationPrefix.push_back(body[0]);
        plan.continuationPrefix += ContinuationSpacing(body, spacingStart, spacingEnd);
        return plan;
    }

    size_t digitsEnd = 0;
    while (digitsEnd < body.size() && iswdigit(static_cast<wint_t>(body[digitsEnd])))
        ++digitsEnd;
    if (digitsEnd == 0 || digitsEnd >= body.size())
        return plan;

    const wchar_t delimiter = body[digitsEnd];
    if (delimiter != L'.' && delimiter != L')')
        return plan;

    size_t spacingStart = digitsEnd + 1;
    size_t spacingEnd = spacingStart;
    while (spacingEnd < body.size() && IsListSpacingChar(body[spacingEnd]))
        ++spacingEnd;

    unsigned long long number = 0;
    for (size_t i = 0; i < digitsEnd; ++i)
    {
        const unsigned long long digit = static_cast<unsigned long long>(body[i] - L'0');
        if (number > (std::numeric_limits<unsigned long long>::max() - digit) / 10)
            return plan;
        number = (number * 10) + digit;
    }
    if (number == std::numeric_limits<unsigned long long>::max())
        return plan;

    const std::wstring itemContent = body.substr(spacingEnd);
    if (itemContent.empty())
    {
        if (caretOffsetInLine != lineText.size())
            return plan;
        plan.matched = true;
        plan.exitListMode = true;
        return plan;
    }

    if (caretOffsetInLine < (indentLength + spacingEnd))
        return plan;

    std::wstring nextNumberText = std::to_wstring(number + 1);
    if (digitsEnd > nextNumberText.size() && body[0] == L'0')
        nextNumberText.insert(0, digitsEnd - nextNumberText.size(), L'0');

    plan.matched = true;
    plan.continuationPrefix = lineText.substr(0, indentLength);
    plan.continuationPrefix.append(nextNumberText);
    plan.continuationPrefix.push_back(delimiter);
    plan.continuationPrefix += ContinuationSpacing(body, spacingStart, spacingEnd);
    return plan;
}

std::wstring NormalizeOrderedListForPaste(const std::wstring &pastedText, const std::wstring &contextLineText)
{
    if (pastedText.empty())
        return pastedText;

    std::vector<PasteLine> lines = SplitLinesWithEndings(pastedText);
    if (lines.empty())
        return pastedText;

    OrderedListMarker contextMarker{};
    const bool hasContextMarker = TryParseOrderedListMarker(contextLineText, contextMarker);

    bool changed = false;
    bool sequenceActive = false;
    bool sequenceBrokenByText = false;
    bool usedContext = false;
    size_t sequenceIndent = 0;
    wchar_t sequenceDelimiter = L'.';
    unsigned long long lastMarkerSourceNumber = 0;
    unsigned long long nextNumber = 0;

    for (PasteLine &line : lines)
    {
        OrderedListMarker marker{};
        if (!TryParseOrderedListMarker(line.content, marker))
        {
            if (sequenceActive && IsOrderedListContinuationLine(line.content, sequenceIndent))
                continue;

            if (sequenceActive)
                sequenceBrokenByText = true;
            continue;
        }

        const bool startNewSequence = !sequenceActive ||
                                      marker.indentLength != sequenceIndent ||
                                      marker.delimiter != sequenceDelimiter ||
                                      (sequenceBrokenByText && marker.number != lastMarkerSourceNumber);

        if (startNewSequence)
        {
            sequenceActive = true;
            sequenceIndent = marker.indentLength;
            sequenceDelimiter = marker.delimiter;

            if (!usedContext &&
                hasContextMarker &&
                contextMarker.indentLength == marker.indentLength &&
                contextMarker.delimiter == marker.delimiter &&
                contextMarker.number < std::numeric_limits<unsigned long long>::max())
            {
                nextNumber = contextMarker.number + 1;
                usedContext = true;
            }
            else
            {
                nextNumber = marker.number;
            }
        }
        else if (nextNumber < std::numeric_limits<unsigned long long>::max())
        {
            ++nextNumber;
        }
        sequenceBrokenByText = false;
        lastMarkerSourceNumber = marker.number;

        const std::wstring newNumberText = FormatOrderedListNumber(nextNumber, marker.digitsWidth, marker.zeroPadded);
        if (line.content.compare(marker.numberStart, marker.digitsWidth, newNumberText) != 0)
        {
            line.content.replace(marker.numberStart, marker.digitsWidth, newNumberText);
            changed = true;
        }
    }

    if (!changed)
        return pastedText;

    return JoinLinesWithEndings(lines);
}
