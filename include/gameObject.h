#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <vector>
#include "unit.h"
#include "icon.h"
#include <iostream>
class GameObject{

public:

    void setPosition(Position pos);
	Position getPosition() const;
	void setIcon(const Icon& icon);
    Icon getIcon() const;
    virtual void update();
protected:

    Position _pos;
    Icon _icon;
};

#endif
