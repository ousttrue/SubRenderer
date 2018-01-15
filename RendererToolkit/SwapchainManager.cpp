#include "SwapchainManager.h"


STDMETHODIMP CSwapChainManager::RuntimeClassInitialize()
{
    return S_OK;
}

#ifdef UWP
STDMETHODIMP CSwapChainManager::Recreate(ID3D11Device *pDevice, void* pWindow)
{
    if (m_pSwapChain)
    {
        // If the swap chain already exists, resize it.
        auto hr = m_pSwapChain->ResizeBuffers(
            2,
            0,
            0,
            DXGI_FORMAT_B8G8R8A8_UNORM,
            0
        );
        if (FAILED(hr)) {
            return hr;
        }
        return S_OK;
    }
    else
    {
        // If the swap chain does not exist, create it.
        DXGI_SWAP_CHAIN_DESC1 swapChainDesc = { 0 };

        swapChainDesc.Stereo = false;
        swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapChainDesc.Scaling = DXGI_SCALING_NONE;
        swapChainDesc.Flags = 0;

        // Use automatic sizing.
        swapChainDesc.Width = 0;
        swapChainDesc.Height = 0;

        // This is the most common swap chain format.
        swapChainDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;

        // Don't use multi-sampling.
        swapChainDesc.SampleDesc.Count = 1;
        swapChainDesc.SampleDesc.Quality = 0;

        // Use two buffers to enable flip effect.
        swapChainDesc.BufferCount = 2;

        // We recommend using this swap effect for all applications.
        swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;


        // Once the swap chain description is configured, it must be
        // created on the same adapter as the existing D3D Device.

        // First, retrieve the underlying DXGI Device from the D3D Device.
        Microsoft::WRL::ComPtr<IDXGIDevice2> dxgiDevice;
        auto hr = pDevice->QueryInterface(IID_PPV_ARGS(&dxgiDevice));
        if (FAILED(hr)) {
            return hr;
        }

        // Ensure that DXGI does not queue more than one frame at a time. This both reduces
        // latency and ensures that the application will only render after each VSync, minimizing
        // power consumption.
        hr = dxgiDevice->SetMaximumFrameLatency(1);
        if (FAILED(hr)) {
            return hr;
        }

        // Next, get the parent factory from the DXGI Device.
        Microsoft::WRL::ComPtr<IDXGIAdapter> dxgiAdapter;
        hr = dxgiDevice->GetAdapter(&dxgiAdapter);
        if (FAILED(hr)) {
            return hr;
        }

        Microsoft::WRL::ComPtr<IDXGIFactory2> dxgiFactory;
        hr = dxgiAdapter->GetParent(IID_PPV_ARGS(&dxgiFactory));
        if (FAILED(hr)) {
            return hr;
        }

        // Finally, create the swap chain.
        hr=dxgiFactory->CreateSwapChainForCoreWindow(
                pDevice,
                (IUnknown*)pWindow,
                &swapChainDesc,
                nullptr, // allow on all displays
                &m_pSwapChain
        );
        if (FAILED(hr)) {
            return hr;
        }

        return S_OK;
    }
}

#else
STDMETHODIMP CSwapChainManager::Recreate(ID3D11Device *pDevice, void* pWindow)
{
    Microsoft::WRL::ComPtr<IDXGIFactory1> pDXGIFactory1;
    auto hr = CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void **)(&pDXGIFactory1));
    if (FAILED(hr)) {
        return hr;
    }

    Microsoft::WRL::ComPtr<IDXGIFactory2> pDXGIFactory2;
    hr = pDXGIFactory1.As(&pDXGIFactory2);
    if (FAILED(hr)) {
        return hr;
    }

    DXGI_SWAP_CHAIN_DESC1 scDesc = { 0 };
    scDesc.Width = 0;
    scDesc.Height = 0;
    scDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    scDesc.Stereo = false;
    scDesc.SampleDesc.Count = 1;
    scDesc.SampleDesc.Quality = 0;
    //scDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    scDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_BACK_BUFFER ;
    scDesc.BufferCount = 2;
    scDesc.Scaling = DXGI_SCALING_STRETCH;
    //scDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    scDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
    scDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
    hr = pDXGIFactory2->CreateSwapChainForHwnd(pDevice
        , (HWND)pWindow
        , &scDesc
        , nullptr
        , nullptr
        , &m_pSwapChain
    );
    if (FAILED(hr)) {
        return hr;
    }

    return S_OK;
}
#endif

STDMETHODIMP CSwapChainManager::GetBuffer(ID3D11Device *pDevice, void* pWindow, ID3D11Texture2D **ppRenderTarget)
{
    if (!ppRenderTarget) {
        return E_POINTER;
    }

    auto hr = S_OK;

    if (m_windowSizeChanged || !m_pSwapChain) {
        hr = Recreate(pDevice, pWindow);
        if (FAILED(hr)) {
            return E_FAIL;
        }
        m_windowSizeChanged = false;
    }

    hr = m_pSwapChain->GetBuffer(0, IID_PPV_ARGS(ppRenderTarget));
    if (FAILED(hr)) {
        return hr;
    }

    return S_OK;
}

STDMETHODIMP CSwapChainManager::SetTargetAndDepthStencil(ID3D11Device *pDevice, void *pWindow
    , const float clearColor[4]
    , BOOL UseDepthStencil, float clearDepth)
{
    auto hr = S_OK;

    if (!pDevice) {
        return E_POINTER;
    }
    Microsoft::WRL::ComPtr<ID3D11Texture2D> pTarget;
    hr = GetBuffer(pDevice, pWindow, &pTarget);
    if (FAILED(hr)) {
        return hr;
    }

    Microsoft::WRL::ComPtr<ID3D11DeviceContext> pContext;
    pDevice->GetImmediateContext(&pContext);

    D3D11_TEXTURE2D_DESC targetDesc;
    pTarget->GetDesc(&targetDesc);

    // RenderTargetViewの作成
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pRenderTargetView;
    hr = pDevice->CreateRenderTargetView(pTarget.Get(), 0, &pRenderTargetView);
    if (FAILED(hr))
    {
        return hr;
    }
    pContext->ClearRenderTargetView(pRenderTargetView.Get(), clearColor);

    // Rasterizer stage
    D3D11_VIEWPORT vp;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    vp.Width = static_cast<float>(targetDesc.Width);
    vp.Height = static_cast<float>(targetDesc.Height);
    vp.MinDepth = D3D11_MIN_DEPTH;
    vp.MaxDepth = D3D11_MAX_DEPTH;
    pContext->RSSetViewports(1, &vp);

    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> pDepthStencilView;
    if (UseDepthStencil) {
        D3D11_TEXTURE2D_DESC depthDesc;
        depthDesc.Width = 0;
        depthDesc.Height = 0;
        if (m_pDepthStencil) {
            m_pDepthStencil->GetDesc(&depthDesc);
        }

        if (targetDesc.Width != depthDesc.Width
            || targetDesc.Height != depthDesc.Height)
        {
            // デプステクスチャの作成
            depthDesc.Width = targetDesc.Width;
            depthDesc.Height = targetDesc.Height;
            depthDesc.MipLevels = 1;
            depthDesc.ArraySize = 1;
            depthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
            depthDesc.SampleDesc.Count = 1;
            depthDesc.SampleDesc.Quality = 0;
            depthDesc.Usage = D3D11_USAGE_DEFAULT;
            depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
            depthDesc.CPUAccessFlags = 0;
            depthDesc.MiscFlags = 0;
            hr = pDevice->CreateTexture2D(&depthDesc, NULL, &m_pDepthStencil);
            if (FAILED(hr)) {
                hr;
            }
        }

        /*
        // DepthStencilViewの作成
        D3D11_TEXTURE2D_DESC depthDesc;
        pDepthStencil->GetDesc(&depthDesc);
        D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
        ZeroMemory(&dsvDesc, sizeof(dsvDesc));
        dsvDesc.Format = depthDesc.Format;
        dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
        dsvDesc.Texture2D.MipSlice = 0;
        */
        auto hr = pDevice->CreateDepthStencilView(m_pDepthStencil.Get(), nullptr, &pDepthStencilView);
        if (FAILED(hr)) {
            return hr;
        }
        pContext->ClearDepthStencilView(pDepthStencilView.Get()
            , D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, clearDepth, 0);

        /*
        D3D11_DEPTH_STENCIL_DESC dsDesc;

        // Depth test parameters
        dsDesc.DepthEnable = true;
        dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
        dsDesc.DepthFunc = D3D11_COMPARISON_LESS;

        // Stencil test parameters
        dsDesc.StencilEnable = true;
        dsDesc.StencilReadMask = 0xFF;
        dsDesc.StencilWriteMask = 0xFF;

        // Stencil operations if pixel is front-facing
        dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
        dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
        dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
        dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

        // Stencil operations if pixel is back-facing
        dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
        dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
        dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
        dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

        // Create depth stencil state
        Microsoft::WRL::ComPtr<ID3D11DepthStencilState> pDSState;
        pDevice->CreateDepthStencilState(&dsDesc, &pDSState);

        // Bind depth stencil state
        pContext->OMSetDepthStencilState(pDSState.Get(), 1);
        */
    }

    // OutputMarger
    pContext->OMSetRenderTargets(1, pRenderTargetView.GetAddressOf(), pDepthStencilView.Get());

    return S_OK;
}

STDMETHODIMP CSwapChainManager::Present()
{
    if(!m_pSwapChain){
        return E_FAIL;
    }

#if 0
    DXGI_PRESENT_PARAMETERS params = { 0 };

    auto hr=m_pSwapChain->Present1(DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL, 0, &params);
    if (FAILED(hr)) {
        return hr;
    }
#else
    //m_swapChain->Present(1, 0)

    auto hr = m_pSwapChain->Present(0, 0);
    if (FAILED(hr)) {
        return hr;
    }
#endif

    return S_OK;
}

STDMETHODIMP CSwapChainManager::PresentSync()
{
    if (!m_pSwapChain) {
        return E_FAIL;
    }

    auto hr = m_pSwapChain->Present(1, 0);
    if (FAILED(hr)) {
        return hr;
    }

    return S_OK;
}

STDMETHODIMP CSwapChainManager::SetSize(int width, int height)
{
    m_windowSizeChanged = true;
    return S_OK;
}

STDMETHODIMP CSwapChainManager::OnWindowSizeChanged(int width, int height)
{
    return SetSize(width, height);
}
