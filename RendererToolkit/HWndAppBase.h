#pragma once
#include "IRendererWindow.h"
#include "StepTimer.h"
#include <wrl/implements.h>


class HWndAppBase
{
    DX::StepTimer m_timer;
    Microsoft::WRL::ComPtr<IRendererWindow> m_pWindow;

public:

    int Main(HINSTANCE hInstance, int nCmdShow)
    {
        auto hr = RENDERERTK_CreateWindow(hInstance
                , L"SAMPLE_FOR_HWND"
                , L"Sample for HWND"
                , nullptr
                , &m_pWindow);
        if (FAILED(hr)) {
            return 2;
        }

        auto hWnd = m_pWindow->GetHWnd();
        if (!hWnd) {
            return 3;
        }
        m_pWindow->Show(nCmdShow);
        m_pWindow->Update();

        hr=SetupLoop();
        if(FAILED(hr)){
            return 4;
        }

        MSG msg;
        auto start = timeGetTime();
        while (true) {

            while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
                if (msg.message == WM_QUIT) {
                    return (int)msg.wParam;
                }
                else {
                    TranslateMessage(&msg);
                    DispatchMessage(&msg);
                }
            }

            // シーン オブジェクトを更新します。
            m_timer.Tick([&]()
            {
                UpdateScene(m_timer.GetTotalSeconds());
            });

            hr = RenderFrame(hWnd);
            if (FAILED(hr)) {
                return 5;
            }
        }

        // not reach here
        return -1;
    }

    void AddCallback(IRendererWindowCallback *pCallback)
    {
        m_pWindow->AddCallback(pCallback);
    }

protected:

    virtual HRESULT STDMETHODCALLTYPE SetupLoop() = 0;
    virtual HRESULT STDMETHODCALLTYPE UpdateScene(double totalSeconds) = 0;
    virtual HRESULT STDMETHODCALLTYPE RenderFrame(void *pWindow) = 0;

};


template<class App>
int AppRun(App *app, HINSTANCE hInstance, int nCmdShow)
{
    return app->Main(hInstance, nCmdShow);
}
