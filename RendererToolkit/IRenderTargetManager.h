#pragma once
#include "IRendererWindow.h"
#include <d3d11.h>

#include <Unknwn.h>
MIDL_INTERFACE("4CDBCED0-860D-4882-BAB8-87075DE1D146")
IRenderTargetManager : public IUnknown
{
public:
    virtual HRESULT STDMETHODCALLTYPE SetSize(int width, int height) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetBuffer(ID3D11Device *pDevice, void *pWindow
        , ID3D11Texture2D **ppRenderTarget)=0;
    virtual HRESULT STDMETHODCALLTYPE SetTargetAndDepthStencil(ID3D11Device *pDevice, void *pWindow
        , const float clearColor[4]
        , BOOL UseDepthStencil, float clearDepth) = 0;
    virtual HRESULT STDMETHODCALLTYPE Present()=0;
    virtual HRESULT STDMETHODCALLTYPE PresentSync() = 0;
};
