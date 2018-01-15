#pragma once
#include "IRendererWindow.h"
#include <wrl/implements.h>
#include <list>


#ifndef UWP
class CHWndWindow :
    public Microsoft::WRL::RuntimeClass<
    Microsoft::WRL::RuntimeClassFlags<Microsoft::WRL::ClassicCom>, IRendererWindow>
{
    HWND m_hWnd = NULL;
    std::list<Microsoft::WRL::ComPtr<IRendererWindowCallback>> m_windowCallbacks;
    std::list<Microsoft::WRL::ComPtr<IRendererWindowMouseCallback>> m_mouseCallbacks;

public:
    STDMETHODIMP RuntimeClassInitialize(HINSTANCE hInstance
        , const WCHAR *windowClass
        , const WCHAR *windowTitle
        , const WCHAR *pMenu
    );

    // IRenderWindow
    STDMETHODIMP_(HWND) GetHWnd()override{ return m_hWnd; }
    STDMETHODIMP Show(int nCmdShow)override;
    STDMETHODIMP Update()override;
    STDMETHODIMP_(int) DefaultMessageLoop()override;
    STDMETHODIMP PeekAndDispatchMessages(int *pExitCode)override;

    STDMETHODIMP_(void) AddCallback(IRendererWindowCallback *pCallback)override;
    STDMETHODIMP_(void) RemoveCallback(IRendererWindowCallback *pCallback)override;
    STDMETHODIMP_(void) ClearCallback()override;

private:
    static LRESULT CALLBACK DelegateProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};
#endif
