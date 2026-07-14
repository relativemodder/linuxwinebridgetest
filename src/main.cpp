#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include "NativeBridge.hpp"
#include "EnvCheck.hpp"

using namespace geode::prelude;

static EnvInfo s_env;

$execute {
    s_env = checkEnvironment();
    log::info("EnvCheck: status={} wineVersion='{}' hostSysname='{}'",
        static_cast<int>(s_env.status), s_env.wineVersion, s_env.hostSysname);

    if (s_env.status != EnvStatus::OK) return;

    auto& bridge = NativeBridge::get();
    auto dllPath = (Mod::get()->getResourcesDir() / "nativelib.dll.so").string();
    log::info("NativeBridge: loading DLL from {}", dllPath);

    if (!bridge.load(dllPath)) {
        log::error("NativeBridge: failed to load DLL (GetLastError={})", GetLastError());
    } else {
        log::info("NativeBridge: DLL loaded");
        if (!bridge.startBridge()) {
            log::error("NativeBridge: startBridge failed");
        } else {
            log::info("NativeBridge: bridge started");
        }
    }
}

class $modify(MyMenuLayer, MenuLayer) {
    bool init() {
        if (!MenuLayer::init()) return false;

        if (s_env.status != EnvStatus::OK) {
            this->runAction(
                CCCallFunc::create(this, callfunc_selector(MyMenuLayer::showEnvWarning)));
        }

        return true;
    }

    void showEnvWarning() {
        std::string msg;
        if (s_env.status == EnvStatus::NotWine) {
            msg = "Penguin Tweaks requires Wine/Proton.\n"
                  "The mod will not function on Windows.";
        } else if (s_env.status == EnvStatus::UnsupportedOS) {
            msg = fmt::format(
                "Penguin Tweaks requires Linux as the host OS.\n"
                "Your host: <cy>{}</c>",
                s_env.hostSysname
            );
        }
        FLAlertLayer::create("Penguin Tweaks", msg, "OK")->show();
    }
};
