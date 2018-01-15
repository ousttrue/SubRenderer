#include "shader.h"
#include <DirectXMath.h>
#include "CompileShaderFromFile.h"
#include "constantbuffer.h"


ShaderBase::ShaderBase(const std::wstring &source, const std::wstring &entryPoint, const std::wstring &shaderModel)
    : m_source(source), m_entryPoint(entryPoint), m_shaderModel(shaderModel)
{}

VertexShader::VertexShader(const std::wstring &source, const std::wstring &entryPoint, const std::wstring &shaderModel)
    : ShaderBase(source, entryPoint, shaderModel)
{}

static DXGI_FORMAT GetDxgiFormat(D3D_REGISTER_COMPONENT_TYPE type, BYTE mask)
{
    if ((mask & 0x0F)==0x0F)
    {
        // xyzw
        switch (type)
        {
        case D3D10_REGISTER_COMPONENT_FLOAT32:
            return DXGI_FORMAT_R32G32B32A32_FLOAT;
        }
    }

    if ((mask & 0x07)==0x07)
    {
        // xyz
        switch (type)
        {
        case D3D10_REGISTER_COMPONENT_FLOAT32:
            return DXGI_FORMAT_R32G32B32_FLOAT;
        }
    }

    if ((mask & 0x03)==0x03)
    {
        // xy
        switch (type)
        {
        case D3D10_REGISTER_COMPONENT_FLOAT32:
            return DXGI_FORMAT_R32G32_FLOAT;
        }
    }

    if ((mask & 0x1)==0x1)
    {
        // x
        switch (type)
        {
        case D3D10_REGISTER_COMPONENT_FLOAT32:
            return DXGI_FORMAT_R32_FLOAT;
        }
    }

    return DXGI_FORMAT_UNKNOWN;
}

HRESULT VertexShader::Load()
{
    m_inputElements.clear();

    HRESULT hr = CompileShaderFromMemory(m_source, m_entryPoint, m_shaderModel, &m_blob);
    if (FAILED(hr)) {
        return hr;
    }

    // vertex shader reflection
    hr = D3DReflect(m_blob->GetBufferPointer(), m_blob->GetBufferSize(), IID_ID3D11ShaderReflection, &m_pReflector);
    if (FAILED(hr)) {
        return hr;
    }

    D3D11_SHADER_DESC shaderdesc;
    m_pReflector->GetDesc(&shaderdesc);

    // Create InputLayout
    for (UINT i = 0; i < shaderdesc.InputParameters; ++i) {
        D3D11_SIGNATURE_PARAMETER_DESC sigdesc;
        m_pReflector->GetInputParameterDesc(i, &sigdesc);

        auto format = GetDxgiFormat(sigdesc.ComponentType, sigdesc.Mask);

        D3D11_INPUT_ELEMENT_DESC eledesc = {
            sigdesc.SemanticName
            , sigdesc.SemanticIndex
            , format
            , 0 // Œˆ‚ß‘Å‚¿
            , D3D11_APPEND_ALIGNED_ELEMENT // Œˆ‚ß‘Å‚¿
            , D3D11_INPUT_PER_VERTEX_DATA // Œˆ‚ß‘Å‚¿
            , 0 // Œˆ‚ß‘Å‚¿
        };
        m_inputElements.push_back(eledesc);
    }

    return S_OK;
}

HRESULT VertexShader::Initialize(ID3D11Device *pDevice)
{
    auto hr = pDevice->CreateVertexShader(m_blob->GetBufferPointer(), m_blob->GetBufferSize(), NULL, &m_shader);
    if (FAILED(hr)) {
        return hr;
    }

    if (m_inputElements.size() > 0) {
        hr = pDevice->CreateInputLayout(m_inputElements.data(), (UINT)m_inputElements.size(),
            m_blob->GetBufferPointer(), m_blob->GetBufferSize(), &m_pInputLayout);
        if (FAILED(hr)) {
            return hr;
        }
    }

    return S_OK;
}

PixelShader::PixelShader(const std::wstring &source, const std::wstring &entryPoint, const std::wstring &shaderModel)
    : ShaderBase(source, entryPoint, shaderModel)
{}

HRESULT PixelShader::Load()
{
    HRESULT hr = CompileShaderFromMemory(m_source, m_entryPoint, m_shaderModel, &m_blob);
    if (FAILED(hr)) {
        return hr;
    }

    return S_OK;
}

HRESULT PixelShader::Initialize(ID3D11Device *pDevice)
{
    auto hr = pDevice->CreatePixelShader(m_blob->GetBufferPointer(), m_blob->GetBufferSize(), NULL, &m_shader);
    if (FAILED(hr)) {
        return hr;
    }

    return S_OK;
}
