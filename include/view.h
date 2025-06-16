#ifndef VIEW_H
#define VIEW_H
#include "unit.h"
#include "gameObject.h"
#include <string>
#include <vector>
#include <string>
#include <utility>

class View{
public:
    View();
    void render();
    void updateGameObject(GameObject*);
    void resetLatest();
private:
    int _termWidth;
    int _termHeight;
    std::vector<std::vector<std::string>> latest_map, last_map;
    std::vector<std::vector<Color>>       latest_fg_color, last_fg_color;
    std::vector<std::vector<Color>>       latest_bg_color, last_bg_color;
    static std::pair<int, int> get_terminal_size();
};

#endif
