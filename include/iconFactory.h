// TODO 
// Finish the declaration of IconFactory that
// 1. offers some methods to create an Icon.
// 2. encapsulates the constructor from the caller.
#ifndef ICON_FACTORY_H
#define ICON_FACTORY_H

#include "icon.h"
#include "unit.h"
#include <string>

class IconFactory {
public:

    // 建立一個矩形區塊圖示（寬、高）
    static Icon createBlockIcon(int width, int height, char ch, Color color);
};

#endif
 
