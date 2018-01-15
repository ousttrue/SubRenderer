#include <wrl/implements.h>
#include <wrl/client.h>
#include <RendererToolkit.h>
#include "StepTimer.h"
#include "DirectXHelper.h"
#include <list>

class CoreWindowAppBase
{
    DX::StepTimer m_timer;

    std::list <Microsoft::WRL::ComPtr<IRendererWindowCallback>> m_windowCallbacks;
    std::list <Microsoft::WRL::ComPtr<IRendererWindowMouseCallback>> m_mouseCallbacks;

    bool m_windowClosed = false;
    bool m_windowVisible = true;

protected:

    void AddCallback(IRendererWindowCallback *pCallback)
    {
        if (!pCallback) {
            return;
        }

        m_windowCallbacks.push_back(pCallback);

        Microsoft::WRL::ComPtr<IRendererWindowMouseCallback> pMouse;
        if (SUCCEEDED(pCallback->QueryInterface(IID_PPV_ARGS(&pMouse)))) {
            m_mouseCallbacks.push_back(pMouse.Get());
        }
    }

    virtual HRESULT STDMETHODCALLTYPE SetupLoop() = 0;
    virtual HRESULT STDMETHODCALLTYPE UpdateScene(double totalSeconds) = 0;
    virtual HRESULT STDMETHODCALLTYPE RenderFrame(void *pWindow) = 0;

public:
    void SetVisible(bool visible)
    {
        m_windowVisible = visible;
    }

    void Close()
    {
        m_windowClosed = true;
    }

    void Main(Windows::UI::Core::CoreWindow ^window)
    {
        DX::ThrowIfFailed(
            SetupLoop()
        );

        OnWindowSizeChanged((int)window->Bounds.Width, (int)window->Bounds.Height);

        auto pWindow = reinterpret_cast<IInspectable*>(window);

        // Enter the render loop.  Note that Windows Store apps should never exit.
        while (!m_windowClosed)
        {
            if (!m_windowVisible) {
                Windows::UI::Core::CoreWindow::GetForCurrentThread()->Dispatcher->ProcessEvents(
                    Windows::UI::Core::CoreProcessEventsOption::ProcessOneAndAllPending);
                continue;
            }

            // Process events incoming to the window.
            /*
            m_window.Dispatcher().ProcessEvents(
            winrt::Windows::UI::Core::CoreProcessEventsOption::ProcessAllIfPresent);
            */
            Windows::UI::Core::CoreWindow::GetForCurrentThread()->Dispatcher->ProcessEvents(
                Windows::UI::Core::CoreProcessEventsOption::ProcessAllIfPresent);

            // �V�[�� �I�u�W�F�N�g���X�V���܂��B
            m_timer.Tick([&]()
            {
                UpdateScene(m_timer.GetTotalSeconds());
            });

            DX::ThrowIfFailed(
                RenderFrame(pWindow)
            );
        }
    }

    void Clear()
    {
        m_windowCallbacks.clear();
        m_mouseCallbacks.clear();
    }

    void OnWindowSizeChanged(int width, int height)
    {
        for (auto &it : m_windowCallbacks)
        {
            it->OnWindowSizeChanged(width, height);
        }
    }

    void MouseLeftDown()
    {
        for (auto &it : m_mouseCallbacks) { it->OnMouseLeftDown(); }
    }

    void MouseLeftUp()
    {
        for (auto &it : m_mouseCallbacks) { it->OnMouseLeftUp(); }
    }

    void MouseMove(int x, int y)
    {
        for (auto &it : m_mouseCallbacks)
        {
            it->OnMouseMove(x, y);
        }
    }

    void MouseWheel(int delta)
    {
        for (auto &it : m_mouseCallbacks)
        {
            it->OnMouseWheel(delta);
        }
    }
};

ref class CoreWindowApp sealed :
    public Windows::ApplicationModel::Core::IFrameworkView
{
    //Windows::UI::Core::CoreWindow ^m_window = nullptr;

    CoreWindowAppBase *m_app;

internal:
    CoreWindowApp(CoreWindowAppBase *app)
        : m_app(app)
    {}

public:

    virtual void Initialize(Windows::ApplicationModel::Core::CoreApplicationView ^applicationView)
    {
        applicationView->Activated
            += ref new Windows::Foundation::TypedEventHandler<Windows::ApplicationModel::Core::CoreApplicationView^, Windows::ApplicationModel::Activation::IActivatedEventArgs^>(
                this, &CoreWindowApp::OnActivated);

        Windows::ApplicationModel::Core::CoreApplication::Suspending +=
            ref new Windows::Foundation::EventHandler<Windows::ApplicationModel::SuspendingEventArgs ^>(
                this, &CoreWindowApp::OnSuspending);

        Windows::ApplicationModel::Core::CoreApplication::Resuming +=
            ref new Windows::Foundation::EventHandler<Platform::Object ^>(
                this, &CoreWindowApp::OnResuming);

    }

    virtual void SetWindow(Windows::UI::Core::CoreWindow ^window)
    {
        //window = window;

        // Specify the cursor type as the standard arrow cursor.
        window->PointerCursor = ref new Windows::UI::Core::CoreCursor(
            Windows::UI::Core::CoreCursorType::Arrow, 0);

        // https://docs.microsoft.com/ja-jp/windows/uwp/input-and-devices/handle-pointer-input

        window->PointerPressed +=
            ref new Windows::Foundation::TypedEventHandler<Windows::UI::Core::CoreWindow ^, Windows::UI::Core::PointerEventArgs ^>(
                this, &CoreWindowApp::OnPointerMoved);

        window->PointerReleased +=
            ref new Windows::Foundation::TypedEventHandler<Windows::UI::Core::CoreWindow ^, Windows::UI::Core::PointerEventArgs ^>(
                this, &CoreWindowApp::OnPointerMoved);

        window->PointerMoved +=
            ref new Windows::Foundation::TypedEventHandler<Windows::UI::Core::CoreWindow ^, Windows::UI::Core::PointerEventArgs ^>(
                this, &CoreWindowApp::OnPointerMoved);

        window->PointerWheelChanged +=
            ref new Windows::Foundation::TypedEventHandler<Windows::UI::Core::CoreWindow ^, Windows::UI::Core::PointerEventArgs ^>(
                this, &CoreWindowApp::OnPointerWheelEvent);

        // Allow the application to respond when the window size changes.
        window->SizeChanged += 
            ref new Windows::Foundation::TypedEventHandler<Windows::UI::Core::CoreWindow ^, Windows::UI::Core::WindowSizeChangedEventArgs ^>(
                this, &CoreWindowApp::OnWindowSizeChanged);

        window->Closed += 
            ref new Windows::Foundation::TypedEventHandler<Windows::UI::Core::CoreWindow ^, Windows::UI::Core::CoreWindowEventArgs ^>(
                this, &CoreWindowApp::OnWindowClosed);

        // display
        auto currentDisplayInformation = Windows::Graphics::Display::DisplayInformation::GetForCurrentView();

        currentDisplayInformation->DpiChanged += 
            ref new Windows::Foundation::TypedEventHandler<Windows::Graphics::Display::DisplayInformation ^, Platform::Object ^>(
                this, &CoreWindowApp::OnDpiChanged);

        currentDisplayInformation->OrientationChanged += 
            ref new Windows::Foundation::TypedEventHandler<Windows::Graphics::Display::DisplayInformation ^, Platform::Object ^>(
                this, &CoreWindowApp::OnOrientationChanged);

        Windows::Graphics::Display::DisplayInformation::DisplayContentsInvalidated += 
            ref new Windows::Foundation::TypedEventHandler<Windows::Graphics::Display::DisplayInformation ^, Platform::Object ^>(
                this, &CoreWindowApp::OnDisplayContentsInvalidated);

    }

    virtual void Load(Platform::String ^entryPoint)
    {
    }

    virtual void Run()
    {
        m_app->Main(Windows::UI::Core::CoreWindow::GetForCurrentThread());
    }

    virtual void Uninitialize()
    {
        m_app->Clear();
    }

private:

    // app callbacks
    void OnActivated(Windows::ApplicationModel::Core::CoreApplicationView ^, Windows::ApplicationModel::Activation::IActivatedEventArgs ^args)
    {
        // Activate the application window, making it visible and enabling it to receive events.
        Windows::UI::Core::CoreWindow::GetForCurrentThread()->Activate();
    }

    void OnSuspending(Platform::Object ^sender, Windows::ApplicationModel::SuspendingEventArgs ^args)
    {
        // �x����v��������ɃA�v���P�[�V�����̏�Ԃ�ۑ����܂��B�x����Ԃ�ێ����邱�Ƃ́A
        //���f����̎��s�ŃA�v���P�[�V�������r�W�[��Ԃł��邱�Ƃ������܂��B
        //�x���͐����Ȃ��ێ������킯�ł͂Ȃ����Ƃɒ��ӂ��Ă��������B�� 5 �b��ɁA
        // �A�v���P�[�V�����͋����I������܂��B
        auto deferral = args->SuspendingOperation->GetDeferral();

        std::thread([this, deferral]()
        {
            //m_deviceResources->Trim();

            // �����ɃR�[�h��}�����܂��B


            deferral->Complete();
        }).detach();
    }

    void OnResuming(Platform::Object ^sender, Platform::Object ^args)
    {
        // ���f���ɃA�����[�h���ꂽ�f�[�^�܂��͏�Ԃ𕜌����܂��B����ł́A�f�[�^�Ə�Ԃ�
        // ���f����ĊJ����Ƃ��ɕێ�����܂��B���̃C�x���g�́A�A�v�������ɏI������Ă���ꍇ��
        //�������܂���B

        // �����ɃR�[�h��}�����܂��B
    }


    bool m_mouseLeftDown = false;
    bool m_mouseRightDown = false;
    bool m_mouseMiddleDown = false;
    void OnPointerMoved(Windows::UI::Core::CoreWindow ^sender, Windows::UI::Core::PointerEventArgs ^args)
    {
        auto props = args->CurrentPoint->Properties;
        bool mouseLeftDown = props->IsLeftButtonPressed;
        if (mouseLeftDown != m_mouseLeftDown) {
            m_mouseLeftDown = mouseLeftDown;
            if (m_mouseLeftDown) {
                m_app->MouseLeftDown();
            }
            else {
                m_app->MouseLeftUp();
            }
        }

        auto pos = args->CurrentPoint->Position;
        m_app->MouseMove((int)pos.X, (int)pos.Y);
    }

    void OnPointerWheelEvent(Windows::UI::Core::CoreWindow ^sender, Windows::UI::Core::PointerEventArgs ^args)
    {
        auto props = args->CurrentPoint->Properties;

        auto delta = props->MouseWheelDelta;
        m_app->MouseWheel(delta);
    }

    //
    void OnVisibilityChanged(Windows::UI::Core::VisibilityChangedEventArgs ^args)
    {
        m_app->SetVisible(args->Visible);
    }

    // view
    void OnWindowSizeChanged(Windows::UI::Core::CoreWindow ^, Windows::UI::Core::WindowSizeChangedEventArgs ^args)
    {
        m_app->OnWindowSizeChanged((int)args->Size.Width, (int)args->Size.Height);
    }

    void OnWindowClosed(Windows::UI::Core::CoreWindow ^, Windows::UI::Core::CoreWindowEventArgs ^)
    {
        m_app->Close();
    }

    // DisplayInformation �C�x���g �n���h���[�B

    void OnDpiChanged(Windows::Graphics::Display::DisplayInformation ^sender, Platform::Object ^args)
    {
        // ����: ���𑜓x�̃f�o�C�X�p�ɃX�P�[�����O����Ă���ꍇ�́A�擾���� LogicalDpi �̒l���A�v���̗L���� DPI �ƈ�v���Ȃ��ꍇ������܂��B
        // DPI �� DeviceResources ��ɐݒ肳�ꂽ�ꍇ�A
        // ��� GetDpi ���\�b�h���g�p���Ă�����擾����K�v������܂��B
        // �ڍׂɂ��ẮADeviceResources.cpp ���Q�Ƃ��Ă��������B

        //m_deviceResources->SetDpi(sender.LogicalDpi());
        //m_main->CreateWindowSizeDependentResources();
    }

    void OnOrientationChanged(Windows::Graphics::Display::DisplayInformation ^sender, Platform::Object ^args)
    {
        //m_deviceResources->SetCurrentOrientation(sender.CurrentOrientation());
        //m_main->CreateWindowSizeDependentResources();
    }

    void OnDisplayContentsInvalidated(Windows::Graphics::Display::DisplayInformation ^sender, Platform::Object ^args)
    {
        //m_deviceResources->ValidateDevice();
    }
};


ref class CoreWindowAppSource sealed :
    public Windows::ApplicationModel::Core::IFrameworkViewSource
{

    CoreWindowAppBase *m_app;

internal:
    CoreWindowAppSource(CoreWindowAppBase *app)
        : m_app(app)
    {
    }

public:
    virtual Windows::ApplicationModel::Core::IFrameworkView^ CreateView()
    {
        return ref new CoreWindowApp(m_app);
    }
};


template<class APP>
void AppRun(APP *app, HINSTANCE, int)
{
    Windows::ApplicationModel::Core::CoreApplication::Run(ref new CoreWindowAppSource(app));
}
