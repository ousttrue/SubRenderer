#include "RendererSamples.h"
#include "BasicRenderer.h"
#include "Sample3DSceneRenderer.h"
#include "SampleFpsTextRenderer.h"
#include "ImGuiRenderer.h"


EXPORT_RENDERERSAMPLES HRESULT RENDERERSAMPLES_CreateBasicRenderer(BOOL useTexture, IRenderer **ppRenderer)
{
    auto hr = Microsoft::WRL::MakeAndInitialize<CBasicRenderer>(ppRenderer, useTexture
        );
    if (FAILED(hr)) {
        return hr;
    }

    return S_OK;
}

EXPORT_RENDERERSAMPLES HRESULT RENDERERSAMPLES_Create3DsceneRenderer(IRenderer **ppRenderer)
{
    auto hr = Microsoft::WRL::MakeAndInitialize<d3d11::Sample3DSceneRenderer>(ppRenderer
        );
    if (FAILED(hr)) {
        return hr;
    }

    return S_OK;
}

EXPORT_RENDERERSAMPLES HRESULT RENDERERSAMPLES_CreateFpsTextRenderer(IRenderer **ppRenderer)
{
    auto hr = Microsoft::WRL::MakeAndInitialize<d3d11::SampleFpsTextRenderer>(ppRenderer
        );
    if (FAILED(hr)) {
        return hr;
    }

    return S_OK;
}

EXPORT_RENDERERSAMPLES HRESULT RENDERERSAMPLES_CreateImGuiRenderer(IRenderer **ppRenderer)
{
    auto hr = Microsoft::WRL::MakeAndInitialize<CImGuiRenderer>(ppRenderer
        );
    if (FAILED(hr)) {
        return hr;
    }

    return S_OK;
}

