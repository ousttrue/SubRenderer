#pragma once
#include <RendererToolkit.h>
#include <d3d11.h>
#include <Unknwn.h>

MIDL_INTERFACE("0ECC1474-E535-4686-9C12-1677B31BCC92")
IRenderer : public IUnknown
{
public:
    virtual HRESULT STDMETHODCALLTYPE CreateDeviceDependentResources(ID3D11Device *pDevice)=0;
    virtual HRESULT STDMETHODCALLTYPE SetTexture(ID3D11Texture2D *pTexture) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetSize(int width, int height) = 0;

    virtual HRESULT STDMETHODCALLTYPE Update(double totalSeconds) = 0;
    virtual HRESULT STDMETHODCALLTYPE Render(ID3D11Device *pDevice)=0;
};
