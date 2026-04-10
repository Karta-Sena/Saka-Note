#include "premium_header.h"
#include "design_system.h"
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

        // Colors from design system
        pRT->CreateSolidColorBrush(D2D1::ColorF(0xFFFFFF, 1.0f), &m_pTextBrush);
        pRT->CreateSolidColorBrush(D2D1::ColorF(0xFFFFFF, 0.4f), &m_pAccentBrush);

        pDW->CreateTextFormat(
            L"Akkurat Mono LL",
            nullptr,
            DWRITE_FONT_WEIGHT_BOLD,
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

    void Header::Update()
    {
        // Internal state updates if needed
    }

    void Header::StartReveal()
    {
        m_revealTransition.Start(0.0f, 1.0f, 800.0f); // 800ms reveal
    }

    void Header::Render(const RECT& rect)
    {
        if (!m_isInitialized || !m_pEngine) return;

        auto pRT = m_pEngine->GetRenderTarget();
        if (!pRT) return;

        float alpha = m_revealTransition.GetCurrentValue();
        
        D2D1_RECT_F layoutRect = D2D1::RectF(
            static_cast<float>(rect.left),
            static_cast<float>(rect.top),
            static_cast<float>(rect.right),
            static_cast<float>(rect.bottom)
        );

        // Render "SAKA NOTE" with reveal alpha
        if (m_pTextBrush) {
            m_pTextBrush->SetOpacity(alpha);
            pRT->DrawTextW(L"SAKA NOTE", 9, m_pTextFormat, layoutRect, m_pTextBrush);
        }
    }
}
