#include "graphics_engine.h"

namespace Graphics
{
    bool Engine::Initialize(HWND hwnd)
    {
        HRESULT hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_pFactory);
        if (FAILED(hr)) return false;

        RECT rc;
        GetClientRect(hwnd, &rc);
        hr = m_pFactory->CreateHwndRenderTarget(
            D2D1::RenderTargetProperties(),
            D2D1::HwndRenderTargetProperties(hwnd, D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top)),
            &m_pRenderTarget
        );
        if (FAILED(hr)) return false;

        hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(&m_pWriteFactory));
        return SUCCEEDED(hr);
    }

    bool Engine::Resize(UINT width, UINT height)
    {
        if (!m_pRenderTarget)
            return false;
        if (width == 0 || height == 0)
            return true;
        return SUCCEEDED(m_pRenderTarget->Resize(D2D1::SizeU(width, height)));
    }

    void Engine::Release()
    {
        if (m_pWriteFactory) { m_pWriteFactory->Release(); m_pWriteFactory = nullptr; }
        if (m_pRenderTarget) { m_pRenderTarget->Release(); m_pRenderTarget = nullptr; }
        if (m_pFactory) { m_pFactory->Release(); m_pFactory = nullptr; }
    }
}
