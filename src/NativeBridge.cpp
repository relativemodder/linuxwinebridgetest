#include "NativeBridge.hpp"
#include "VkMapper.hpp"
#include <optional>
#include <thread>

static constexpr int RESP_BUF_SIZE = 4096;
NativeBridge& NativeBridge::get() {
    static NativeBridge instance;
    return instance;
}

NativeBridge::~NativeBridge() {
    if (m_lib) FreeLibrary(m_lib);
}

bool NativeBridge::load(const std::string& dllPath) {
    if (m_lib) return true;

    m_lib = LoadLibraryA(dllPath.c_str());
    if (!m_lib) return false;

    m_startBridge = reinterpret_cast<StartBridgeFn>(GetProcAddress(m_lib, "NativeStartBridge"));
    m_sendCommand = reinterpret_cast<SendCommandFn>(GetProcAddress(m_lib, "NativeSendCommand"));

    if (!m_startBridge || !m_sendCommand) {
        FreeLibrary(m_lib);
        m_lib = nullptr;
        return false;
    }

    return true;
}

bool NativeBridge::startBridge() {
    if (!m_startBridge || m_bridgeStarted) return false;
    m_bridgeStarted = m_startBridge() == 0;
    return m_bridgeStarted;
}

bool NativeBridge::isLoaded() const {
    return m_lib != nullptr;
}

std::optional<nlohmann::json> NativeBridge::call(const std::string& cmd,
                                                   const nlohmann::json& args) {
    if (!m_sendCommand) return std::nullopt;

    nlohmann::json req = { {"cmd", cmd}, {"args", args} };
    std::string req_str = req.dump();

    char resp_buf[RESP_BUF_SIZE] = {};
    int rc = m_sendCommand(req_str.c_str(), resp_buf, RESP_BUF_SIZE);
    if (rc != 0) return std::nullopt;

    try {
        return nlohmann::json::parse(resp_buf);
    } catch (...) {
        return std::nullopt;
    }
}

void NativeBridge::callAsync(const std::string& cmd, const nlohmann::json& args) {
    std::thread([this, cmd, args]() {
        call(cmd, args);
    }).detach();
}

void NativeBridge::pressKey(DWORD vk) {
    auto keysym = vkToKeySym(vk);
    if (!keysym) return;
    callAsync("key_press", {{"keysym", *keysym}});
}
