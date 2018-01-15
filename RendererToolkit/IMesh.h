#pragma once
#include <d3d11.h>
#include <Unknwn.h>


MIDL_INTERFACE("89F3721D-430A-4CE4-B54B-CF8218CD500E")
IMesh : public IUnknown
{
public:
    virtual HRESULT STDMETHODCALLTYPE AddVertexBuffer(ID3D11Device *pDevice, UINT stride, UINT offset
        , const void *p, UINT size)=0;
    virtual HRESULT STDMETHODCALLTYPE CreateIndices(ID3D11Device *pDevice, UINT stride, const void *p, UINT size)=0;
    virtual HRESULT STDMETHODCALLTYPE Draw(ID3D11DeviceContext *pContext) = 0;
};
