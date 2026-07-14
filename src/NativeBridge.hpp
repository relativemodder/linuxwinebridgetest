#pragma once

#include <windows.h>
#include <string>
#include <optional>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <atomic>
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

    void keyDown(DWORD vk);
    void keyUp(DWORD vk);
    void keyPress(DWORD vk, int holdMs = 50);

private:
    NativeBridge() = default;
    ~NativeBridge();
    NativeBridge(const NativeBridge&) = delete;
    NativeBridge& operator=(const NativeBridge&) = delete;

    void startWorker();
    void workerLoop();

    typedef int (WINAPI *StartBridgeFn)();
    typedef int (WINAPI *SendCommandFn)(const char*, char*, int);

    HMODULE m_lib = nullptr;
    bool m_bridgeStarted = false;
    StartBridgeFn m_startBridge = nullptr;
    SendCommandFn m_sendCommand = nullptr;

    struct QueueItem {
        std::string cmd;
        nlohmann::json args;
    };

    std::queue<QueueItem>    m_queue;
    std::mutex               m_queueMutex;
    std::condition_variable  m_queueCv;
    std::atomic<bool>        m_workerRunning { false };
    std::thread              m_worker;
};
