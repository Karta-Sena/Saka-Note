/*
  Technical Standard
  
  Direct2D graphics engine for high-performance native UI rendering.
  Handles resource lifecycle and provides primitives for hardware-accelerated visuals.
*/

#pragma once
#include <windows.h>
#include <d2d1.h>
#include <dwrite.h>
#include <wincodec.h>

namespace Graphics
{
    class Engine
    {
    public:
        Engine() = default;
        ~Engine() { Release(); }

        bool Initialize(HWND hwnd);
        bool Resize(UINT width, UINT height);
        void Release();

        ID2D1HwndRenderTarget* GetRenderTarget() const { return m_pRenderTarget; }
        ID2D1Factory* GetFactory() const { return m_pFactory; }
        IDWriteFactory* GetWriteFactory() const { return m_pWriteFactory; }

        void BeginDraw() { if (m_pRenderTarget) m_pRenderTarget->BeginDraw(); }
        HRESULT EndDraw() { return m_pRenderTarget ? m_pRenderTarget->EndDraw() : S_OK; }
        void Clear(COLORREF color) { 
            if (m_pRenderTarget) {
                m_pRenderTarget->Clear(D2D1::ColorF(GetRValue(color) / 255.0f, GetGValue(color) / 255.0f, GetBValue(color) / 255.0f));
            }
        }

    private:
        ID2D1Factory* m_pFactory = nullptr;
        ID2D1HwndRenderTarget* m_pRenderTarget = nullptr;
        IDWriteFactory* m_pWriteFactory = nullptr;
    };
}
