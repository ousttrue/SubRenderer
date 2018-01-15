#pragma once
#include "IDeviceManager.h"
#include <wrl/implements.h>
#include <wrl/client.h>
#include <d3d11.h>


class CD3D11Renderer :
    public Microsoft::WRL::RuntimeClass<
    Microsoft::WRL::RuntimeClassFlags<Microsoft::WRL::ClassicCom>, IDeviceManager>
{
    Microsoft::WRL::ComPtr<ID3D11Device> m_pDevice;

public:
    STDMETHODIMP RuntimeClassInitialize();
    STDMETHODIMP GetDevice(ID3D11Device **ppDevice)override;
};
