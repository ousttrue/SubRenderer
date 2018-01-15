//#include "pch.h"
#include "SampleFpsTextRenderer.h"
#include "DirectXHelper.h"
#include <dxgi1_3.h>

using namespace d3d11;
using namespace Microsoft::WRL;


STDMETHODIMP SampleFpsTextRenderer::RuntimeClassInitialize()
{
    // Direct2D リソースを初期化します。
    D2D1_FACTORY_OPTIONS options;
    ZeroMemory(&options, sizeof(D2D1_FACTORY_OPTIONS));

#if defined(_DEBUG)
    // プロジェクトがデバッグ ビルドに含まれている場合は、Direct2D デバッグを SDK レイヤーを介して有効にします。
    options.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
#endif

    // Direct2D ファクトリを初期化します。
    auto hr = D2D1CreateFactory(
        D2D1_FACTORY_TYPE_SINGLE_THREADED,
        __uuidof(ID2D1Factory3),
        &options,
        &m_d2dFactory
    );
    if (FAILED(hr)) {
        return hr;
    }

    // DirectWrite ファクトリを初期化します。
    hr = DWriteCreateFactory(
        DWRITE_FACTORY_TYPE_SHARED,
        __uuidof(IDWriteFactory3),
        &m_dwriteFactory
    );
    if (FAILED(hr)) {
        return hr;
    }

    hr = m_d2dFactory->CreateDrawingStateBlock(&m_stateBlock);
    if (FAILED(hr)) {
        return hr;
    }

    ComPtr<IDWriteTextFormat> textFormat;
    hr = m_dwriteFactory->CreateTextFormat(
        L"Segoe UI",
        nullptr,
        DWRITE_FONT_WEIGHT_LIGHT,
        DWRITE_FONT_STYLE_NORMAL,
        DWRITE_FONT_STRETCH_NORMAL,
        32.0f,
        L"en-US",
        &textFormat
    );
    if (FAILED(hr)) {
        return hr;
    }

    hr = textFormat.As(&m_textFormat);
    if (FAILED(hr)) {
        return hr;
    }

    hr = m_textFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
    if (FAILED(hr)) {
        return hr;
    }

    return S_OK;
}

STDMETHODIMP SampleFpsTextRenderer::CreateDeviceDependentResources(ID3D11Device *pDevice)
{
    auto hr = S_OK;

    ComPtr<IDXGIDevice3> dxgiDevice;
    hr = pDevice->QueryInterface(IID_PPV_ARGS(&dxgiDevice));
    if (FAILED(hr)) {
        return hr;
    }

    hr = m_d2dFactory->CreateDevice(dxgiDevice.Get(), &m_d2dDevice);
    if (FAILED(hr)) {
        return hr;
    }

    hr = m_d2dDevice->CreateDeviceContext(
        D2D1_DEVICE_CONTEXT_OPTIONS_NONE,
        &m_d2dContext
    );

    hr = m_d2dContext->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &m_whiteBrush);
    if (FAILED(hr)) {
        return hr;
    }

    return S_OK;
}

STDMETHODIMP SampleFpsTextRenderer::SetTexture(ID3D11Texture2D *pTexture)
{
    return S_OK;
}

STDMETHODIMP SampleFpsTextRenderer::SetSize(int width, int height)
{
    m_width = width;
    m_height = height;

    m_d2dTargetBitmap.Reset();

    return S_OK;
}

STDMETHODIMP SampleFpsTextRenderer::Update(double totalSeconds)
{
    m_frameCount++;
    auto seconds = (int)totalSeconds;
    if (m_lastSeconds == seconds) {
        return S_OK;
    }

    m_lastSeconds = seconds;
    m_fps = m_frameCount;
    m_frameCount = 0;

    // 表示するテキストを更新します。
    m_text = (m_fps > 0) ? std::to_wstring(m_fps) + L" FPS" : L" - FPS";

    ComPtr<IDWriteTextLayout> textLayout;
    auto hr = m_dwriteFactory->CreateTextLayout(
        m_text.c_str(),
        (UINT)m_text.length(),
        m_textFormat.Get(),
        240.0f, // 入力テキストの最大幅。
        50.0f, // 入力テキストの最大高さ。
        &textLayout
    );
    if (FAILED(hr)) {
        return hr;
    }

    hr = textLayout.As(&m_textLayout);
    if (FAILED(hr)) {
        return hr;
    }

    hr = m_textLayout->GetMetrics(&m_textMetrics);
    if (FAILED(hr)) {
        return hr;
    }

    return S_OK;
}

STDMETHODIMP SampleFpsTextRenderer::Render(ID3D11Device *pDevice)
{
    auto hr = S_OK;

    if (!m_textLayout) {
        return E_FAIL;
    }

    if (!m_d2dContext) {
        hr=CreateDeviceDependentResources(pDevice);
        if (FAILED(hr)) {
            return hr;
        }
    }

    if (!m_d2dTargetBitmap)
    {
        Microsoft::WRL::ComPtr<ID3D11DeviceContext> pContext;
        pDevice->GetImmediateContext(&pContext);

        ID3D11RenderTargetView* views[] = { nullptr };
        pContext->OMGetRenderTargets(1, views, nullptr);
        if (!views[0]) {
            return E_FAIL;
        }

        Microsoft::WRL::ComPtr<ID3D11Resource> pResource;
        views[0]->GetResource(&pResource);

        Microsoft::WRL::ComPtr<ID3D11Texture2D> pTarget;
        hr=pResource.As(&pTarget);
        if (FAILED(hr)) {
            return hr;
        }

        hr=SetupTargetBitmap(pTarget.Get());
        if (FAILED(hr)) {
            return hr;
        }
    }

    m_d2dContext->SaveDrawingState(m_stateBlock.Get());
    m_d2dContext->BeginDraw();

    // 右下隅に配置
    D2D1::Matrix3x2F screenTranslation = D2D1::Matrix3x2F::Translation(
        m_width - m_textMetrics.layoutWidth,
        m_height - m_textMetrics.height
    );

    m_d2dContext->SetTransform(screenTranslation /* * m_deviceResources->GetOrientationTransform2D()*/);

    hr = m_textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
    if (FAILED(hr)) {
        return hr;
    }

    m_d2dContext->SetTarget(m_d2dTargetBitmap.Get());
    m_d2dContext->DrawTextLayout(
        D2D1::Point2F(0.f, 0.f),
        m_textLayout.Get(),
        m_whiteBrush.Get()
    );

    //D2DERR_RECREATE_TARGET をここで無視します。このエラーは、デバイスが失われたことを示します。
    // これは、Present に対する次回の呼び出し中に処理されます。
    hr = m_d2dContext->EndDraw();

    m_d2dContext->SetTarget(nullptr);
    m_d2dContext->RestoreDrawingState(m_stateBlock.Get());

    if (hr != D2DERR_RECREATE_TARGET)
    {
        if (FAILED(hr)) {
            return hr;
        }
    }
    return S_OK;
}

HRESULT SampleFpsTextRenderer::SetupTargetBitmap(ID3D11Texture2D *pTarget)
{
    auto hr = S_OK;

    // スワップ チェーン バック バッファーに関連付けられた Direct2D ターゲット ビットマップを作成し、
    // それを現在のターゲットとして設定します。
    D2D1_BITMAP_PROPERTIES1 bitmapProperties =
        D2D1::BitmapProperties1(
            D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
            D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED),
            m_dpi,
            m_dpi
        );

    Microsoft::WRL::ComPtr<IDXGISurface> dxgiBackBuffer;
    hr = pTarget->QueryInterface(IID_PPV_ARGS(&dxgiBackBuffer));
    if (FAILED(hr)) {
        return hr;
    }

    DX::ThrowIfFailed(
        m_d2dContext->CreateBitmapFromDxgiSurface(
            dxgiBackBuffer.Get(),
            &bitmapProperties,
            &m_d2dTargetBitmap
        )
    );

    //m_d2dContext->SetDpi(m_effectiveDpi, m_effectiveDpi);

    // すべての Windows ストア アプリで、グレースケール テキストのアンチエイリアシングをお勧めします。
    m_d2dContext->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE);
    //

    return S_OK;
}

STDMETHODIMP SampleFpsTextRenderer::OnMouseLeftDown()
{
    return S_OK;
}

STDMETHODIMP SampleFpsTextRenderer::OnMouseLeftUp()
{
    return S_OK;
}

STDMETHODIMP SampleFpsTextRenderer::OnMouseMiddleDown()
{
    return S_OK;
}

STDMETHODIMP SampleFpsTextRenderer::OnMouseMiddleUp()
{
    return S_OK;
}

STDMETHODIMP SampleFpsTextRenderer::OnMouseRightDown()
{
    return S_OK;
}

STDMETHODIMP SampleFpsTextRenderer::OnMouseRightUp()
{
    return S_OK;
}

STDMETHODIMP SampleFpsTextRenderer::OnMouseMove(int width, int height)
{
    return S_OK;
}

STDMETHODIMP SampleFpsTextRenderer::OnMouseWheel(int delta)
{
    return S_OK;
}

