#include "core/list_continuation.h"

#include <windows.h>
#include <iostream>

namespace
{
bool ExpectTrue(const wchar_t *name, bool condition)
{
    if (condition)
        return true;
    std::wcerr << L"[FAIL] " << name << L"\n";
    return false;
}

bool ExpectEq(const wchar_t *name, const std::wstring &actual, const std::wstring &expected)
{
    if (actual == expected)
        return true;
    std::wcerr << L"[FAIL] " << name << L" | expected='" << expected << L"' actual='" << actual << L"'\n";
    return false;
}
}

int RunListContinuationTests()
{
    bool ok = true;

    {
        const auto plan = BuildListContinuationPlan(L"1. item", 7);
        ok = ExpectTrue(L"Numeric list recognized", plan.matched) && ok;
        ok = ExpectTrue(L"Numeric list continues", !plan.exitListMode) && ok;
        ok = ExpectEq(L"Numeric continuation prefix", plan.continuationPrefix, L"2. ") && ok;
    }

    {
        const auto plan = BuildListContinuationPlan(L"009) task", 9);
        ok = ExpectTrue(L"Numeric padding recognized", plan.matched) && ok;
        ok = ExpectEq(L"Numeric padding continuation", plan.continuationPrefix, L"010) ") && ok;
    }

    {
        const auto plan = BuildListContinuationPlan(L"  - item", 8);
        ok = ExpectTrue(L"Bullet list recognized", plan.matched) && ok;
        ok = ExpectTrue(L"Bullet list continues", !plan.exitListMode) && ok;
        ok = ExpectEq(L"Bullet continuation prefix", plan.continuationPrefix, L"  - ") && ok;
    }

    {
        const auto plan = BuildListContinuationPlan(L"- ", 2);
        ok = ExpectTrue(L"Empty bullet exits list", plan.matched && plan.exitListMode) && ok;
    }

    {
        const auto plan = BuildListContinuationPlan(L"2. ", 3);
        ok = ExpectTrue(L"Empty number exits list", plan.matched && plan.exitListMode) && ok;
    }

    {
        const auto plan = BuildListContinuationPlan(L"2. ", 1);
        ok = ExpectTrue(L"Caret before marker spacing should not match", !plan.matched) && ok;
    }

    {
        const auto plan = BuildListContinuationPlan(L"plain text", 10);
        ok = ExpectTrue(L"Plain text should not match", !plan.matched) && ok;
    }

    {
        const auto plan = BuildListContinuationPlan(L"1.item", 6);
        ok = ExpectTrue(L"No-spacing number list recognized", plan.matched) && ok;
        ok = ExpectEq(L"No-spacing number continuation normalizes spacing", plan.continuationPrefix, L"2. ") && ok;
    }

    {
        const std::wstring pasted = L"3. c\r\n3. d\r\n3. e";
        const std::wstring normalized = NormalizeOrderedListForPaste(pasted, L"2. b");
        ok = ExpectEq(L"Paste duplicate numbering corrected with context continuation", normalized, L"3. c\r\n4. d\r\n5. e") && ok;
    }

    {
        const std::wstring pasted = L"7. x\n7. y";
        const std::wstring normalized = NormalizeOrderedListForPaste(pasted, L"plain text");
        ok = ExpectEq(L"Paste duplicate numbering corrected without context", normalized, L"7. x\n8. y") && ok;
    }

    {
        const std::wstring pasted = L"3. c\r\n   details about c\r\n3. d";
        const std::wstring normalized = NormalizeOrderedListForPaste(pasted, L"2. b");
        ok = ExpectEq(L"Paste keeps numbering sequence across wrapped list item lines", normalized, L"3. c\r\n   details about c\r\n4. d") && ok;
    }

    {
        const std::wstring pasted = L"3. c\r\n\r\n3. d";
        const std::wstring normalized = NormalizeOrderedListForPaste(pasted, L"2. b");
        ok = ExpectEq(L"Paste keeps numbering sequence across blank separator lines", normalized, L"3. c\r\n\r\n4. d") && ok;
    }

    {
        const std::wstring pasted = L"3. c\r\nRepro details line\r\n3. d";
        const std::wstring normalized = NormalizeOrderedListForPaste(pasted, L"2. b");
        ok = ExpectEq(L"Paste keeps numbering sequence across non-list explanatory lines when source marker repeats", normalized, L"3. c\r\nRepro details line\r\n4. d") && ok;
    }

    {
        const std::wstring pasted = L"3. c\r\nSection break\r\n1. fresh list";
        const std::wstring normalized = NormalizeOrderedListForPaste(pasted, L"2. b");
        ok = ExpectEq(L"Paste starts a new sequence after explanatory line when next source marker changes", normalized, L"3. c\r\nSection break\r\n1. fresh list") && ok;
    }

    {
        const std::wstring pasted = L"009) alpha\r\n009) beta";
        const std::wstring normalized = NormalizeOrderedListForPaste(pasted, L"008) root");
        ok = ExpectEq(L"Paste keeps padded numbering with ')' delimiter", normalized, L"009) alpha\r\n010) beta") && ok;
    }

    {
        const std::wstring pasted = L"1) alpha\r\n1) beta\r\n1. gamma\r\n1. delta";
        const std::wstring normalized = NormalizeOrderedListForPaste(pasted, L"0) root");
        ok = ExpectEq(L"Paste renumbers each number-symbol sequence independently", normalized, L"1) alpha\r\n2) beta\r\n1. gamma\r\n2. delta") && ok;
    }

    {
        const std::wstring pasted = L"- alpha\r\n- beta";
        const std::wstring normalized = NormalizeOrderedListForPaste(pasted, L"2. root");
        ok = ExpectEq(L"Paste leaves bullet list unchanged", normalized, pasted) && ok;
    }

    if (!ok)
        return 1;
    std::wcout << L"[PASS] list continuation tests\n";
    return 0;
}

int main()
{
    return RunListContinuationTests();
}

int WINAPI wWinMain(HINSTANCE, HINSTANCE, PWSTR, int)
{
    return RunListContinuationTests();
}
