#include "D3D11Renderer.h"

/*
// First, create the Direct3D device.

// This flag is required in order to enable compatibility with Direct2D.
UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

#if defined(_DEBUG)
// If the project is in a debug build, enable debugging via SDK Layers with this flag.
creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

// This array defines the ordering of feature levels that D3D should attempt to create.
D3D_FEATURE_LEVEL featureLevels[] =
{
D3D_FEATURE_LEVEL_11_1,
D3D_FEATURE_LEVEL_11_0,
D3D_FEATURE_LEVEL_10_1,
D3D_FEATURE_LEVEL_10_0,
D3D_FEATURE_LEVEL_9_3,
D3D_FEATURE_LEVEL_9_1
};

Microsoft::WRL::ComPtr<ID3D11Device> d3dDevice;
Microsoft::WRL::ComPtr<ID3D11DeviceContext> d3dDeviceContext;
DX::ThrowIfFailed(
D3D11CreateDevice(
nullptr,                    // specify nullptr to use the default adapter
D3D_DRIVER_TYPE_HARDWARE,
nullptr,                    // leave as nullptr if hardware is used
creationFlags,              // optionally set debug and Direct2D compatibility flags
featureLevels,
ARRAYSIZE(featureLevels),
D3D11_SDK_VERSION,          // always set this to D3D11_SDK_VERSION
&d3dDevice,
nullptr,
&d3dDeviceContext
)
);
*/


STDMETHODIMP CD3D11Renderer::RuntimeClassInitialize()
{
    // CreateDevice
    D3D_DRIVER_TYPE dtype = D3D_DRIVER_TYPE_HARDWARE;
    UINT flags = D3D11_CREATE_DEVICE_BGRA_SUPPORT; // for D2D
#ifdef _DEBUG
    flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D_FEATURE_LEVEL featureLevels[] = {
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
        /*
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
        D3D_FEATURE_LEVEL_9_3,
        D3D_FEATURE_LEVEL_9_2,
        D3D_FEATURE_LEVEL_9_1
        */
    };
    D3D_FEATURE_LEVEL featureLevel;
    auto hr = D3D11CreateDevice(NULL, dtype
        , NULL, flags
        , featureLevels, _countof(featureLevels)
        , D3D11_SDK_VERSION, &m_pDevice
        , &featureLevel, NULL);
    if (FAILED(hr)) {
        return hr;
    }

    return S_OK;
}

STDMETHODIMP CD3D11Renderer::GetDevice(ID3D11Device **ppDevice)
{
    if (!ppDevice) {
        return E_POINTER;
    }
    if (!m_pDevice) {
        return E_FAIL;
    }

    *ppDevice = m_pDevice.Get();
    (*ppDevice)->AddRef();
    return S_OK;
}
