#include "../CrystalProfile.hpp"
#include <Geode/modify/MenuLayer.hpp>

std::string image;

class $modify(MenuLayer) {
	bool init() {
		MenuLayer::init();

        if (getVar<bool>("better_BG")) {
			image = geode::Mod::get()->getConfigDir().append("BetterBG").append("background.jpg");

			if (ghc::filesystem::exists(image)) {
				auto bg = this->getChildByID("main-menu-bg");
				auto winSize = CCDirector::sharedDirector()->getWinSize();
				auto sprite = CCSprite::create(image.c_str());

				bg->setVisible(false);

				sprite->setScaleY(winSize.height / sprite->getContentSize().height);
				sprite->setScaleX(winSize.width / sprite->getContentSize().width);
				sprite->setPositionX(winSize.width / 2);
				sprite->setPositionY(winSize.height / 2);

				this->addChild(sprite, -1);
			} else {
				CrystalClient::get()->noImage(this);
			}
		}

        return true;
    }
};