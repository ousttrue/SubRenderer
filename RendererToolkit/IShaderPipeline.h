#pragma once
#include <d3d11.h>
#include <Unknwn.h>


MIDL_INTERFACE("B6EB80F9-D110-4699-ABBA-30E598EEC8BC")
IShaderPipeline : public IUnknown
{
public:
    virtual HRESULT STDMETHODCALLTYPE GetInputLayout(D3D11_INPUT_ELEMENT_DESC **pElements, UINT *pCount) = 0;
    virtual HRESULT STDMETHODCALLTYPE Draw(ID3D11Device *pDevice, struct IMesh *pMesh) = 0;
};
