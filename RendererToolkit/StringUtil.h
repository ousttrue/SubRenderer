#pragma once
#include <Windows.h>
#include <string>
#include <vector>


inline std::wstring to_WideChar(UINT uCodePage, const std::string &text)
{
    int size = MultiByteToWideChar(uCodePage, 0, text.c_str(), -1, NULL, 0);
    if (size <= 1) {
        return L"";
    }
    std::vector<wchar_t> buf(size - 1);
    size = MultiByteToWideChar(uCodePage, 0, text.c_str(), -1, &buf[0], (int)buf.size());
    return std::wstring(buf.begin(), buf.end());
}

inline std::string to_MultiByte(UINT uCodePage, const std::wstring &text)
{
    int size = WideCharToMultiByte(uCodePage, 0, text.c_str(), -1, NULL, 0, 0, NULL);
    if (size <= 1) {
        return "";
    }
    std::vector<char> buf(size - 1);
    size = WideCharToMultiByte(uCodePage, 0, text.c_str(), -1, &buf[0], (int)buf.size(), 0, NULL);
    return std::string(buf.begin(), buf.end());
}

inline bool endswith(const std::string &lhs, const std::string &rhs)
{
    if (lhs.size() < rhs.size())
    {
        return false;
    }

    auto lit = lhs.begin() + (lhs.size() - rhs.size());
    for (auto it = rhs.begin(); it != rhs.end(); ++it, ++lit)
    {
        if (*lit != *it)
        {
            return false;
        }
    }

    return true;
}

