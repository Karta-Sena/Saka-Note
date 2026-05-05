#include "custom_scrollbar.h"
#include "design_system.h"
#include "editor.h"
#include "theme.h"
#include <commctrl.h>
#include <richedit.h>
#include <windowsx.h>
#include <algorithm>
#include <cmath>

namespace UI {

namespace
{
constexpr UINT_PTR kScrollbarAnimationTimerId = 0x5A11;
constexpr float kThumbWidthIdle = 4.0f;
constexpr float kThumbWidthHover = 6.0f;
constexpr float kThumbWidthDrag = 8.0f;
constexpr float kThumbOpacityIdle = 0.28f;
constexpr float kThumbOpacityHover = 0.74f;
constexpr float kThumbOpacityDrag = 0.92f;
}

bool CustomScrollbar::RegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wc{};
    wc.cbSize = sizeof(wc);
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"TechnicalStandardCustomScrollbar";
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    return ::RegisterClassExW(&wc) != 0;
}

HWND CustomScrollbar::Create(HWND hwndParent, HWND hwndTarget)
{
    HWND hwnd = CreateWindowExW(0, L"TechnicalStandardCustomScrollbar", nullptr,
        WS_CHILD | WS_VISIBLE, 0, 0, 0, 0,
        hwndParent, nullptr, GetModuleHandle(nullptr), hwndTarget);
    return hwnd;
}

void CustomScrollbar::SetTarget(HWND hwndScrollbar, HWND hwndTarget)
{
    if (!hwndScrollbar)
        return;
    State* state = reinterpret_cast<State*>(GetWindowLongPtrW(hwndScrollbar, GWLP_USERDATA));
    if (!state)
        return;
    state->hwndTarget = hwndTarget;
    InvalidateRect(hwndScrollbar, nullptr, FALSE);
}

LRESULT CALLBACK CustomScrollbar::WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    State* state = reinterpret_cast<State*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));

    auto animateVisualState = [&](bool immediate)
    {
        if (!state)
            return;

        const float targetOpacity = state->isDragging ? kThumbOpacityDrag : (state->isHovered ? kThumbOpacityHover : kThumbOpacityIdle);
        const float targetWidth = state->isDragging ? kThumbWidthDrag : (state->isHovered ? kThumbWidthHover : kThumbWidthIdle);

        if (immediate)
        {
            state->opacitySpring.Reset(targetOpacity);
            state->widthSpring.Reset(targetWidth);
            KillTimer(hwnd, kScrollbarAnimationTimerId);
            return;
        }

        state->opacitySpring.target = targetOpacity;
        state->widthSpring.target = targetWidth;

        state->lastUpdate = GetTickCount64();
        SetTimer(hwnd, kScrollbarAnimationTimerId, 16, nullptr);
    };

    switch (msg) {
    case WM_CREATE: {
        State* createdState = new State();
        createdState->hwndTarget = reinterpret_cast<HWND>(reinterpret_cast<CREATESTRUCT*>(lParam)->lpCreateParams);
        createdState->engine.Initialize(hwnd);
        createdState->lastUpdate = GetTickCount64();
        SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(createdState));
        return 0;
    }
    case WM_SIZE: {
        if (state) state->engine.Resize(LOWORD(lParam), HIWORD(lParam));
        return 0;
    }
    case WM_MOUSEMOVE: {
        if (!state) return 0;
        if (!state->isHovered) {
            state->isHovered = true;
            TRACKMOUSEEVENT tme = { sizeof(tme), TME_LEAVE, hwnd, 0 };
            TrackMouseEvent(&tme);
            animateVisualState(false);
            InvalidateRect(hwnd, nullptr, FALSE);
        }
        
        if (state->isDragging && state->hwndTarget) {
            RECT rc;
            GetClientRect(hwnd, &rc);
            POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };

            const int trackHeight = rc.bottom - rc.top;
            const float thumbH = std::max(20.0f, state->thumbHeight);
            const int usableTrack = std::max(1, trackHeight - static_cast<int>(thumbH));

            // Anchor drag to where within the thumb the user originally clicked.
            // This prevents the thumb from jumping to center-under-cursor.
            const int anchoredY = pt.y - state->dragThumbOffset;
            float scrollRatio = static_cast<float>(anchoredY) / static_cast<float>(usableTrack);
            scrollRatio = std::clamp(scrollRatio, 0.0f, 1.0f);

            // Use EM_GETFIRSTVISIBLELINE as the ground truth — avoids relying on
            // GetScrollInfo which can return stale data when the native scrollbar is hidden.
            const int totalLines = std::max(1, (int)SendMessageW(state->hwndTarget, EM_GETLINECOUNT, 0, 0));
            const int maxFirstLine = std::max(0, totalLines - 1);
            const int targetLine = static_cast<int>(scrollRatio * maxFirstLine + 0.5f);
            const int firstVisible = (int)SendMessageW(state->hwndTarget, EM_GETFIRSTVISIBLELINE, 0, 0);
            const int lineDelta = std::clamp(targetLine - firstVisible, -4096, 4096);

            if (lineDelta != 0)
            {
                SendMessageW(state->hwndTarget, EM_LINESCROLL, 0, static_cast<LPARAM>(lineDelta));
                RedrawWindow(state->hwndTarget, nullptr, nullptr, RDW_INVALIDATE | RDW_NOERASE);
            }
            InvalidateRect(hwnd, nullptr, FALSE);
        }
        return 0;
    }
    case WM_MOUSELEAVE:
        if (state) {
            state->isHovered = false;
            animateVisualState(false);
            if (!state->isDragging) InvalidateRect(hwnd, nullptr, FALSE);
        }
        return 0;
    case WM_LBUTTONDOWN:
        if (state && state->hwndTarget) {
            POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
            state->isDragging = true;
            state->dragStartY = pt.y;
            state->dragStartFirstLine = (int)SendMessageW(state->hwndTarget, EM_GETFIRSTVISIBLELINE, 0, 0);
            state->dragTotalLines = std::max(1, (int)SendMessageW(state->hwndTarget, EM_GETLINECOUNT, 0, 0));
            // Capture where within the thumb the user clicked (avoid jump)
            state->dragThumbOffset = pt.y - (int)state->thumbPos;
            SetCapture(hwnd);
            animateVisualState(false);
            InvalidateRect(hwnd, nullptr, FALSE);
        }
        return 0;
    case WM_LBUTTONUP:
        if (state) {
            state->isDragging = false;
            ReleaseCapture();
            if (state->hwndTarget)
                SetFocus(state->hwndTarget);
            animateVisualState(false);
            InvalidateRect(hwnd, nullptr, FALSE);
        }
        return 0;
    case WM_TIMER:
        if (wParam == kScrollbarAnimationTimerId && state)
        {
            ULONGLONG now = GetTickCount64();
            float dt = static_cast<float>(now - state->lastUpdate) / 1000.0f;
            state->lastUpdate = now;

            // Cap dt to avoid explosion on massive lag
            dt = std::min(dt, 0.1f);

            state->opacitySpring.Update(dt);
            state->widthSpring.Update(dt);

            InvalidateRect(hwnd, nullptr, FALSE);

            if (state->opacitySpring.IsSettled() && state->widthSpring.IsSettled())
            {
                KillTimer(hwnd, kScrollbarAnimationTimerId);
            }
            return 0;
        }
        break;
    case WM_MOUSEWHEEL:
    case WM_MOUSEHWHEEL:
        if (state && state->hwndTarget) {
            if (msg == WM_MOUSEWHEEL)
                ScrollEditorFromMouseWheel(state->hwndTarget, wParam);
            else
                SendMessageW(state->hwndTarget, msg, wParam, lParam);
            InvalidateRect(hwnd, nullptr, FALSE);
            return 0;
        }
        return 0;
    case WM_PAINT: {
        if (!state) return 0;
        PAINTSTRUCT ps;
        BeginPaint(hwnd, &ps);
        
        auto context = state->engine.GetDeviceContext();
        if (context) {
            RECT rc;
            GetClientRect(hwnd, &rc);
            
            SCROLLINFO si{};
            si.cbSize = sizeof(si);
            si.fMask = SIF_ALL;
            if (state->hwndTarget)
                GetScrollInfo(state->hwndTarget, SB_VERT, &si);
            
            float trackHeight = (float)(rc.bottom - rc.top);
            float thumbHeight = si.nMax > si.nMin ? (trackHeight * si.nPage) / (si.nMax - si.nMin + 1) : 0;
            thumbHeight = std::max(thumbHeight, 20.0f); // Min height
            
            float scrollRange = (float)(si.nMax - si.nMin - (int)si.nPage + 1);
            float thumbY = scrollRange > 0 ? (si.nPos * (trackHeight - thumbHeight)) / scrollRange : 0;
            state->thumbPos = thumbY;
            state->thumbHeight = thumbHeight;

            context->BeginDraw();
            
            const bool dark = IsDarkMode();
            D2D1_COLOR_F bgColor = Graphics::Engine::ColorToD2D(dark ? DesignSystem::Color::kDarkBg : DesignSystem::Color::kLightBg, 1.0f);
            context->Clear(bgColor);
            
            const float opacity = std::clamp(state->opacitySpring.x, 0.0f, 1.0f);
            D2D1_COLOR_F color = Graphics::Engine::ColorToD2D(DesignSystem::Color::kAccent, opacity);
            
            ID2D1SolidColorBrush* brush = nullptr;
            context->CreateSolidColorBrush(color, &brush);
            
            if (brush) {
                float width = (float)(rc.right - rc.left);
                float barWidth = std::clamp(state->widthSpring.x, 3.0f, std::max(3.0f, width - 1.0f));
                float xOffset = (width - barWidth) / 2.0f;
                
                D2D1_RECT_F rect = D2D1::RectF(xOffset, thumbY, xOffset + barWidth, thumbY + thumbHeight);
                D2D1_ROUNDED_RECT rounded = D2D1::RoundedRect(rect, barWidth * 0.5f, barWidth * 0.5f);
                context->FillRoundedRectangle(rounded, brush);

                if (state->isDragging)
                {
                    ID2D1SolidColorBrush* glowBrush = nullptr;
                    D2D1_COLOR_F glowColor = Graphics::Engine::ColorToD2D(DesignSystem::Color::kAccent, opacity * 0.24f);
                    context->CreateSolidColorBrush(glowColor, &glowBrush);
                    if (glowBrush)
                    {
                        D2D1_RECT_F glowRect = D2D1::RectF(rect.left - 1.0f, rect.top - 1.0f, rect.right + 1.0f, rect.bottom + 1.0f);
                        D2D1_ROUNDED_RECT glowRounded = D2D1::RoundedRect(glowRect, (barWidth + 2.0f) * 0.5f, (barWidth + 2.0f) * 0.5f);
                        context->FillRoundedRectangle(glowRounded, glowBrush);
                        glowBrush->Release();
                    }
                }
                brush->Release();
            }
            
            context->EndDraw();
        }
        
        EndPaint(hwnd, &ps);
        return 0;
    }
    case WM_DESTROY:
        KillTimer(hwnd, kScrollbarAnimationTimerId);
        delete state;
        return 0;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

} // namespace UI
