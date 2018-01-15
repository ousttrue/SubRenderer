#include "BasicRenderer.h"
#include "BasicRendererShader.h"
#include "MeshFactory.h"
#include <RendererToolkit.h>
#include <DirectXMath.h>


STDMETHODIMP CBasicRenderer::RuntimeClassInitialize(BOOL useTexture)
{
    // shade
    auto psEntry = useTexture ? L"psMainTexture" : L"psMain";

    auto hr = RENDERERTK_CreateShaderPipeline(
        SHADER, L"vsMain", L"vs_4_0"
        , SHADER, psEntry, L"ps_4_0"
        , &m_pShaderPipeline);
    if (FAILED(hr)) {
        return 5;
    }

    return S_OK;
}

HRESULT CBasicRenderer::CreateDeviceDependentResources(ID3D11Device *pDevice)
{
    // IA
    auto hr=CreateQuad(pDevice, &m_pMesh);
    if (FAILED(hr)) {
        return hr;
    }

    return S_OK;
}

STDMETHODIMP CBasicRenderer::SetTexture(ID3D11Texture2D *pTexture)
{
    if (!pTexture) {
        return E_POINTER;
    }

    Microsoft::WRL::ComPtr<ID3D11Device> pDevice;
    pTexture->GetDevice(&pDevice);
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> pContext;
    pDevice->GetImmediateContext(&pContext);

    // SRV
    D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};
    SRVDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    SRVDesc.Texture2D.MipLevels = 1;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv;
    auto hr = pDevice->CreateShaderResourceView(pTexture, &SRVDesc, &srv);
    if (FAILED(hr))
    {
        return hr;
    }
    pContext->PSSetShaderResources(0, 1, srv.GetAddressOf());

    // SamplerState
    D3D11_SAMPLER_DESC samplerDesc;
    samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.MipLODBias = 0.0f;
    samplerDesc.MaxAnisotropy = 1;
    samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
    samplerDesc.BorderColor[0] = 0;
    samplerDesc.BorderColor[1] = 0;
    samplerDesc.BorderColor[2] = 0;
    samplerDesc.BorderColor[3] = 0;
    samplerDesc.MinLOD = 0;
    samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
    Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler;
    hr = pDevice->CreateSamplerState(&samplerDesc, &sampler);
    if (FAILED(hr))
    {
        return hr;
    }
    pContext->PSSetSamplers(0, 1, sampler.GetAddressOf());

    return S_OK;
}

STDMETHODIMP CBasicRenderer::SetSize(int width, int height)
{
    return S_OK;
}

STDMETHODIMP CBasicRenderer::Update(double totalSeconds)
{
    return S_OK;
}

HRESULT CBasicRenderer::Render(ID3D11Device *pDevice)
{
    if (!m_pMesh) {
        CreateDeviceDependentResources(pDevice);
    }

    m_pShaderPipeline->Draw(pDevice, m_pMesh.Get());

    return S_OK;
}

STDMETHODIMP CBasicRenderer::OnMouseLeftDown()
{
    return S_OK;
}

STDMETHODIMP CBasicRenderer::OnMouseLeftUp()
{
    return S_OK;
}

STDMETHODIMP CBasicRenderer::OnMouseMiddleDown()
{
    return S_OK;
}

STDMETHODIMP CBasicRenderer::OnMouseMiddleUp()
{
    return S_OK;
}

STDMETHODIMP CBasicRenderer::OnMouseRightDown()
{
    return S_OK;
}

STDMETHODIMP CBasicRenderer::OnMouseRightUp()
{
    return S_OK;
}

STDMETHODIMP CBasicRenderer::OnMouseMove(int width, int height)
{
    return S_OK;
}

STDMETHODIMP CBasicRenderer::OnMouseWheel(int delta)
{
    return S_OK;
}

