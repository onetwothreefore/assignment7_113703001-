#include "foodGameObject.h"
#include "iconFactory.h"

/**
 * 建構子
 * @param pos   食物出現的位置
 * @param lifetime  初始保鮮期
 */
FoodGameObject::FoodGameObject(Position pos, int lifetime)
    : _lifetime(lifetime)
{
    setPosition(pos);
    // 圖示預設黃色，顏色會依保鮮期顯示可由 controller 控制
    setIcon(IconFactory::createBlockIcon(1, 1, '*', Color::YELLOW));
}

/**
 * 每回合呼叫一次，保鮮期 -1
 */
void FoodGameObject::decay() {
    --_lifetime;
}

/**
 * 是否已腐壞
 */
bool FoodGameObject::isSpoiled() const {
    return _lifetime <= 0;
}

/**
 * 查詢剩餘保鮮期
 */
int FoodGameObject::lifetime() const {
    return _lifetime;
}
