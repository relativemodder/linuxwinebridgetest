#pragma once

#include <string>

enum class EnvStatus {
    OK,
    NotWine,
    UnsupportedOS
};

struct EnvInfo {
    EnvStatus   status;
    std::string wineVersion;
    std::string hostSysname;
};

EnvInfo checkEnvironment();
