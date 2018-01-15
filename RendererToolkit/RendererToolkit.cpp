#include "RendererToolkit.h"
#include "HWndWindow.h"
#include "D3D11Renderer.h"
#include "SwapchainManager.h"
#include "ShaderPipeline.h"
#include "Mesh.h"


EXPORT_RENDERERTK HRESULT RENDERERTK_CreateD3D11Device(IDeviceManager **ppDeviceManager)
{
    auto hr = Microsoft::WRL::MakeAndInitialize<CD3D11Renderer>(ppDeviceManager);
    if (FAILED(hr)) {
        return hr;
    }

    return S_OK;
}

EXPORT_RENDERERTK HRESULT RENDERERTK_CreateSwapchainManager(IRenderTargetManager **ppRenderTargetManager)
{
    auto hr = Microsoft::WRL::MakeAndInitialize<CSwapChainManager>(ppRenderTargetManager);
    if (FAILED(hr)) {
        return hr;
    }

    return S_OK;
}

#ifdef UWP


#else
EXPORT_RENDERERTK HRESULT RENDERERTK_CreateWindow(HINSTANCE hInstance
    , const WCHAR *windowClass
    , const WCHAR *windowTitle
    , const WCHAR *pMenu
    , IRendererWindow **ppWindow
)
{
    auto hr = Microsoft::WRL::MakeAndInitialize<CHWndWindow>(ppWindow, hInstance
        , windowClass, windowTitle, pMenu
        );
    if (FAILED(hr)) {
        return hr;
    }

    return S_OK;
}
#endif

EXPORT_RENDERERTK HRESULT RENDERERTK_CreateShaderPipeline(
        const WCHAR *vs, const WCHAR *vsEntry, const WCHAR *vsShaderModel
        , const WCHAR *ps, const WCHAR *psEntry, const WCHAR *psShaderModel
        , IShaderPipeline **ppShaderPipeline
        )
{
    auto hr=Microsoft::WRL::MakeAndInitialize<CShaderPipeline>(ppShaderPipeline
        , vs, vsEntry, vsShaderModel
        , ps, psEntry, psShaderModel
        );
    if (FAILED(hr)) {
        return hr;
    }

    return S_OK;
}

EXPORT_RENDERERTK HRESULT RENDERERTK_CreateMesh(
    IMesh **ppMesh
)
{
    auto hr = Microsoft::WRL::MakeAndInitialize<CMesh>(ppMesh
        );
    if (FAILED(hr)) {
        return hr;
    }

    return S_OK;
}
