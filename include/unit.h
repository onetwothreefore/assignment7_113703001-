#ifndef UNIT_H
#define UNIT_H
struct Vec2{
    int e1;
    int e2;
	Vec2() : e1(0), e2(0) {}  // 預設建構子
    Vec2(int x, int y) : e1(x), e2(y) {} // 有參數的建構子
    int& x(){ return e1;}
    int& y(){ return e2;}
    int& width(){return e1;}
    int& height(){return e2;}
	bool operator==(const Vec2& rhs) const {  // 等號運算子
        return e1 == rhs.e1 && e2 == rhs.e2;
    }
};

using Position = Vec2;

enum Color {BLACK=0, RED, GREEN, YELLOW, BLUE, PINK, CYAN, WHITE, NOCHANGE};

enum class Direction { UP, DOWN, LEFT, RIGHT }; //assign6

#endif
