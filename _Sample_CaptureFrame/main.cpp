#include "pch.h"
#include <AppBase.h>
#include <RendererToolkit.h>
#include <RendererSamples.h>

//
// require InternetClient capability in manifest !
//


//const auto HLS_URL = L"http://amssamples.streaming.mediaservices.windows.net/49b57c87-f5f3-48b3-ba22-c55cfdffa9cb/Sintel.ism/manifest(format=m3u8-aapl)";
//const auto HLS_URL = L"http://sample.vodobox.com/planete_interdite/planete_interdite_alternate.m3u8";
//const auto HLS_URL = L"https://d2zihajmogu5jn.cloudfront.net/bipbop-advanced/bipbop_16x9_variant.m3u8";
const auto HLS_URL = L"http://docs.evostream.com/sample_content/assets/hls-bunny-166/playlist.m3u8";


class App : public AppBase
{
    Microsoft::WRL::ComPtr<IDeviceManager> m_pDeviceManager;
    Microsoft::WRL::ComPtr<IRenderTargetManager> m_renderTarget;
    Microsoft::WRL::ComPtr<IRenderer> m_pRenderer;

    //winrt::Windows::Media::Capture::MediaCapture m_mediaCapture;
    Windows::Media::Playback::MediaPlayer ^m_mediaPlayer;

public:
    HRESULT SetupLoop()override
    {
        //auto task = StartCaptureAsync();
        auto manifestUri = ref new Windows::Foundation::Uri(ref new Platform::String(HLS_URL));
        m_mediaPlayer = ref new Windows::Media::Playback::MediaPlayer();
        m_mediaPlayer->Source=Windows::Media::Core::MediaSource::CreateFromUri(manifestUri);
        m_mediaPlayer->Play();

        auto hr = RENDERERTK_CreateD3D11Device(&m_pDeviceManager);
        if (FAILED(hr)) {
            return hr;
        }

        hr=RENDERERTK_CreateSwapchainManager(&m_renderTarget);
        if (FAILED(hr)) {
            return hr;
        }

        hr = RENDERERSAMPLES_CreateBasicRenderer(TRUE, &m_pRenderer);
        if (FAILED(hr)) {
            return hr;
        }

        return S_OK;
    }

    HRESULT UpdateScene(double totalSeconds)override
    {
        return S_OK;
    }

    HRESULT RenderFrame(void *pWindow)override
    {
        Microsoft::WRL::ComPtr<ID3D11Device> pDevice;
        m_pDeviceManager->GetDevice(&pDevice);

        float clearColor[] = { 0.2f, 0.2f, 0.4f, 1.0f };
        auto hr = m_renderTarget->SetTargetAndDepthStencil(pDevice.Get(), pWindow
            , clearColor, TRUE, 1.0f);
        if (FAILED(hr)) {
            return hr;
        }

        Microsoft::WRL::ComPtr<ID3D11Texture2D> frameTexture;
        hr = GetFrameTexture(pDevice.Get(), &frameTexture);
        if (SUCCEEDED(hr)) {

            // render frameTexture to backBuffer
            hr = m_pRenderer->SetTexture(frameTexture.Get());
            if (FAILED(hr)) {
                return hr;
            }
            hr = m_pRenderer->Render(pDevice.Get());
            if (FAILED(hr)) {
                return hr;
            }

            hr = m_renderTarget->PresentSync();
            if (FAILED(hr)) {
                return hr;
            }
        }

        return S_OK;
    }

private:
    HRESULT GetFrameTexture(ID3D11Device *pDevice, ID3D11Texture2D **ppTexture)
    {
        auto session = m_mediaPlayer->PlaybackSession;

        auto state = session->PlaybackState;
        if (state != Windows::Media::Playback::MediaPlaybackState::Playing) {
            return E_FAIL;
        }
        if (session->NaturalVideoWidth == 0 || session->NaturalVideoHeight == 0) {
            return E_FAIL;
        }

        D3D11_TEXTURE2D_DESC desc = { 0 };
        desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        desc.Width = session->NaturalVideoWidth;
        desc.Height = session->NaturalVideoHeight;
        desc.CPUAccessFlags = 0;
        desc.MiscFlags = 0;
        desc.SampleDesc.Count = 1;
        desc.SampleDesc.Quality = 0;
        desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
        desc.ArraySize = 1;
        desc.MipLevels = 1;
        desc.Usage = D3D11_USAGE_DEFAULT;

        auto hr = pDevice->CreateTexture2D(&desc, nullptr, ppTexture);
        if (FAILED(hr)) {
            return hr;
        }

        Microsoft::WRL::ComPtr<IDXGISurface> pSurface;
        hr = (*ppTexture)->QueryInterface(IID_PPV_ARGS(&pSurface));
        if (FAILED(hr)) {
            return hr;
        }

        auto d3dSurface = Windows::Graphics::DirectX::Direct3D11::CreateDirect3DSurface(pSurface.Get());
        if (!d3dSurface) {
            return E_FAIL;
        }

        // render
        m_mediaPlayer->CopyFrameToVideoSurface(d3dSurface);

        return S_OK;
    }

};


[Platform::MTAThread]
int __stdcall wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR, int nCmdShow)
{
    App app;
    AppRun(&app,hInstance, nCmdShow);
}
