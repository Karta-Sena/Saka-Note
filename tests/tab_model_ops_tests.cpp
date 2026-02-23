#include "modules/tab_model_ops.h"

#include <windows.h>
#include <algorithm>
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

std::wstring NormalizeCaseInsensitive(const std::wstring &path)
{
    std::wstring lowered = path;
    std::transform(lowered.begin(), lowered.end(), lowered.begin(), towlower);
    return lowered;
}

bool AlwaysPathExists(const std::wstring &)
{
    return true;
}
}

int RunTabModelOpsTests()
{
    bool ok = true;

    {
        std::vector<DocumentTabState> docs(3);
        docs[0].filePath = L"C:\\Temp\\A.txt";
        docs[1].filePath = L"C:\\Temp\\b.txt";
        docs[2].filePath = L"";

        const int idx = TabFindDocumentByPath(docs, L"c:\\temp\\B.TXT", NormalizeCaseInsensitive);
        ok = ExpectTrue(L"Find path case-insensitive", idx == 1) && ok;
    }

    {
        DocumentTabState doc;
        ok = ExpectTrue(L"Untitled empty detected", TabIsEmptyUntitled(doc)) && ok;
        doc.text = L"x";
        ok = ExpectTrue(L"Untitled non-empty rejected", !TabIsEmptyUntitled(doc)) && ok;
    }

    {
        std::vector<DocumentTabState> closed;
        DocumentTabState emptyDoc;
        TabPushClosedDocument(closed, emptyDoc, 2);
        ok = ExpectTrue(L"Empty closed doc not pushed", closed.empty()) && ok;

        DocumentTabState d1;
        d1.text = L"1";
        DocumentTabState d2;
        d2.text = L"2";
        DocumentTabState d3;
        d3.text = L"3";
        TabPushClosedDocument(closed, d1, 2);
        TabPushClosedDocument(closed, d2, 2);
        TabPushClosedDocument(closed, d3, 2);
        ok = ExpectTrue(L"Closed doc max cap enforced", closed.size() == 2) && ok;
        if (closed.size() == 2)
            ok = ExpectTrue(L"Closed docs keep newest entries", closed[0].text == L"2" && closed[1].text == L"3") && ok;
    }

    {
        std::vector<DocumentTabState> docs(2);
        docs[0].filePath = L"C:\\Temp\\Large.txt";
        docs[0].modified = false;
        docs[0].text.assign(300 * 1024, L'x');
        docs[0].sourceBytes = docs[0].text.size() * sizeof(wchar_t);

        docs[1].filePath = L"C:\\Temp\\Active.txt";
        docs[1].text = L"keep";

        const bool compacted = TabCompactDocumentTextIfEligible(docs, 0, 1, 256 * 1024, AlwaysPathExists);
        ok = ExpectTrue(L"Large inactive doc compacted", compacted) && ok;
        ok = ExpectTrue(L"Compacted text cleared", docs[0].text.empty()) && ok;
        ok = ExpectTrue(L"Compacted flag set", docs[0].needsReloadFromDisk) && ok;
    }

    {
        std::vector<DocumentTabState> docs(4);
        docs[0].filePath = L"C:\\Temp\\A.txt";
        docs[1].filePath = L"c:\\temp\\a.txt";
        docs[2].filePath = L"C:\\Temp\\B.txt";
        docs[3].filePath = L"";

        std::vector<std::wstring> paths;
        int activePathIndex = -1;
        TabBuildPathSessionFallback(docs, 1, paths, activePathIndex, NormalizeCaseInsensitive);

        ok = ExpectTrue(L"Fallback de-duplicates paths", paths.size() == 2) && ok;
        if (paths.size() == 2)
            ok = ExpectTrue(L"Fallback active index tracks deduped entry", activePathIndex == 0) && ok;
    }

    if (!ok)
        return 1;
    std::wcout << L"[PASS] tab model ops tests\n";
    return 0;
}

int main()
{
    return RunTabModelOpsTests();
}

int WINAPI wWinMain(HINSTANCE, HINSTANCE, PWSTR, int)
{
    return RunTabModelOpsTests();
}
