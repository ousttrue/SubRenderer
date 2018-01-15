#include "ShaderPipeline.h"
#include "Shader.h"
#include "IMesh.h"


STDMETHODIMP CShaderPipeline::RuntimeClassInitialize(
    const WCHAR *vs, const WCHAR *vsEntry, const WCHAR *vsShaderModel
    , const WCHAR *ps, const WCHAR *psEntry, const WCHAR *psShaderModel)
{
    m_vs.reset(new VertexShader(vs, vsEntry, vsShaderModel));
    auto hr = m_vs->Load();
    if(FAILED(hr)){
        return hr;
    }

    m_ps.reset(new PixelShader(ps, psEntry, psShaderModel));
    hr = m_ps->Load();
    if (FAILED(hr)) {
        return hr;
    }

    return S_OK;
}

STDMETHODIMP CShaderPipeline::GetInputLayout(D3D11_INPUT_ELEMENT_DESC **pElements
    , UINT *pCount)
{
    if (!pElements || !pCount)
    {
        return E_POINTER;
    }
    if (!m_vs) {
        return E_FAIL;
    }
    if (m_vs->InputElements().empty()) {
        return E_FAIL;
    }

    *pElements = m_vs->InputElements().data();
    *pCount = (UINT)m_vs->InputElements().size();

    return S_OK;
}

STDMETHODIMP CShaderPipeline::Draw(ID3D11Device *pDevice, IMesh *pMesh)
{
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> pContext;
    pDevice->GetImmediateContext(&pContext);

    // VS
    pContext->VSSetShader(m_vs->Get(pDevice), NULL, 0);

    // PS
    pContext->PSSetShader(m_ps->Get(pDevice), NULL, 0);

    // IA InputLayout
    pContext->IASetInputLayout(m_vs->GetIA(pDevice));

    /*
    // 定数バッファ
    m_constant->UpdateCB(pContext);
    m_constant->SetCB(pContext);

    // Texture
    {
    auto v = m_constant->GetSRV("diffuseTexture");
    m_constant->SetSRV(pContext, v, m_texture->GetSRV());
    }
    {
    auto v = m_constant->GetSampler("diffuseTextureSampler");
    m_constant->SetSampler(pContext, v, m_texture->GetSampler());
    }
    */

    auto hr=pMesh->Draw(pContext.Get());

    pContext->OMSetRenderTargets(0, nullptr, nullptr);

    return S_OK;
}

HRESULT CShaderPipeline::CreateResources(ID3D11Device *pDevice)
{
    auto hr=m_vs->Initialize(pDevice);    
    if (FAILED(hr)) {
        return hr;
    }

    hr = m_ps->Initialize(pDevice);
    if (FAILED(hr)) {
        return hr;
    }

    return S_OK;
}
