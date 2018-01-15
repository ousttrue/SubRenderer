#include "IRenderer.h"
#include <wrl/implements.h>
#include <wrl/client.h>
#include <d3d11.h>


class CBasicRenderer :
    public Microsoft::WRL::RuntimeClass<
    Microsoft::WRL::RuntimeClassFlags<Microsoft::WRL::ClassicCom>, IRenderer, IRendererWindowCallback, IRendererWindowMouseCallback>
{
    Microsoft::WRL::ComPtr<struct IShaderPipeline> m_pShaderPipeline;
    Microsoft::WRL::ComPtr<struct IMesh> m_pMesh;

public:
    STDMETHODIMP RuntimeClassInitialize(BOOL useTexture);

    STDMETHODIMP CreateDeviceDependentResources(ID3D11Device *pDevice)override;
    STDMETHODIMP SetTexture(ID3D11Texture2D *pTexture)override;
    STDMETHODIMP SetSize(int width, int height)override;
    STDMETHODIMP Update(double totalSeconds)override;
    STDMETHODIMP Render(ID3D11Device *pDevice)override;

    STDMETHODIMP OnWindowSizeChanged(int width, int height)override
    {
        return SetSize(width, height);
    }
    STDMETHODIMP OnMouseLeftDown()override;
    STDMETHODIMP OnMouseLeftUp()override;
    STDMETHODIMP OnMouseMiddleDown()override;
    STDMETHODIMP OnMouseMiddleUp()override;
    STDMETHODIMP OnMouseRightDown()override;
    STDMETHODIMP OnMouseRightUp()override;
    STDMETHODIMP OnMouseMove(int width, int height)override;
    STDMETHODIMP OnMouseWheel(int delta)override;
};

