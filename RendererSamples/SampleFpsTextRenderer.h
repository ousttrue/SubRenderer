#pragma once
#include "IRenderer.h"
#include <wrl/implements.h>
#include <string>
#include <dwrite_3.h>
#include <d2d1_3.h>


namespace d3d11
{
    // Direct2D および DirectWrite を使用して、画面右下隅に現在の FPS 値を描画します。
    class SampleFpsTextRenderer :
        public Microsoft::WRL::RuntimeClass<
        Microsoft::WRL::RuntimeClassFlags<Microsoft::WRL::ClassicCom>, IRenderer, IRendererWindowCallback, IRendererWindowMouseCallback>
    {
        Microsoft::WRL::ComPtr<ID2D1Factory3>		m_d2dFactory;
        Microsoft::WRL::ComPtr<IDWriteFactory3>		m_dwriteFactory;
        int m_width = 0;
        int m_height = 0;
        float m_dpi=75.0f;

        // テキスト レンダリングに関連するリソース。
        std::wstring                                    m_text;
        DWRITE_TEXT_METRICS	                            m_textMetrics;
        Microsoft::WRL::ComPtr<ID2D1SolidColorBrush>    m_whiteBrush;
        Microsoft::WRL::ComPtr<ID2D1DrawingStateBlock1> m_stateBlock;
        Microsoft::WRL::ComPtr<IDWriteTextLayout3>      m_textLayout;
        Microsoft::WRL::ComPtr<IDWriteTextFormat2>      m_textFormat;

        Microsoft::WRL::ComPtr<ID2D1Device2>		m_d2dDevice;
        Microsoft::WRL::ComPtr<ID2D1DeviceContext2>	m_d2dContext;
        Microsoft::WRL::ComPtr<ID2D1Bitmap1>		m_d2dTargetBitmap;

        int m_frameCount=0;
        int m_lastSeconds=-1;
        int m_fps=0;

    public:
        STDMETHODIMP RuntimeClassInitialize();

        STDMETHODIMP CreateDeviceDependentResources(ID3D11Device *pDevice)override;
        STDMETHODIMP SetTexture(ID3D11Texture2D *pTexture)override;
        STDMETHODIMP SetSize(int width, int height)override;

        STDMETHODIMP Update(double totalSeconds)override;
        STDMETHODIMP Render(ID3D11Device *pDevice)override;

        STDMETHODIMP OnWindowSizeChanged(int width, int height)override
        {
            return SetSize(width, height);
        }
        STDMETHODIMP OnMouseLeftDown()override;
        STDMETHODIMP OnMouseLeftUp()override;
        STDMETHODIMP OnMouseMiddleDown()override;
        STDMETHODIMP OnMouseMiddleUp()override;
        STDMETHODIMP OnMouseRightDown()override;
        STDMETHODIMP OnMouseRightUp()override;
        STDMETHODIMP OnMouseMove(int width, int height)override;
        STDMETHODIMP OnMouseWheel(int delta)override;
    private:
        HRESULT SetupTargetBitmap(ID3D11Texture2D *pTarget);
    };
}
