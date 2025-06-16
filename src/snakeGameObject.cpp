#include "snakeGameObject.h"
#include "iconFactory.h"
#include "environment.h"
#include <algorithm>

SnakeGameObject::SnakeGameObject(Position start)
    : _dir(Direction::RIGHT), _growFlag(false) {
    _body.push_back(start);
    setPosition(start);
    setIcon(IconFactory::createBlockIcon(1, 1, 'O', Color::GREEN));
}

void SnakeGameObject::setDirection(Direction dir) {
    // 防止直接反向
    if ((_dir == Direction::UP && dir == Direction::DOWN) ||
        (_dir == Direction::DOWN && dir == Direction::UP) ||
        (_dir == Direction::LEFT && dir == Direction::RIGHT) ||
        (_dir == Direction::RIGHT && dir == Direction::LEFT)) {
        return;
    }
    _dir = dir;
}

Direction SnakeGameObject::getDirection() const {
    return _dir;
}

void SnakeGameObject::update() {
    Position new_head = _body.front();
    switch (_dir) {
        case Direction::UP:    new_head.y() -= 1; break;
        case Direction::DOWN:  new_head.y() += 1; break;
        case Direction::LEFT:  new_head.x() -= 1; break;
        case Direction::RIGHT: new_head.x() += 1; break;
    }
    _body.push_front(new_head);
    setPosition(new_head);
    setIcon(IconFactory::createBlockIcon(1, 1, 'O', Color::GREEN));
    if (!_growFlag) {
        _body.pop_back();
    } else {
        _growFlag = false;
    }
}

void SnakeGameObject::grow() {
    _growFlag = true;
}

bool SnakeGameObject::hitSelf() const {
    Position h = _body.front();
    return std::count(_body.begin() + 1, _body.end(), h) > 0;
}

bool SnakeGameObject::hitWall() const {
    Position h = _body.front();
    return h.x() < 0 || h.x() >= GAME_WINDOW_WIDTH ||
           h.y() < 0 || h.y() >= GAME_WINDOW_HEIGHT;
}

Position SnakeGameObject::head() const {
    return _body.front();
}

const std::deque<Position>& SnakeGameObject::body() const {
    return _body;
}
