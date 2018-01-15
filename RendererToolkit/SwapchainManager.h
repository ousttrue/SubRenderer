#pragma once
#include "IRenderTargetManager.h"
#include <wrl/implements.h>
#include <wrl/client.h>
#include <d3d11.h>
#include <dxgi1_2.h>


class CSwapChainManager :
    public Microsoft::WRL::RuntimeClass<
    Microsoft::WRL::RuntimeClassFlags<Microsoft::WRL::ClassicCom>, IRenderTargetManager, IRendererWindowCallback>
{
    Microsoft::WRL::ComPtr<IDXGISwapChain1> m_pSwapChain;
    bool m_windowSizeChanged = false;

    Microsoft::WRL::ComPtr<ID3D11Texture2D> m_pDepthStencil;

public:
    STDMETHODIMP SetSize(int width, int height)override;
    STDMETHODIMP RuntimeClassInitialize();
    STDMETHODIMP GetBuffer(ID3D11Device *pDevice, void* pWindow, ID3D11Texture2D **ppRenderTarget)override;
    STDMETHODIMP SetTargetAndDepthStencil(ID3D11Device *pDevice, void *pWindow
        , const float clearColor[4]
        , BOOL UseDepthStencil, float clearDepth)override;
    STDMETHODIMP Present()override;
    STDMETHODIMP PresentSync()override;

    STDMETHODIMP OnWindowSizeChanged(int width, int height)override;

private:
    STDMETHODIMP Recreate(ID3D11Device *pDevice, void* pWindow);
};
