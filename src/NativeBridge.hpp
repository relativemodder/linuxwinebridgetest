#pragma once

#include <windows.h>
#include <string>
#include <optional>
#include "json.hpp"

class NativeBridge {
public:
    static NativeBridge& get();

    bool load(const std::string& dllPath);
    bool startBridge();
    bool isLoaded() const;

    std::optional<nlohmann::json> call(const std::string& cmd,
                                       const nlohmann::json& args = nullptr);

    void callAsync(const std::string& cmd, const nlohmann::json& args = nullptr);

    void pressKey(DWORD vk);

private:
    NativeBridge() = default;
    ~NativeBridge();
    NativeBridge(const NativeBridge&) = delete;
    NativeBridge& operator=(const NativeBridge&) = delete;

    typedef int (WINAPI *StartBridgeFn)();
    typedef int (WINAPI *SendCommandFn)(const char*, char*, int);

    HMODULE m_lib = nullptr;
    bool m_bridgeStarted = false;
    StartBridgeFn m_startBridge = nullptr;
    SendCommandFn m_sendCommand = nullptr;
};
