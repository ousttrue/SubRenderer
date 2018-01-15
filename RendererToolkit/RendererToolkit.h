#pragma once
#ifdef BUILD_RENDERERTK
#define EXPORT_RENDERERTK extern "C" __declspec(dllexport)
#else
#define EXPORT_RENDERERTK extern "C" __declspec(dllimport)
#endif


#include "IRendererWindow.h"
#include "IDeviceManager.h"
#include "IRenderTargetManager.h"
#include "IShaderPipeline.h"
#include "IMesh.h"


EXPORT_RENDERERTK HRESULT RENDERERTK_CreateD3D11Device(IDeviceManager **ppDeviceManager);

EXPORT_RENDERERTK HRESULT RENDERERTK_CreateSwapchainManager(IRenderTargetManager **ppRenderTargetManager);

EXPORT_RENDERERTK HRESULT RENDERERTK_CreateWindow(HINSTANCE hInstance
    , const WCHAR *windowClass
    , const WCHAR *windowTitle
    , const WCHAR *pMenu
    , IRendererWindow **ppWindow
);

EXPORT_RENDERERTK HRESULT RENDERERTK_CreateShaderPipeline(
    const WCHAR *vs, const WCHAR *vsEntry, const WCHAR *vsShaderModel
    , const WCHAR *ps, const WCHAR *psEntry, const WCHAR *psShaderModel
    , IShaderPipeline **ppShaderPipeline
);

EXPORT_RENDERERTK HRESULT RENDERERTK_CreateMesh(
    IMesh **ppMesh
);

