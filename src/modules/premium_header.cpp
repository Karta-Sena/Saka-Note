#include "premium_header.h"
#include "design_system.h"
#include "theme.h"
#include "lang/lang.h"
#include <algorithm>

namespace Premium
{
    Header::~Header()
    {
        if (m_pAccentBrush) m_pAccentBrush->Release();
        if (m_pTextBrush) m_pTextBrush->Release();
        if (m_pTextFormat) m_pTextFormat->Release();
    }

    bool Header::Initialize(Graphics::Engine* pEngine)
    {
        m_pEngine = pEngine;
        CreateResources();
        m_isInitialized = true;
        return true;
    }

    void Header::CreateResources()
    {
        if (!m_pEngine || !m_pEngine->GetRenderTarget()) return;

        auto pRT = m_pEngine->GetRenderTarget();
        auto pDW = m_pEngine->GetWriteFactory();

        pRT->CreateSolidColorBrush(D2D1::ColorF(0, 1.0f), &m_pTextBrush);
        pRT->CreateSolidColorBrush(D2D1::ColorF(0, 0.4f), &m_pAccentBrush);

        pDW->CreateTextFormat(
            DesignSystem::kUiFontPrimary,
            nullptr,
            DWRITE_FONT_WEIGHT_REGULAR,
            DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL,
            14.0f,
            L"en-us",
            &m_pTextFormat
        );
        
        if (m_pTextFormat) {
            m_pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
            m_pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
        }
    }

    void Header::Update(float dt)
    {
        if (m_isInitialized)
        {
            m_revealSpring.Update(dt);
            m_scaleSpring.Update(dt);
        }
    }

    void Header::StartReveal()
    {
        m_revealSpring.Reset(0.0f);
        m_revealSpring.target = 1.0f;
        
        m_scaleSpring.Reset(0.92f);
        m_scaleSpring.target = 1.0f;
        m_scaleSpring.stiffness = 180.0f; // Slightly softer pop
    }

    void Header::Render(const RECT& rect)
    {
        if (!m_isInitialized || !m_pEngine) return;

        auto pRT = m_pEngine->GetRenderTarget();
        if (!pRT) return;

        float alpha = m_revealSpring.x;
        float scale = m_scaleSpring.x;
        
        D2D1_RECT_F layoutRect = D2D1::RectF(
            static_cast<float>(rect.left),
            static_cast<float>(rect.top),
            static_cast<float>(rect.right),
            static_cast<float>(rect.bottom)
        );

        // Apply organic pop scale transform centered on the header
        D2D1::Matrix3x2F originalTransform;
        pRT->GetTransform(&originalTransform);
        
        D2D1_POINT_2F center = D2D1::Point2F(
            (layoutRect.left + layoutRect.right) * 0.5f,
            (layoutRect.top + layoutRect.bottom) * 0.5f
        );
        
        pRT->SetTransform(D2D1::Matrix3x2F::Scale(scale, scale, center) * originalTransform);

        if (m_pTextBrush) {
            bool dark = IsDarkMode();
            COLORREF cText = ThemeColorEditorText(dark);
            m_pTextBrush->SetColor(D2D1::ColorF(
                GetRValue(cText) / 255.0f,
                GetGValue(cText) / 255.0f,
                GetBValue(cText) / 255.0f
            ));

            const auto &lang = GetLangStrings();
            const std::wstring &wordmark = lang.appNameWordmark.empty() ? lang.appName : lang.appNameWordmark;

            // Render optical rim highlight for depth (subtle offset)
            m_pTextBrush->SetOpacity(alpha * 0.15f);
            D2D1_RECT_F rimRect = layoutRect;
            rimRect.top += 0.5f;
            rimRect.bottom += 0.5f;
            pRT->DrawTextW(wordmark.c_str(), static_cast<UINT32>(wordmark.length()), m_pTextFormat, rimRect, m_pTextBrush);

            // Primary logo text
            m_pTextBrush->SetOpacity(alpha);
            pRT->DrawTextW(wordmark.c_str(), static_cast<UINT32>(wordmark.length()), m_pTextFormat, layoutRect, m_pTextBrush);
        }

        pRT->SetTransform(originalTransform);
    }
}
