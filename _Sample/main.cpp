#include "pch.h"
#include <AppBase.h>
#include <RendererToolkit.h>
#include <RendererSamples.h>


class App : public AppBase
{
    Microsoft::WRL::ComPtr<struct IDeviceManager> m_deviceResources;
    Microsoft::WRL::ComPtr<struct IRenderTargetManager> m_renderTarget;

    Microsoft::WRL::ComPtr<struct IRenderer> m_main;
    //Microsoft::WRL::ComPtr<struct IRenderer> m_fps;

protected:
    HRESULT SetupLoop()override
    {
        RENDERERTK_CreateD3D11Device(&m_deviceResources);

        RENDERERTK_CreateSwapchainManager(&m_renderTarget);
        {
            Microsoft::WRL::ComPtr<IRendererWindowCallback> pCallback;
            if (SUCCEEDED(m_renderTarget.As(&pCallback))) {
                AddCallback(pCallback.Get());
            }
        }

        RENDERERSAMPLES_Create3DsceneRenderer(&m_main);
        //RENDERERSAMPLES_CreateFpsTextRenderer(&m_fps);
        //RENDERERSAMPLES_CreateImGuiRenderer(&m_main);
        {
            Microsoft::WRL::ComPtr<IRendererWindowCallback> pCallback;
            if (SUCCEEDED(m_main.As(&pCallback))) {
                AddCallback(pCallback.Get());
            }
        }

        return S_OK;
    }

    HRESULT UpdateScene(double totalSeconds)override
    {
        auto hr = S_OK;

        hr = m_main->Update(totalSeconds);
        if (FAILED(hr)) {
            return hr;
        }

        /*
        hr = m_fps->Update(totalSeconds);
        if (FAILED(hr)) {
        return hr;
        }
        */

        return S_OK;
    }

    HRESULT RenderFrame(void *pWindow)override
    {
        auto hr = S_OK;

        Microsoft::WRL::ComPtr<ID3D11Device> pDevice;
        hr = m_deviceResources->GetDevice(&pDevice);
        if (FAILED(hr)) {
            return hr;
        }

        float clearColor[] = { 0.2f, 0.2f, 0.4f, 1.0f };
        hr = m_renderTarget->SetTargetAndDepthStencil(pDevice.Get(), pWindow
            , clearColor, TRUE, 1.0f);
        if (FAILED(hr)) {
            return hr;
        }

        hr = m_main->Render(pDevice.Get());
        if (FAILED(hr)) {
            return hr;
        }

        /*
        hr = m_fps->Render(pDevice.Get());
        if (FAILED(hr)) {
        return hr;
        }
        */

        hr = m_renderTarget->PresentSync();
        if (FAILED(hr)) {
            return hr;
        }

        return S_OK;
    }
};


#ifdef UWP
[Platform::MTAThread]
#endif
int __stdcall wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR, int nCmdShow)
{
    App app;   
    AppRun(&app, hInstance, nCmdShow);
}
