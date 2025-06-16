// TODO implementation
#include "iconFactory.h"


// 建立一個矩形圖示（由指定字元與顏色填滿）
Icon IconFactory::createBlockIcon(int width, int height, char ch, Color color) {
    Icon icon;
    for (int i = 0; i < height; ++i) {
        std::vector<Cell> row;
        for (int j = 0; j < width; ++j) {
            row.push_back(Cell{color,std::string(1, ch)});
        }
        icon.push_back(row);
    }
    return icon;
}

