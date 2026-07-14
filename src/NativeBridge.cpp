#include "NativeBridge.hpp"
#include "VkMapper.hpp"
#include <Geode/Geode.hpp>
#include <optional>
#include <chrono>

using namespace geode::prelude;

static constexpr int RESP_BUF_SIZE = 4096;

NativeBridge& NativeBridge::get() {
    static NativeBridge instance;
    return instance;
}

NativeBridge::~NativeBridge() {
    m_workerRunning = false;
    m_queueCv.notify_all();
    if (m_worker.joinable()) m_worker.join();

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
    int rc = m_startBridge();
    log::info("NativeBridge: NativeStartBridge returned {}", rc);
    m_bridgeStarted = rc == 0;
    if (m_bridgeStarted) startWorker();
    return m_bridgeStarted;
}

bool NativeBridge::isLoaded() const {
    return m_lib != nullptr;
}

std::optional<nlohmann::json> NativeBridge::call(const std::string& cmd,
                                                   const nlohmann::json& args) {
    if (!m_sendCommand) {
        log::warn("NativeBridge::call: m_sendCommand is null");
        return std::nullopt;
    }

    nlohmann::json req = { {"cmd", cmd}, {"args", args} };
    std::string req_str = req.dump();

    char resp_buf[RESP_BUF_SIZE] = {};
    int rc = m_sendCommand(req_str.c_str(), resp_buf, RESP_BUF_SIZE);
    if (rc != 0) {
        log::warn("NativeBridge::call: NativeSendCommand returned {} for cmd '{}'", rc, cmd);
        return std::nullopt;
    }

    try {
        return nlohmann::json::parse(resp_buf);
    } catch (...) {
        log::warn("NativeBridge::call: failed to parse response for cmd '{}'", cmd);
        return std::nullopt;
    }
}

void NativeBridge::startWorker() {
    m_workerRunning = true;
    m_worker = std::thread(&NativeBridge::workerLoop, this);
}

void NativeBridge::workerLoop() {
    while (m_workerRunning) {
        QueueItem item;
        {
            std::unique_lock<std::mutex> lock(m_queueMutex);
            m_queueCv.wait(lock, [this] {
                return !m_queue.empty() || !m_workerRunning;
            });
            if (!m_workerRunning && m_queue.empty()) break;
            item = std::move(m_queue.front());
            m_queue.pop();
        }
        call(item.cmd, item.args);
    }
}

void NativeBridge::callAsync(const std::string& cmd, const nlohmann::json& args) {
    {
        std::lock_guard<std::mutex> lock(m_queueMutex);
        m_queue.push({ cmd, args });
    }
    m_queueCv.notify_one();
}

void NativeBridge::keyDown(DWORD vk) {
    auto keysym = vkToKeySym(vk);
    if (!keysym) return;
    callAsync("key_down", {{"keysym", *keysym}});
}

void NativeBridge::keyUp(DWORD vk) {
    auto keysym = vkToKeySym(vk);
    if (!keysym) return;
    callAsync("key_up", {{"keysym", *keysym}});
}

void NativeBridge::keyPress(DWORD vk, int holdMs) {
    auto keysym = vkToKeySym(vk);
    if (!keysym) return;

    callAsync("key_down", {{"keysym", *keysym}});
    std::thread([this, ks = *keysym, holdMs]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(holdMs));
        callAsync("key_up", {{"keysym", ks}});
    }).detach();
}
