#pragma once

#include <windows.h>
#include <string>
#include <optional>

std::optional<std::string> vkToKeySym(DWORD vk);
