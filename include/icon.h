#ifndef ICON_H
#define ICON_H

#include <vector>
#include <string>

#include "unit.h"

struct Cell{
    Color color;
    std::string ascii;
    Cell(Color c, std::string a) : color(c), ascii(a){}
};

using Icon = std::vector<std::vector<Cell>>;

size_t icon_width(const Icon& icon);

size_t icon_height(const Icon& icon); 


#endif
