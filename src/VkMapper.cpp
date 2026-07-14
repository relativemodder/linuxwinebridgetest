#include "VkMapper.hpp"
#include <unordered_map>

std::optional<std::string> vkToKeySym(DWORD vk) {
    static const std::unordered_map<DWORD, std::string> kMap = {
        { VK_BACK,      "BackSpace" },
        { VK_TAB,       "Tab"       },
        { VK_RETURN,    "Return"    },
        { VK_ESCAPE,    "Escape"    },
        { VK_SPACE,     "space"     },
        { VK_DELETE,    "Delete"    },
        { VK_INSERT,    "Insert"    },
        { VK_HOME,      "Home"      },
        { VK_END,       "End"       },
        { VK_PRIOR,     "Prior"     },  // pageup
        { VK_NEXT,      "Next"      },  // pagedown
        { VK_LEFT,      "Left"      },
        { VK_RIGHT,     "Right"     },
        { VK_UP,        "Up"        },
        { VK_DOWN,      "Down"      },

        { VK_SHIFT,     "Shift_L"   },
        { VK_CONTROL,   "Control_L" },
        { VK_MENU,      "Alt_L"     },  // alt
        { VK_LSHIFT,    "Shift_L"   },
        { VK_RSHIFT,    "Shift_R"   },
        { VK_LCONTROL,  "Control_L" },
        { VK_RCONTROL,  "Control_R" },
        { VK_LMENU,     "Alt_L"     },
        { VK_RMENU,     "Alt_R"     },

        { VK_F1,        "F1"  }, { VK_F2,  "F2"  }, { VK_F3,  "F3"  },
        { VK_F4,        "F4"  }, { VK_F5,  "F5"  }, { VK_F6,  "F6"  },
        { VK_F7,        "F7"  }, { VK_F8,  "F8"  }, { VK_F9,  "F9"  },
        { VK_F10,       "F10" }, { VK_F11, "F11" }, { VK_F12, "F12" },

        { '0', "0" }, { '1', "1" }, { '2', "2" }, { '3', "3" }, { '4', "4" },
        { '5', "5" }, { '6', "6" }, { '7', "7" }, { '8', "8" }, { '9', "9" },

        { 'A', "a" }, { 'B', "b" }, { 'C', "c" }, { 'D', "d" }, { 'E', "e" },
        { 'F', "f" }, { 'G', "g" }, { 'H', "h" }, { 'I', "i" }, { 'J', "j" },
        { 'K', "k" }, { 'L', "l" }, { 'M', "m" }, { 'N', "n" }, { 'O', "o" },
        { 'P', "p" }, { 'Q', "q" }, { 'R', "r" }, { 'S', "s" }, { 'T', "t" },
        { 'U', "u" }, { 'V', "v" }, { 'W', "w" }, { 'X', "x" }, { 'Y', "y" },
        { 'Z', "z" },

        { VK_NUMPAD0, "KP_0" }, { VK_NUMPAD1, "KP_1" }, { VK_NUMPAD2, "KP_2" },
        { VK_NUMPAD3, "KP_3" }, { VK_NUMPAD4, "KP_4" }, { VK_NUMPAD5, "KP_5" },
        { VK_NUMPAD6, "KP_6" }, { VK_NUMPAD7, "KP_7" }, { VK_NUMPAD8, "KP_8" },
        { VK_NUMPAD9, "KP_9" },
        { VK_MULTIPLY,  "KP_Multiply" },
        { VK_ADD,       "KP_Add"      },
        { VK_SUBTRACT,  "KP_Subtract" },
        { VK_DECIMAL,   "KP_Decimal"  },
        { VK_DIVIDE,    "KP_Divide"   },
    };

    auto it = kMap.find(vk);
    if (it == kMap.end()) return std::nullopt;
    return it->second;
}
