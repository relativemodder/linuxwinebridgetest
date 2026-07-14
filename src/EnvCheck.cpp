#include "EnvCheck.hpp"
#include <windows.h>

typedef const char* (CDECL *WineGetVersionFn)();
typedef void (CDECL *WineGetHostVersionFn)(const char** sysname, const char** release);

EnvInfo checkEnvironment() {
    HMODULE ntdll = GetModuleHandleA("ntdll.dll");
    if (!ntdll) {
        return { EnvStatus::NotWine, {}, {} };
    }

    auto wineGetVersion = reinterpret_cast<WineGetVersionFn>(
        GetProcAddress(ntdll, "wine_get_version")
    );

    if (!wineGetVersion) {
        return { EnvStatus::NotWine, {}, {} }; // this is Windows
    }

    std::string wineVer = wineGetVersion();

    auto wineGetHostVersion = reinterpret_cast<WineGetHostVersionFn>(
        GetProcAddress(ntdll, "wine_get_host_version")
    );

    std::string sysname;
    if (wineGetHostVersion) {
        const char* sys = nullptr;
        const char* rel = nullptr;
        wineGetHostVersion(&sys, &rel);
        if (sys) sysname = sys;
    }

    if (sysname.empty() || sysname == "Linux") {
        return { EnvStatus::OK, wineVer, sysname };
    }

    return { EnvStatus::UnsupportedOS, wineVer, sysname };
}
