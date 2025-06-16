#include "gameObject.h"
#include <deque>
#include "unit.h" // 這裡取得 Direction

//enum class Direction { UP, DOWN, LEFT, RIGHT };

class SnakeGameObject : public GameObject {
public:
    SnakeGameObject(Position start);

    void setDirection(Direction dir);
    Direction getDirection() const;

    void update() override; // 移動蛇
    void grow();
    bool hitSelf() const;
    bool hitWall() const;
    Position head() const;
    const std::deque<Position>& body() const;

private:
    std::deque<Position> _body;
    Direction _dir;
    bool _growFlag;
};
