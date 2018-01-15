#pragma once


#ifdef BUILD_RENDERERSAMPLES
#define EXPORT_RENDERERSAMPLES extern "C" __declspec(dllexport)
#else
#define EXPORT_RENDERERSAMPLES extern "C" __declspec(dllimport)
#endif


#include <RendererToolkit.h>
#include "IRenderer.h"

EXPORT_RENDERERSAMPLES HRESULT RENDERERSAMPLES_CreateBasicRenderer(BOOL useTexture, IRenderer **ppRenderer);

EXPORT_RENDERERSAMPLES HRESULT RENDERERSAMPLES_Create3DsceneRenderer(IRenderer **ppRenderer);
EXPORT_RENDERERSAMPLES HRESULT RENDERERSAMPLES_CreateFpsTextRenderer(IRenderer **ppRenderer);

EXPORT_RENDERERSAMPLES HRESULT RENDERERSAMPLES_CreateImGuiRenderer(IRenderer **ppRenderer);

