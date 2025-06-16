// TODO implementation
#include "gameObject.h"

// 設定位置
void GameObject::setPosition(Position pos) {
    _pos = pos;
}

// 取得位置
Position GameObject::getPosition() const{
    return _pos;
}

// 設定圖示
void GameObject::setIcon(const Icon& icon) {
    _icon = icon;
}

// 取得圖示
Icon GameObject::getIcon() const {
    return _icon;
}

// 預設 update 不做事，可擴充
void GameObject::update() {

}

