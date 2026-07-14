#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include "NativeBridge.hpp"

using namespace geode::prelude;

class $modify(MyMenuLayer, MenuLayer) {
    bool init() {
        if (!MenuLayer::init()) return false;

        auto& bridge = NativeBridge::get();
        auto dllPath = (Mod::get()->getResourcesDir() / "nativelib.dll.so").string();
        if (bridge.load(dllPath))
            bridge.startBridge();

        auto myButton = CCMenuItemSpriteExtra::create(
            CCSprite::createWithSpriteFrameName("GJ_likeBtn_001.png"),
            this,
            menu_selector(MyMenuLayer::onMyButton)
        );

        auto menu = this->getChildByID("bottom-menu");
        menu->addChild(myButton);
        myButton->setID("my-button"_spr);
        menu->updateLayout();

        return true;
    }

    void onMyButton(CCObject*) {
        NativeBridge::get().pressKey(VK_F7);
    }
};
