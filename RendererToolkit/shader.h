#pragma once
#include <wrl/client.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <memory>
#include <string>
#include <vector>


class ShaderBase
{
protected:
    std::wstring m_source;
    std::wstring m_entryPoint;
    std::wstring m_shaderModel;
    Microsoft::WRL::ComPtr<ID3DBlob> m_blob;

    ShaderBase(const std::wstring &source, const std::wstring &entoryPoint, const std::wstring &shaderModel);
public:
    virtual HRESULT Load()=0;
    virtual HRESULT Initialize(ID3D11Device *pDevice)=0;
};

class VertexShader: public ShaderBase
{
    Microsoft::WRL::ComPtr<ID3D11VertexShader> m_shader;
    Microsoft::WRL::ComPtr<ID3D11ShaderReflection> m_pReflector;
    std::vector<D3D11_INPUT_ELEMENT_DESC> m_inputElements;
    Microsoft::WRL::ComPtr<ID3D11InputLayout> m_pInputLayout;

public:
	VertexShader(const std::wstring &source, const std::wstring &entoryPoint, const std::wstring &shaderModel);
    ~VertexShader() {}

    ID3D11VertexShader* Get(ID3D11Device *pDevice) { 
        if (!m_shader) {
            Initialize(pDevice);
        }
        return m_shader.Get(); 
    }

    ID3D11InputLayout* GetIA(ID3D11Device *pDevice) {
        if (!m_pInputLayout) {
            Initialize(pDevice);
        }
        return m_pInputLayout.Get();
    }

    std::vector<D3D11_INPUT_ELEMENT_DESC> &InputElements() { return m_inputElements; }
    const std::vector<D3D11_INPUT_ELEMENT_DESC> &InputElements()const { return m_inputElements; }

    HRESULT Load()override;
    HRESULT Initialize(ID3D11Device *pDevice)override;
};

class PixelShader : public ShaderBase
{
    Microsoft::WRL::ComPtr<ID3D11PixelShader> m_shader;

public:
    PixelShader(const std::wstring &source, const std::wstring &entoryPoint, const std::wstring &shaderModel);
    ~PixelShader() {}

    ID3D11PixelShader* Get(ID3D11Device *pDevice) { 
        if (!m_shader) {
            Initialize(pDevice);
        }
        return m_shader.Get(); 
    }

    HRESULT Load()override;
    HRESULT Initialize(ID3D11Device *pDevice)override;
};
