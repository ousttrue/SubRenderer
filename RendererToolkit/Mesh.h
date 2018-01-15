#pragma once
#include "IMesh.h"
#include <wrl/implements.h>
#include <vector>
#include <cctype>

struct ID3D11Buffer;
class CMesh :
    public Microsoft::WRL::RuntimeClass<
    Microsoft::WRL::RuntimeClassFlags<Microsoft::WRL::ClassicCom>, IMesh>
{
    std::vector<struct ID3D11Buffer*> m_pVertexBuffers;
    std::vector<UINT> m_strides;
    std::vector<UINT> m_offsets;

    Microsoft::WRL::ComPtr<struct ID3D11Buffer> m_pIndexBuffer;
    DXGI_FORMAT m_indexFormat = DXGI_FORMAT_UNKNOWN;
    UINT m_indexCount = 0;

public:
    ~CMesh();

    STDMETHODIMP RuntimeClassInitialize();

    // IMesh
    STDMETHODIMP AddVertexBuffer(ID3D11Device *pDevice, UINT stride, UINT offset
        , const void *p, UINT size)override;
    STDMETHODIMP CreateIndices(ID3D11Device *pDevice, UINT stride, const void *p, UINT size)override;
    STDMETHODIMP Draw(ID3D11DeviceContext *pContext)override;
};
