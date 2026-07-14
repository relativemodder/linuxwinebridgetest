#include <Geode/Geode.hpp>
#include <Geode/loader/Hook.hpp>
#include "NativeBridge.hpp"

using namespace geode::prelude;

static void WINAPI keybd_event_hook(BYTE bVk, BYTE bScan, DWORD dwFlags, ULONG_PTR dwExtraInfo) {
    auto& bridge = NativeBridge::get();
    if (bridge.isLoaded()) {
        if (dwFlags & KEYEVENTF_KEYUP)
            bridge.keyUp(static_cast<DWORD>(bVk));
        else
            bridge.keyDown(static_cast<DWORD>(bVk));
    }
}

$execute {
    auto* addr = reinterpret_cast<void*>(
        GetProcAddress(GetModuleHandleA("user32.dll"), "keybd_event")
    );
    if (addr) {
        auto res = Mod::get()->hook(
            addr,
            &keybd_event_hook,
            "keybd_event",
            tulip::hook::TulipConvention::Stdcall
        );
        if (!res) {
            log::warn("KeybdEventHook: failed to hook keybd_event: {}", res.unwrapErr());
        }
    } else {
        log::warn("KeybdEventHook: keybd_event not found in user32.dll");
    }
}
