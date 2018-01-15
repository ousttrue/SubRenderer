#pragma once

#include <windows.h>
#include <d3d11.h>
#include <string>


HRESULT CompileShaderFromFile(
    const std::wstring &source,
    const std::wstring &entryPoint,
    const std::wstring &shaderModel,
    ID3DBlob**  ppBlobOut
);

HRESULT CompileShaderFromMemory(
    const std::wstring &source,
    const std::wstring &entryPoint,
    const std::wstring &shaderModel,
    ID3DBlob**  ppBlobOut
);
