#pragma once
#include "IRenderer.h"
#include "ShaderStructures.h"
#include <wrl/implements.h>


struct IShaderPipeline;
struct IMesh;
namespace d3d11
{
    // このサンプル レンダリングでは、基本的なレンダリング パイプラインをインスタンス化します。
    class Sample3DSceneRenderer :
            public Microsoft::WRL::RuntimeClass<
            Microsoft::WRL::RuntimeClassFlags<Microsoft::WRL::ClassicCom>, IRenderer, IRendererWindowCallback, IRendererWindowMouseCallback>
    {
        int m_width = 0;
        int m_height = 0;

        Microsoft::WRL::ComPtr<IShaderPipeline> m_pShaderPipeline;
        Microsoft::WRL::ComPtr<IMesh> m_pMesh;

        // キューブ ジオメトリの Direct3D リソース。
        Microsoft::WRL::ComPtr<ID3D11Buffer>		m_constantBuffer;

        // キューブ ジオメトリのシステム リソース。
        ModelViewProjectionConstantBuffer	m_constantBufferData;
        UINT	m_indexCount=0;

        // レンダリング ループで使用する変数。
        bool	m_loadingComplete=false;
        float	m_degreesPerSecond=45.0f;
        bool	m_tracking=false;

    public:
        STDMETHODIMP RuntimeClassInitialize();

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
    private:
        void StartTracking();
        void TrackingUpdate(float positionX);
        void StopTracking();
        bool IsTracking() { return m_tracking; }
        void Rotate(float radians);
    };
}
