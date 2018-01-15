#pragma once
#include "IShaderPipeline.h"
#include "shader.h"
#include <wrl/implements.h>
#include <string>
#include <memory>


struct ID3D11Device;
struct ID3D11Texture2D;
struct ID3D11InputLayout;
class CShaderPipeline :
    public Microsoft::WRL::RuntimeClass<
    Microsoft::WRL::RuntimeClassFlags<Microsoft::WRL::ClassicCom>, IShaderPipeline>
{
    std::wstring m_pixelShaderSource;

    std::unique_ptr<VertexShader> m_vs;
    std::unique_ptr<PixelShader> m_ps;

public:
    STDMETHODIMP RuntimeClassInitialize(
        const WCHAR *vs, const WCHAR *vsEntry, const WCHAR *vsShaderModel
        , const WCHAR *ps, const WCHAR *psEntry, const WCHAR *psShaderModel
    );

    STDMETHODIMP GetInputLayout(D3D11_INPUT_ELEMENT_DESC **pElements, UINT *pCount)override;
    STDMETHODIMP Draw(ID3D11Device *pDevice, IMesh *pMesh)override;

private:
    HRESULT CreateResources(ID3D11Device *pDevice);
};
