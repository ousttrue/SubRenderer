#include "pch.h"
#include "../CaptureFrameGrabber/CaptureFrameGrabber.hpp"
#include <RendererSamples.h>
#include <windows.media.h>

#if 0
static std::future<winrt::hstring> GetDeviceAsync()
{
    // Get available devices for capturing pictures
    auto devices = co_await winrt::Windows::Devices::Enumeration::DeviceInformation::FindAllAsync(
        winrt::Windows::Devices::Enumeration::DeviceClass::VideoCapture);

    for (auto cameraDeviceInfo : devices)
    {
        if (cameraDeviceInfo.EnclosureLocation() 
            && cameraDeviceInfo.EnclosureLocation().Panel() == winrt::Windows::Devices::Enumeration::Panel::Back)
        {
            return cameraDeviceInfo.Id();
        }
    }

    // Nothing matched, just return the first
    if (devices.Size() > 0)
    {
        return devices.GetAt(0).Id();
    }

    return winrt::hstring();
}

static void OnFrameCallback(IMFSample *pSample, void *p)
{
    DWORD cBuffers = 0;
    auto hr = pSample->GetBufferCount(&cBuffers);
    if (FAILED(hr))
    {
        return;
    }

    Microsoft::WRL::ComPtr<IMFMediaBuffer> pBuffer;
    if (1 == cBuffers)
    {
        hr = pSample->GetBufferByIndex(0, &pBuffer);
    }
    else
    {
        hr = pSample->ConvertToContiguousBuffer(&pBuffer);
    }
    if (FAILED(hr))
    {
        return;
    }

    Microsoft::WRL::ComPtr<IMFDXGIBuffer> pDXGIBuffer;
    hr = pBuffer.As(&pDXGIBuffer);
    if (FAILED(hr))
    {
        // ‚±‚±‚É—ˆ‚½
        return;
    }

    Microsoft::WRL::ComPtr<ID3D11Texture2D> pTexture2D;
    hr = pDXGIBuffer->GetResource(IID_PPV_ARGS(&pTexture2D));
    if (FAILED(hr))
    {
        return;
    }
}

    // use rgb24 layout
    create_task(frameGrabber->GetFrameAsync()).then([this, frameGrabber](const ComPtr<IMF2DBuffer2>& buffer)
    {
        // do the RGB swizzle while copying the pixels from the IMF2DBuffer2
        BYTE *pbScanline;
        LONG plPitch;
        unsigned int colBytes = width * bytesPerPixel;
        CHK(buffer->Lock2D(&pbScanline, &plPitch));

        // flip
        if (bFlipImageX)
        {
            std::lock_guard<std::mutex> lock(VideoioBridge::getInstance().inputBufferMutex);

            // ptr to input Mat data array
            auto buf = VideoioBridge::getInstance().backInputPtr;

            for (unsigned int row = 0; row < height; row++)
            {
                unsigned int i = 0;
                unsigned int j = colBytes - 1;

                while (i < colBytes)
                {
                    // reverse the scan line
                    // as a side effect this also swizzles R and B channels
                    buf[j--] = pbScanline[i++];
                    buf[j--] = pbScanline[i++];
                    buf[j--] = pbScanline[i++];
                }
                pbScanline += plPitch;
                buf += colBytes;
            }
            VideoioBridge::getInstance().bIsFrameNew = true;
        }
        else
        {
            std::lock_guard<std::mutex> lock(VideoioBridge::getInstance().inputBufferMutex);

            // ptr to input Mat data array
            auto buf = VideoioBridge::getInstance().backInputPtr;

            for (unsigned int row = 0; row < height; row++)
            {
                // used for Bgr8:
                //for (unsigned int i = 0; i < colBytes; i++ )
                //    buf[i] = pbScanline[i];

                // used for RGB24:
                for (unsigned int i = 0; i < colBytes; i += bytesPerPixel)
                {
                    // swizzle the R and B values (BGR to RGB)
                    buf[i] = pbScanline[i + 2];
                    buf[i + 1] = pbScanline[i + 1];
                    buf[i + 2] = pbScanline[i];

                    // no swizzle
                    //buf[i] = pbScanline[i];
                    //buf[i + 1] = pbScanline[i + 1];
                    //buf[i + 2] = pbScanline[i + 2];
                }

                pbScanline += plPitch;
                buf += colBytes;
            }
            VideoioBridge::getInstance().bIsFrameNew = true;
        }
        CHK(buffer->Unlock2D());

        VideoioBridge::getInstance().frameCounter++;

        if (bGrabberInited)
        {
            _GrabFrameAsync(frameGrabber);
        }
    }, task_continuation_context::use_current());
}

std::future<void> MyApp::StartCaptureAsync()
{
    auto cameraId = co_await GetDeviceAsync();
    if (cameraId.empty()) {
        return;
    }

    auto settings = winrt::Windows::Media::Capture::MediaCaptureInitializationSettings();
    settings.StreamingCaptureMode(winrt::Windows::Media::Capture::StreamingCaptureMode::Video); // Video-only capture
    settings.VideoDeviceId(cameraId);

    m_mediaCapture = winrt::Windows::Media::Capture::MediaCapture();

    // Initialize media capture and start the preview
    co_await m_mediaCapture.InitializeAsync(settings);

    auto controller = winrt::to_cx<Windows::Media::Devices::VideoDeviceController>(m_mediaCapture.VideoDeviceController());

    auto cx = controller->GetMediaStreamProperties(Windows::Media::Capture::MediaStreamType::VideoPreview);

    auto cxVideo = safe_cast<Windows::Media::MediaProperties::VideoEncodingProperties^>(cx);

    // for 24 bpp
    cxVideo->Subtype = Windows::Media::MediaProperties::MediaEncodingSubtypes::Rgb24; // bytesPerPixel = 3;

                                                                                      // XAML & WBM use BGRA8, so it would look like
                                                                                      // props->Subtype = MediaEncodingSubtypes::Bgra8;   bytesPerPixel = 4;

    cxVideo->Width = 640;
    cxVideo->Height = 480;

    Microsoft::WRL::ComPtr<IMFMediaType> videoMT;
    auto hr = MFCreateMediaTypeFromProperties((IUnknown*)cxVideo, &videoMT);
    if (FAILED(hr)) {
        return;
    }

    Microsoft::WRL::ComPtr<IMFMediaSink> pSink;
    hr = CreateCaptureFrameGrabberSink(videoMT.Get(), OnFrameCallback, nullptr, IID_PPV_ARGS(&pSink));
    if (FAILED(hr)) {
        return;
    }

    Microsoft::WRL::ComPtr<ABI::Windows::Media::IMediaExtension> pMediaEx;
    hr = pSink.As(&pMediaEx);
    if (FAILED(hr)) {
        return;
    }

    auto mediaExtension = reinterpret_cast<Windows::Media::IMediaExtension^>(pMediaEx.Get());

    auto profile = ref new  Windows::Media::MediaProperties::MediaEncodingProfile();
    profile->Video = cxVideo;

    co_await m_mediaCapture.StartPreviewToCustomSinkAsync(
        winrt::from_cx<winrt::Windows::Media::MediaProperties::MediaEncodingProfile>(profile)
        , winrt::from_cx<winrt::Windows::Media::IMediaExtension>(mediaExtension));

    int a = 0;

    // 
    // GetFrameAsync
    //
    /*
    if (_state != State::Started)
    {
    throw ref new COMException(E_UNEXPECTED, L"State");
    }
    */

    //hr=pSink->RequestVideoSample();
}
#endif
