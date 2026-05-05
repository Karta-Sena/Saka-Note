#pragma once
#include <windows.h>
#include <d2d1_1.h>
#include "graphics_engine.h"
#include "../core/spring_solver.h"

namespace UI {

class CustomScrollbar {
public:
    static bool RegisterClass(HINSTANCE hInstance);
    static HWND Create(HWND hwndParent, HWND hwndTarget);
    static void SetTarget(HWND hwndScrollbar, HWND hwndTarget);

private:
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
    
    struct State {
        HWND hwndTarget;
        Graphics::Engine engine;
        bool isHovered = false;
        bool isDragging = false;
        float thumbPos = 0.0f;
        float thumbHeight = 0.0f;
        Core::Spring opacitySpring{0.28f};
        Core::Spring widthSpring{4.0f};
        ULONGLONG lastUpdate = 0;
        // Drag anchor: captured at WM_LBUTTONDOWN to avoid thumb jump
        int dragStartY = 0;
        int dragStartFirstLine = 0;
        int dragTotalLines = 1;
        int dragThumbOffset = 0; // Y offset within thumb where user clicked
    };
};

} // namespace UI
