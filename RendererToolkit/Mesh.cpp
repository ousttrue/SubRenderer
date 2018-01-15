#include "Mesh.h"
#include <assert.h>


CMesh::~CMesh()
{
    for (auto pVertexBuffer : m_pVertexBuffers)
    {
        pVertexBuffer->Release();
    }
}

STDMETHODIMP CMesh::RuntimeClassInitialize()
{
    return S_OK;
}

STDMETHODIMP CMesh::AddVertexBuffer(ID3D11Device *pDevice, UINT stride, UINT offset
    , const void *p, UINT size)
{
    D3D11_BUFFER_DESC desc;
    ZeroMemory(&desc, sizeof(desc));
    desc.ByteWidth = size;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    desc.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA data;
    ZeroMemory(&data, sizeof(data));
    data.pSysMem = p;

    ID3D11Buffer *pVertexBuffer=nullptr;
    HRESULT hr = pDevice->CreateBuffer(&desc, &data, &pVertexBuffer);
    if (FAILED(hr)) {
        return hr;
    }

    m_pVertexBuffers.push_back(pVertexBuffer);
    m_strides.push_back(stride);
    m_offsets.push_back(offset);

    return S_OK;
}

STDMETHODIMP CMesh::CreateIndices(ID3D11Device *pDevice, UINT stride, const void *p, UINT size)
{
    D3D11_BUFFER_DESC desc;
    ZeroMemory(&desc, sizeof(desc));
    desc.ByteWidth = size;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    desc.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA data;
    ZeroMemory(&data, sizeof(data));
    data.pSysMem = p;

    HRESULT hr = pDevice->CreateBuffer(&desc, &data, &m_pIndexBuffer);
    if (FAILED(hr)) {
        return hr;
    }

    assert(size%stride == 0);
    m_indexCount = size / stride;
    switch (stride)
    {
    case 2:
        m_indexFormat=DXGI_FORMAT_R16_UINT;
        break;

    case 4:
        m_indexFormat= DXGI_FORMAT_R32_UINT;
        break;

    default:
        assert(false);
        break;
    }
    return S_OK;
}

STDMETHODIMP CMesh::Draw(ID3D11DeviceContext *pContext)
{
    if (!m_pVertexBuffers.empty()) {
        pContext->IASetVertexBuffers(0, (UINT)m_pVertexBuffers.size()
            , m_pVertexBuffers.data()
            , m_strides.data()
            , m_offsets.data()
        );
    }
    pContext->IASetIndexBuffer(m_pIndexBuffer.Get(), m_indexFormat, 0);
    pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    pContext->DrawIndexed(m_indexCount, 0, 0);

    return S_OK;
}
