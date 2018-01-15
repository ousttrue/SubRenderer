#include "Sample3DSceneRenderer.h"
#include "Sample3DSceneRendererShader.h"
#include "DirectXHelper.h"
#include "MeshFactory.h"
#include <RendererToolkit.h>
#include <memory>
#include <d3d11_1.h>

using namespace d3d11;
using namespace DirectX; // for DirectXMath

STDMETHODIMP Sample3DSceneRenderer::RuntimeClassInitialize()
{
    // shader
    auto hr = RENDERERTK_CreateShaderPipeline(
        VS, L"main", L"vs_5_0"
        , PS, L"main", L"ps_5_0"
        , &m_pShaderPipeline);
    if (FAILED(hr)) {
        return 5;
    }

    return S_OK;
}

STDMETHODIMP Sample3DSceneRenderer::CreateDeviceDependentResources(ID3D11Device *pDevice)
{
    auto hr = CreateCube(pDevice, &m_pMesh);
    if (FAILED(hr)) {
        return hr;
    }

    // 定数バッファ
    CD3D11_BUFFER_DESC constantBufferDesc(sizeof(ModelViewProjectionConstantBuffer) , D3D11_BIND_CONSTANT_BUFFER);
    hr = pDevice->CreateBuffer(
        &constantBufferDesc,
        nullptr,
        &m_constantBuffer
    );
    if (FAILED(hr)) {
        return hr;
    }

    return S_OK;
}

STDMETHODIMP Sample3DSceneRenderer::SetTexture(ID3D11Texture2D *pTexture)
{
    return S_OK;
}

// ウィンドウのサイズが変更されたときに、ビューのパラメーターを初期化します。
STDMETHODIMP Sample3DSceneRenderer::SetSize(int width, int height)
{
    m_width = width;
    m_height = height;

	//auto outputSize = m_deviceResources->GetOutputSize();
	float aspectRatio = (float)width / (float)height;
	float fovAngleY = 70.0f * XM_PI / 180.0f;

	// これは、アプリケーションが縦向きビューまたはスナップ ビュー内にあるときに行うことのできる
	// 変更の簡単な例です。
	if (aspectRatio < 1.0f)
	{
		fovAngleY *= 2.0f;
	}

	// OrientationTransform3D マトリックスは、シーンの方向を表示方向と
	// 正しく一致させるため、ここで事後乗算されます。
	// この事後乗算ステップは、スワップ チェーンのターゲット ビットマップに対して行われるすべての
	//描画呼び出しで実行する必要があります。他のターゲットに対する呼び出しでは、
	// 適用する必要はありません。

	// このサンプルでは、行優先のマトリックスを使用した右辺座標系を使用しています。
	XMMATRIX perspectiveMatrix = XMMatrixPerspectiveFovRH(
		fovAngleY,
		aspectRatio,
		0.01f,
		100.0f
		);

    /*
	XMFLOAT4X4 orientation = m_deviceResources->GetOrientationTransform3D();
	XMMATRIX orientationMatrix = XMLoadFloat4x4(&orientation);
    */

	XMStoreFloat4x4(
		&m_constantBufferData.projection,
		XMMatrixTranspose(perspectiveMatrix /* * orientationMatrix*/)
		);

	// 視点は (0,0.7,1.5) の位置にあり、y 軸に沿って上方向のポイント (0,-0.1,0) を見ています。
	static const XMVECTORF32 eye = { 0.0f, 0.7f, 1.5f, 0.0f };
	static const XMVECTORF32 at = { 0.0f, -0.1f, 0.0f, 0.0f };
	static const XMVECTORF32 up = { 0.0f, 1.0f, 0.0f, 0.0f };

	XMStoreFloat4x4(&m_constantBufferData.view, XMMatrixTranspose(XMMatrixLookAtRH(eye, at, up)));

    return S_OK;
}

// フレームごとに 1 回呼び出し、キューブを回転させてから、モデルおよびビューのマトリックスを計算します。
STDMETHODIMP Sample3DSceneRenderer::Update(double totalSeconds)
{
    if (!m_tracking)
    {
        // 度をラジアンに変換し、秒を回転角度に変換します
        float radiansPerSecond = XMConvertToRadians(m_degreesPerSecond);
        double totalRotation = totalSeconds * radiansPerSecond;
        float radians = static_cast<float>(fmod(totalRotation, XM_2PI));

        Rotate(radians);
    }

    return S_OK;
}

// 頂点とピクセル シェーダーを使用して、1 つのフレームを描画します。
STDMETHODIMP Sample3DSceneRenderer::Render(ID3D11Device *pDevice)
{
    if (!m_pShaderPipeline) {
        return E_FAIL;
    }

    // 読み込みは非同期です。読み込みが完了した後にのみ描画してください。
    if (!m_loadingComplete)
    {
        CreateDeviceDependentResources(pDevice);
        m_loadingComplete = true;
    }

    Microsoft::WRL::ComPtr<ID3D11DeviceContext> context0;
    pDevice->GetImmediateContext(&context0);

    Microsoft::WRL::ComPtr<ID3D11DeviceContext1> context;
    auto hr = context0.As(&context);
    if (FAILED(hr)) {
        return hr;
    }

    // 定数バッファーを準備して、グラフィックス デバイスに送信します。
    context->UpdateSubresource1(
        m_constantBuffer.Get(),
        0,
        NULL,
        &m_constantBufferData,
        0,
        0,
        0
    );

    // 定数バッファーをグラフィックス デバイスに送信します。
    context->VSSetConstantBuffers1(
        0,
        1,
        m_constantBuffer.GetAddressOf(),
        nullptr,
        nullptr
    );

    hr=m_pShaderPipeline->Draw(pDevice, m_pMesh.Get());
    if (FAILED(hr)) {
        return hr;
    }

    return S_OK;
}

//3D キューブ モデルを、ラジアン単位で設定された大きさだけ回転させます。
void Sample3DSceneRenderer::Rotate(float radians)
{
	//更新されたモデル マトリックスをシェーダーに渡す準備をします
	XMStoreFloat4x4(&m_constantBufferData.model, XMMatrixTranspose(XMMatrixRotationY(radians)));
}

void Sample3DSceneRenderer::StartTracking()
{
	m_tracking = true;
}

// 追跡時に、出力画面の幅方向を基準としてポインターの位置を追跡することにより、3D キューブを Y 軸に沿って回転させることができます。
void Sample3DSceneRenderer::TrackingUpdate(float positionX)
{
	if (m_tracking)
	{
		float radians = XM_2PI * 2.0f * positionX / (float)m_width;
		Rotate(radians);
	}
}

void Sample3DSceneRenderer::StopTracking()
{
	m_tracking = false;
}

STDMETHODIMP Sample3DSceneRenderer::OnMouseLeftDown()
{
    return S_OK;
}

STDMETHODIMP Sample3DSceneRenderer::OnMouseLeftUp()
{
    return S_OK;
}

STDMETHODIMP Sample3DSceneRenderer::OnMouseMiddleDown()
{
    return S_OK;
}

STDMETHODIMP Sample3DSceneRenderer::OnMouseMiddleUp()
{
    return S_OK;
}

STDMETHODIMP Sample3DSceneRenderer::OnMouseRightDown()
{
    return S_OK;
}

STDMETHODIMP Sample3DSceneRenderer::OnMouseRightUp()
{
    return S_OK;
}

STDMETHODIMP Sample3DSceneRenderer::OnMouseMove(int width, int height)
{
    return S_OK;
}

STDMETHODIMP Sample3DSceneRenderer::OnMouseWheel(int delta)
{
    return S_OK;
}

