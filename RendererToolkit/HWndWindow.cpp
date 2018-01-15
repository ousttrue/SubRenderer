#include "HWndWindow.h"

#ifndef UWP
STDMETHODIMP CHWndWindow::RuntimeClassInitialize(HINSTANCE hInstance
    , const WCHAR *windowClass
    , const WCHAR *windowTitle
    , const WCHAR *pMenu
)
{
    // Register the window class.
    WNDCLASSEX wcex;
    ZeroMemory(&wcex, sizeof(WNDCLASSEX));
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = &CHWndWindow::DelegateProc;
    wcex.hInstance = hInstance;
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = pMenu;
    wcex.lpszClassName = windowClass;
    if (RegisterClassEx(&wcex) == 0)
    {
        return E_FAIL;
    }

    // Create the application window.
    m_hWnd = CreateWindow(windowClass, windowTitle
        , WS_OVERLAPPEDWINDOW
        , CW_USEDEFAULT, 0, CW_USEDEFAULT, 0
        , NULL, NULL, hInstance
        , this
    );
    if (!m_hWnd)
    {
        return E_FAIL;
    }

    return S_OK;
}

STDMETHODIMP CHWndWindow::Show(int nCmdShow)
{
    if (ShowWindow(m_hWnd, nCmdShow)) {
        return S_OK;
    }
    else {
        return E_FAIL;
    }
}

STDMETHODIMP CHWndWindow::Update()
{
    if (UpdateWindow(m_hWnd)) {
        return S_OK;
    }
    else {
        return E_FAIL;
    }
}

STDMETHODIMP_(int) CHWndWindow::DefaultMessageLoop()
{
    MSG msg;
    ZeroMemory(&msg, sizeof(msg));
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}

STDMETHODIMP CHWndWindow::PeekAndDispatchMessages(int *pExitCode)
{
    MSG msg;
    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
        if (msg.message == WM_QUIT) {
            if (pExitCode) {
                *pExitCode = (int)msg.wParam;
            }
            // exit loop
            return E_FAIL;
        }
        else {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    // continue loop
    return S_OK;
}

LRESULT CALLBACK CHWndWindow::DelegateProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (uMsg == WM_CREATE)
    {
        auto app = (CHWndWindow*)((LPCREATESTRUCT)lParam)->lpCreateParams;
        ::SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)app);
        return app->WndProc(hwnd, uMsg, wParam, lParam);
    }
    else {
        auto app = (CHWndWindow*)::GetWindowLongPtr(hwnd, GWLP_USERDATA);
        if (app) {
            return app->WndProc(hwnd, uMsg, wParam, lParam);
        }
        else {
            return ::DefWindowProc(hwnd, uMsg, wParam, lParam);
        }
    }
}

LRESULT CALLBACK CHWndWindow::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    // fallback

    switch (message)
    {
    case WM_ERASEBKGND:
        // Suppress window erasing, to reduce flickering while the video is playing.
        return 1;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    case WM_SIZE:
    {
        auto width = LOWORD(lParam);
        auto height = HIWORD(lParam);
        for (auto &it : m_windowCallbacks)
        {
            it->OnWindowSizeChanged(width, height);
        }
        return 0;
    }

    case WM_LBUTTONDOWN:
        for (auto &it : m_mouseCallbacks)
        {
            it->OnMouseLeftDown();
        }
        return 0;

    case WM_LBUTTONUP:
        for (auto &it : m_mouseCallbacks)
        {
            it->OnMouseLeftUp();
        }
        return 0;

    case WM_RBUTTONDOWN:
        for (auto &it : m_mouseCallbacks)
        {
            it->OnMouseRightDown();
        }
        return 0;

    case WM_RBUTTONUP:
        for (auto &it : m_mouseCallbacks)
        {
            it->OnMouseRightUp();
        }
        return 0;

    case WM_MBUTTONDOWN:
        for (auto &it : m_mouseCallbacks)
        {
            it->OnMouseMiddleDown();
        }
        return 0;

    case WM_MBUTTONUP:
        for (auto &it : m_mouseCallbacks)
        {
            it->OnMouseMiddleUp();
        }
        return 0;

    case WM_MOUSEWHEEL:
    {
        auto delta = GET_WHEEL_DELTA_WPARAM(wParam);
        for (auto &it : m_mouseCallbacks)
        {
            it->OnMouseWheel(delta);
        }
        return 0;
    }

    case WM_MOUSEMOVE:
    {
        auto width = LOWORD(lParam);
        auto height = HIWORD(lParam);
        for (auto &it : m_mouseCallbacks)
        {
            it->OnMouseMove(width, height);
        }
        return 0;
    }

        /*
    case WM_KEYDOWN:
        if (wParam < 256)
            io.KeysDown[wParam] = 1;
        return true;
    case WM_KEYUP:
        if (wParam < 256)
            io.KeysDown[wParam] = 0;
        return true;
    case WM_CHAR:
        // You can also use ToAscii()+GetKeyboardState() to retrieve characters.
        if (wParam > 0 && wParam < 0x10000)
            io.AddInputCharacter((unsigned short)wParam);
        return true;
        */
    }

    return DefWindowProc(hwnd, message, wParam, lParam);
}

STDMETHODIMP_(void) CHWndWindow::AddCallback(IRendererWindowCallback *pCallback)
{
    if (!pCallback) {
        return;
    }

    m_windowCallbacks.push_back(pCallback);

    Microsoft::WRL::ComPtr<IRendererWindowMouseCallback> mouseCallback;
    auto hr=pCallback->QueryInterface(IID_PPV_ARGS(&mouseCallback));
    if (SUCCEEDED(hr)) {
        m_mouseCallbacks.push_back(mouseCallback.Get());
    }

    if (m_hWnd) {
        RECT rect;
        GetClientRect(m_hWnd, &rect);
        pCallback->OnWindowSizeChanged(rect.right, rect.bottom);
    }
}

STDMETHODIMP_(void) CHWndWindow::RemoveCallback(IRendererWindowCallback *pCallback)
{
    for (auto it = m_windowCallbacks.begin(); it != m_windowCallbacks.end();)
    {
        if (it->Get() == pCallback) {
            it = m_windowCallbacks.erase(it);
        }
        else{
            ++it;
        }
    }

    Microsoft::WRL::ComPtr<IRendererWindowMouseCallback> mouseCallback;
    auto hr = pCallback->QueryInterface(IID_PPV_ARGS(&mouseCallback));
    if (SUCCEEDED(hr)) {
        for (auto it = m_mouseCallbacks.begin(); it != m_mouseCallbacks.end();)
        {
            if (it->Get() == mouseCallback.Get()) {
                it = m_mouseCallbacks.erase(it);
            }
            else {
                ++it;
            }
        }
    }
}

STDMETHODIMP_(void) CHWndWindow::ClearCallback()
{
    m_windowCallbacks.clear();
    m_mouseCallbacks.clear();
}

#endif
