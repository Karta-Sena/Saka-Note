#pragma once

#include <windows.h>
#include <d2d1.h>
#include <cmath>
#include <vector>

namespace UI::Squircle
{
inline int NormalizeSamplesPerQuarter(int samplesPerQuarter)
{
    return (samplesPerQuarter < 4) ? 4 : samplesPerQuarter;
}

inline double SignedPow(double value, double power)
{
    return std::copysign(std::pow(std::abs(value), power), value);
}

inline bool FillSuperellipseDc(HDC hdc, const RECT &rc, COLORREF color, double exponent = 4.0, int samplesPerQuarter = 24)
{
    if (!hdc || exponent <= 0.0)
        return false;

    const int width = rc.right - rc.left;
    const int height = rc.bottom - rc.top;
    if (width <= 1 || height <= 1)
        return false;

    constexpr double kPi = 3.14159265358979323846;
    const int normalizedSamples = NormalizeSamplesPerQuarter(samplesPerQuarter);
    const int totalSamples = normalizedSamples * 4;
    const double invExp = 2.0 / exponent;

    const double a = static_cast<double>(width) * 0.5;
    const double b = static_cast<double>(height) * 0.5;
    const double cx = static_cast<double>(rc.left) + a;
    const double cy = static_cast<double>(rc.top) + b;

    std::vector<POINT> points;
    points.reserve(static_cast<size_t>(totalSamples));
    for (int i = 0; i < totalSamples; ++i)
    {
        const double t = (2.0 * kPi * static_cast<double>(i)) / static_cast<double>(totalSamples);
        const double x = SignedPow(std::cos(t), invExp);
        const double y = SignedPow(std::sin(t), invExp);

        POINT p{};
        p.x = static_cast<LONG>(std::lround(cx + a * x));
        p.y = static_cast<LONG>(std::lround(cy + b * y));
        points.push_back(p);
    }

    HBRUSH hBrush = CreateSolidBrush(color);
    if (!hBrush)
        return false;

    HPEN hNullPen = static_cast<HPEN>(GetStockObject(NULL_PEN));
    HGDIOBJ oldPen = SelectObject(hdc, hNullPen);
    HGDIOBJ oldBrush = SelectObject(hdc, hBrush);
    if (!points.empty())
        Polygon(hdc, points.data(), static_cast<int>(points.size()));
    SelectObject(hdc, oldBrush);
    SelectObject(hdc, oldPen);
    DeleteObject(hBrush);
    return !points.empty();
}

inline HRESULT CreateSuperellipsePathGeometry(ID2D1Factory *factory,
                                              const D2D1_RECT_F &rc,
                                              ID2D1PathGeometry **outGeometry,
                                              float exponent = 4.0f,
                                              int samplesPerQuarter = 16)
{
    if (!factory || !outGeometry || exponent <= 0.0f)
        return E_INVALIDARG;
    *outGeometry = nullptr;

    const float width = rc.right - rc.left;
    const float height = rc.bottom - rc.top;
    if (width <= 1.0f || height <= 1.0f)
        return E_INVALIDARG;

    constexpr float kPi = 3.14159265358979323846f;
    const int normalizedSamples = NormalizeSamplesPerQuarter(samplesPerQuarter);
    const int totalSamples = normalizedSamples * 4;
    const float invExp = 2.0f / exponent;

    const float a = width * 0.5f;
    const float b = height * 0.5f;
    const float cx = rc.left + a;
    const float cy = rc.top + b;

    std::vector<D2D1_POINT_2F> points;
    points.reserve(static_cast<size_t>(totalSamples));
    for (int i = 0; i < totalSamples; ++i)
    {
        const float t = (2.0f * kPi * static_cast<float>(i)) / static_cast<float>(totalSamples);
        const float x = static_cast<float>(SignedPow(std::cos(t), invExp));
        const float y = static_cast<float>(SignedPow(std::sin(t), invExp));
        points.push_back(D2D1::Point2F(cx + a * x, cy + b * y));
    }

    if (points.size() < 4)
        return E_FAIL;

    ID2D1PathGeometry *geometry = nullptr;
    HRESULT hr = factory->CreatePathGeometry(&geometry);
    if (FAILED(hr) || !geometry)
        return FAILED(hr) ? hr : E_FAIL;

    ID2D1GeometrySink *sink = nullptr;
    hr = geometry->Open(&sink);
    if (FAILED(hr) || !sink)
    {
        geometry->Release();
        return FAILED(hr) ? hr : E_FAIL;
    }

    sink->SetFillMode(D2D1_FILL_MODE_WINDING);
    sink->BeginFigure(points.front(), D2D1_FIGURE_BEGIN_FILLED);
    sink->AddLines(points.data() + 1, static_cast<UINT32>(points.size() - 1));
    sink->EndFigure(D2D1_FIGURE_END_CLOSED);
    hr = sink->Close();
    sink->Release();

    if (FAILED(hr))
    {
        geometry->Release();
        return hr;
    }

    *outGeometry = geometry;
    return S_OK;
}

inline bool FillSuperellipseD2D(ID2D1RenderTarget *target,
                                ID2D1Factory *factory,
                                const D2D1_RECT_F &rc,
                                ID2D1Brush *brush,
                                float exponent = 4.0f,
                                int samplesPerQuarter = 16)
{
    if (!target || !factory || !brush)
        return false;

    ID2D1PathGeometry *geometry = nullptr;
    const HRESULT hr = CreateSuperellipsePathGeometry(factory, rc, &geometry, exponent, samplesPerQuarter);
    if (FAILED(hr) || !geometry)
        return false;

    target->FillGeometry(geometry, brush);
    geometry->Release();
    return true;
}
} // namespace UI::Squircle
 
namespace Squircle = UI::Squircle;

