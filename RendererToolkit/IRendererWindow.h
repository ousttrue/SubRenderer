#pragma once
#include <Unknwn.h>
#include <Windows.h>


MIDL_INTERFACE("5E662891-C7D6-4C7C-9719-9307DAC8FC53")
IRendererWindowCallback : public IUnknown
{
public:
    virtual HRESULT STDMETHODCALLTYPE OnWindowSizeChanged(int width, int height) = 0;
};


MIDL_INTERFACE("21B4CF55-9438-4D69-8A6B-B921E28E6441")
IRendererWindowMouseCallback: public IUnknown
{
public:
    virtual HRESULT STDMETHODCALLTYPE OnMouseLeftDown() = 0;
    virtual HRESULT STDMETHODCALLTYPE OnMouseLeftUp() = 0;
    virtual HRESULT STDMETHODCALLTYPE OnMouseMiddleDown() = 0;
    virtual HRESULT STDMETHODCALLTYPE OnMouseMiddleUp() = 0;
    virtual HRESULT STDMETHODCALLTYPE OnMouseRightDown() = 0;
    virtual HRESULT STDMETHODCALLTYPE OnMouseRightUp() = 0;
    virtual HRESULT STDMETHODCALLTYPE OnMouseMove(int width, int height) = 0;
    virtual HRESULT STDMETHODCALLTYPE OnMouseWheel(int delta) = 0;
};


MIDL_INTERFACE("DF2DCFF4-FD54-474B-A9E0-D70FC13C4CC4")
IRendererWindow : public IUnknown
{
public:
    virtual HWND STDMETHODCALLTYPE GetHWnd()=0;
    virtual HRESULT STDMETHODCALLTYPE Show(int nCmdShow)=0;
    virtual HRESULT STDMETHODCALLTYPE Update() = 0;
    virtual int STDMETHODCALLTYPE DefaultMessageLoop() = 0;
    virtual HRESULT PeekAndDispatchMessages(int *pExitCode) = 0;

    virtual void AddCallback(IRendererWindowCallback *pCallback) = 0;
    virtual void RemoveCallback(IRendererWindowCallback *pCallback) = 0;
    virtual void ClearCallback() = 0;
};
