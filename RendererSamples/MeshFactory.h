#pragma once
#include <d3d11.h>

struct IMesh;
HRESULT CreateQuad(ID3D11Device *pDevice, IMesh **ppMesh);
HRESULT CreateCube(ID3D11Device *pDevice, IMesh **ppMesh);
