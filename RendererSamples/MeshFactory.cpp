#include "MeshFactory.h"
#include <RendererToolkit.h>
#include <DirectXMath.h>


HRESULT CreateQuad(ID3D11Device *pDevice, IMesh **ppMesh)
{
    auto hr = RENDERERTK_CreateMesh(ppMesh);
    if (FAILED(hr)) {
        return hr;
    }

    struct Vertex
    {
        DirectX::XMFLOAT4 pos;
        DirectX::XMFLOAT4 color;
        DirectX::XMFLOAT2 tex;
    };
    auto size = 1.0f;
    auto white = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
    //
    // 0 1
    // 3 2
    //
    Vertex pVertices[] =
    {
        { DirectX::XMFLOAT4(-size, size, 0, 1.0f), white, DirectX::XMFLOAT2(0, 0) },
        { DirectX::XMFLOAT4(size, size, 0, 1.0f), white, DirectX::XMFLOAT2(1, 0) },
        { DirectX::XMFLOAT4(size, -size, 0, 1.0f), white, DirectX::XMFLOAT2(1, 1) },
        { DirectX::XMFLOAT4(-size, -size, 0, 1.0f), white, DirectX::XMFLOAT2(0, 1) },
    };
    hr = (*ppMesh)->AddVertexBuffer(pDevice
        , sizeof(Vertex), 0
        , pVertices, sizeof(pVertices));
    if (FAILED(hr)) {
        return hr;
    }

    int pIndices[] =
    {
        0, 1, 2,
        2, 3, 0,
    };
    hr = (*ppMesh)->CreateIndices(pDevice, sizeof(pIndices[0]), pIndices, sizeof(pIndices));
    if (FAILED(hr)) {
        return hr;
    }

    return S_OK;
}


#include "ShaderStructures.h"
HRESULT CreateCube(ID3D11Device *pDevice, IMesh **ppMesh)
{
    using namespace DirectX;
    using namespace d3d11;

    auto hr = RENDERERTK_CreateMesh(ppMesh);
    if (FAILED(hr)) {
        return hr;
    }

    // ���b�V���̒��_��ǂݍ��݂܂��B�e���_�ɂ́A�ʒu�ƐF������܂��B
    static const VertexPositionColor cubeVertices[] =
    {
    {XMFLOAT3(-0.5f, -0.5f, -0.5f), XMFLOAT3(0.0f, 0.0f, 0.0f)},
    {XMFLOAT3(-0.5f, -0.5f,  0.5f), XMFLOAT3(0.0f, 0.0f, 1.0f)},
    {XMFLOAT3(-0.5f,  0.5f, -0.5f), XMFLOAT3(0.0f, 1.0f, 0.0f)},
    {XMFLOAT3(-0.5f,  0.5f,  0.5f), XMFLOAT3(0.0f, 1.0f, 1.0f)},
    {XMFLOAT3( 0.5f, -0.5f, -0.5f), XMFLOAT3(1.0f, 0.0f, 0.0f)},
    {XMFLOAT3( 0.5f, -0.5f,  0.5f), XMFLOAT3(1.0f, 0.0f, 1.0f)},
    {XMFLOAT3( 0.5f,  0.5f, -0.5f), XMFLOAT3(1.0f, 1.0f, 0.0f)},
    {XMFLOAT3( 0.5f,  0.5f,  0.5f), XMFLOAT3(1.0f, 1.0f, 1.0f)},
    };

    D3D11_SUBRESOURCE_DATA vertexBufferData = {0};
    vertexBufferData.pSysMem = cubeVertices;
    vertexBufferData.SysMemPitch = 0;
    vertexBufferData.SysMemSlicePitch = 0;
    CD3D11_BUFFER_DESC vertexBufferDesc(sizeof(cubeVertices), D3D11_BIND_VERTEX_BUFFER);

    hr = (*ppMesh)->AddVertexBuffer(pDevice
        , sizeof(VertexPositionColor), 0
        , cubeVertices, sizeof(cubeVertices));
    if (FAILED(hr)) {
        return hr;
    }

    // ���b�V���̃C���f�b�N�X��ǂݍ��݂܂��B�C���f�b�N�X�� 3 �� 1 �g�̒l�̂��ꂼ��́A���̂��̂�\���܂�
    // ��ʏ�ɕ`�悳���O�p�`��\���܂��B
    // ���Ƃ��΁A0,2,1 �Ƃ́A���_�o�b�t�@�[�̃C���f�b�N�X
    //0�A2�A1 �ɂ��钸�_���A���̃��b�V����
    // �ŏ��̎O�p�`���\�����邱�Ƃ��Ӗ����܂��B
    static const unsigned short cubeIndices [] =
    {
    0,2,1, // -x
    1,2,3,

    4,5,6, // +x
    5,7,6,

    0,1,5, // -y
    0,5,4,

    2,6,7, // +y
    2,7,3,

    0,4,6, // -z
    0,6,2,

    1,3,7, // +z
    1,7,5,
    };

    hr = (*ppMesh)->CreateIndices(pDevice, sizeof(cubeIndices[0]), cubeIndices, sizeof(cubeIndices));
    if (FAILED(hr)) {
        return hr;
    }

    return S_OK;
}
