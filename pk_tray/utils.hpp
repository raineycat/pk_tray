#pragma once

#include <Windows.h>
#include <string>
#include <codecvt>
#include <filesystem>
#include <optional>
#include <ShlObj.h>

namespace pk {
    inline std::wstring s2ws(const std::string& str) {
        std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
        return converter.from_bytes(str);
    }

    inline std::string ws2s(const std::wstring& wstr) {
        std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
        return converter.to_bytes(wstr);
    }

    inline std::optional<std::filesystem::path> getDataPath() {
        PWSTR appDataPath;
        if (FAILED(SHGetKnownFolderPath(
            FOLDERID_RoamingAppData,
            0,
            NULL,
            &appDataPath))) {
            return std::nullopt;
        }

        std::filesystem::path path(appDataPath);
        CoTaskMemFree(appDataPath);
        return path / "pk_tray";
    }
}