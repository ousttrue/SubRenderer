//------------------------------------------------------------------------------
// <copyright file="DX11Utils.cpp" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

#include "CompileShaderFromFile.h"
#include "StringUtil.h"
#include <wrl/client.h>
#include <vector>
#include <fstream>
#include <d3dcompiler.h>

#ifndef D3D_COMPILE_STANDARD_FILE_INCLUDE
#define D3D_COMPILE_STANDARD_FILE_INCLUDE ((ID3DInclude*)(UINT_PTR)1)

inline std::string to_MultiByte(UINT uCodePage, const std::wstring &text)
{
    int size = WideCharToMultiByte(uCodePage, 0, text.c_str(), -1, NULL, 0, 0, NULL);
    if (size <= 1) {
        return "";
    }
    std::vector<char> buf(size);
    size = WideCharToMultiByte(uCodePage, 0, text.c_str(), -1, &buf[0], buf.size(), 0, NULL);
    // ������\0�𗎂Ƃ�
    return std::string(buf.begin(), buf.begin() + size - 1);
}

inline HRESULT D3DCompileFromFile(
    LPCWSTR pFileName,
    const D3D_SHADER_MACRO *pDefines,
    ID3DInclude *pInclude,
    LPCSTR pEntrypoint,
    LPCSTR pTarget,
    UINT Flags1,
    UINT Flags2,
    ID3DBlob **ppCode,
    ID3DBlob **ppErrorMsgs
)
{
    std::ifstream ifs(pFileName, std::ios::binary);
    if (!ifs) {
        return E_FAIL;
    }

    ifs.seekg(0, std::ios::end);
    std::vector<unsigned char> buffer(static_cast<unsigned int>(ifs.tellg()));
    ifs.seekg(0, std::ios::beg);
    if (buffer.empty()) {
        return E_FAIL;
    }
    ifs.read((char*)&buffer[0], buffer.size());

    return D3DCompile(&buffer[0], buffer.size(), to_MultiByte(932, pFileName).c_str()
        , pDefines
        , pInclude
        , pEntrypoint
        , pTarget
        , Flags1
        , Flags2
        , ppCode
        , ppErrorMsgs);
}

#endif

HRESULT CompileShaderFromFile
(
    const std::wstring &source,
    const std::wstring &entryPoint,
    const std::wstring &shaderModel,
    ID3DBlob**  ppBlobOut
)
{
    // �R���p�C���t���O.
    DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;

#if defined(DEBUG) || defined(_DEBUG)
    dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif//defiend(DEBUG) || defined(_DEBUG)

#if defined(NDEBUG) || defined(_NDEBUG)
    dwShaderFlags |= D3DCOMPILE_OPTIMIZATION_LEVEL3;
#endif//defined(NDEBUG) || defined(_NDEBUG)

    Microsoft::WRL::ComPtr<ID3DBlob> pErrorBlob;
    auto hr = D3DCompileFromFile(
        source.c_str(),
        NULL,
        D3D_COMPILE_STANDARD_FILE_INCLUDE,
        to_MultiByte(CP_OEMCP, entryPoint).c_str(),
        to_MultiByte(CP_OEMCP, shaderModel).c_str(),
        dwShaderFlags,
        0,
        ppBlobOut,
        &pErrorBlob
    );
    if (FAILED(hr))
    {
        if (pErrorBlob)
        {
            OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());
        }
    }

    return hr;
}

HRESULT CompileShaderFromMemory(
    const std::wstring &source,
    const std::wstring &entryPoint,
    const std::wstring &shaderModel,
    ID3DBlob**  ppBlobOut
)
{
    // �R���p�C���t���O.
    DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;

#if defined(DEBUG) || defined(_DEBUG)
    dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif//defiend(DEBUG) || defined(_DEBUG)

#if defined(NDEBUG) || defined(_NDEBUG)
    dwShaderFlags |= D3DCOMPILE_OPTIMIZATION_LEVEL3;
#endif//defined(NDEBUG) || defined(_NDEBUG)

    Microsoft::WRL::ComPtr<ID3DBlob> pErrorBlob;
    auto mbSource = to_MultiByte(CP_OEMCP, source);
    auto hr = D3DCompile(
        mbSource.c_str(), mbSource.size(), "source_name",
        NULL,
        D3D_COMPILE_STANDARD_FILE_INCLUDE,
        to_MultiByte(CP_OEMCP, entryPoint).c_str(),
        to_MultiByte(CP_OEMCP, shaderModel).c_str(),
        dwShaderFlags,
        0,
        ppBlobOut,
        &pErrorBlob
    );
    if (FAILED(hr))
    {
        if (pErrorBlob)
        {
            OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());
        }
    }

    return hr;
}
