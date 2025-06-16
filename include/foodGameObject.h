#pragma once
#include "gameObject.h"

/**
 * FoodGameObject
 * - 支援食物保鮮期 (lifetime)
 * - 每步 decay() 一次，lifetime 歸零自動判定為腐壞
 */
class FoodGameObject : public GameObject {
public:
    // ==== 變動：建構子支援設定保鮮期（預設100，可自訂） ====
    FoodGameObject(Position pos, int lifetime = 300);

    // ==== 變動：每次呼叫會保鮮期 -1 ====
    void decay();

    // ==== 變動：是否已腐壞、應消失 ====
    bool isSpoiled() const;

    // ==== 變動：取得目前剩餘保鮮期 ====
    int lifetime() const;

    // ==== 位置存取維持繼承 GameObject 寫法 ====
private:
    int _lifetime; // ==== 變動：食物的保鮮期 ====
};
