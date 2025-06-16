#include "view.h"
#include "environment.h"
#include "AnsiPrint.h"
#include <codecvt>      // std::wstring_convert
#include <cwchar>       // wcwidth
#include <sys/ioctl.h>   // winsize, TIOCGWINSZ
#include <unistd.h>  //  STDOUT_FILENO
#include <iostream>
#include <locale>
#include <utility>

// return the actual length in terminal of a string.
int displayWidth(const std::string& utf8) {
    static std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> conv;
    auto u32 = conv.from_bytes(utf8);

    int w = 0;
    for (char32_t ch : u32)
    w += std::max(0, ::wcwidth(static_cast<wchar_t>(ch)));
    return std::max(1, w);
}

View::View() {
    std::setlocale(LC_ALL, "");
    
    auto make_str_buf = [] {
        return std::vector<std::vector<std::string>>(
            GAME_WINDOW_HEIGHT, std::vector<std::string>(GAME_WINDOW_WIDTH, ""));
    };
    auto make_col_buf = [] {
        return std::vector<std::vector<Color>>(
            GAME_WINDOW_HEIGHT, std::vector<Color>(GAME_WINDOW_WIDTH, Color::NOCHANGE));
    };

    latest_map   = last_map   = make_str_buf();
    latest_fg_color = last_fg_color = make_col_buf();
    latest_bg_color = last_bg_color = make_col_buf();
    resetLatest();

}

void View::updateGameObject(GameObject* obj) {
    Icon icon = obj->getIcon();
        Position pos  = obj->getPosition();

        for (std::size_t dy = 0; dy < icon.size(); ++dy) {
            int row = pos.y() + static_cast<int>(dy);
            if (row < 0 || row >= GAME_WINDOW_HEIGHT) continue;

            for (std::size_t dx = 0; dx < icon[dy].size(); ++dx) {
                int col = pos.x() + static_cast<int>(dx);
                if (col < 0 || col >= GAME_WINDOW_WIDTH) continue;

                const Cell& cell = icon[dy][dx];
                latest_map      [row][col] = cell.ascii;
                latest_bg_color [row][col] = cell.color;
            }
        }

}

void View::render(){
    auto [rows, cols] = get_terminal_size();
    bool resized = (rows != _termHeight || cols != _termWidth);
    _termHeight = rows;
    _termWidth  = cols;

    if (resized) std::cout << "\033[2J\033[H";

    bool dirty = (   last_map      != latest_map
        || last_fg_color != latest_fg_color
        || last_bg_color != latest_bg_color);
    if (!dirty) return;

    std::string frame;
    frame.reserve((GAME_WINDOW_HEIGHT + 2) * (GAME_WINDOW_CELL_WIDTH * GAME_WINDOW_WIDTH + 3));


    // Top line

    frame += '+' + std::string(GAME_WINDOW_WIDTH * GAME_WINDOW_CELL_WIDTH, '-') + "+\n";

    for (int r = 0; r < GAME_WINDOW_HEIGHT; ++r) {
        int rowScreen = r + 2;

        // Left line
        frame += '|';

        int pixelCol = 1;

        for (int c = 0; c < GAME_WINDOW_WIDTH; ++c) {
            const std::string& txt = latest_map[r][c];
            const auto         fg  = latest_fg_color[r][c];
            const auto         bg  = latest_bg_color[r][c];

            int w        = std::max(1, displayWidth(txt));
            int padTotal = GAME_WINDOW_CELL_WIDTH - w;
            int padLeft  = padTotal / 2;
            int padRight = padTotal - padLeft;

            // Left blank
            for (int p = 0; p < padLeft; ++p) {
                frame += AnsiPrint(" ", Color::NOCHANGE, bg, false, false);
            }

            // icon 
            frame += AnsiPrint(txt.c_str(), fg, bg, false, false);

            // Right 
            for (int p = 0; p < padRight; ++p) {
                frame += AnsiPrint(" ", Color::NOCHANGE, bg, false, false);
            }

            pixelCol += GAME_WINDOW_CELL_WIDTH;
        }


        // Right line
        frame += "|\n";

    }
    // Bottom line
    frame += '+' + std::string(GAME_WINDOW_WIDTH * GAME_WINDOW_CELL_WIDTH, '-') + "+\n";

    std::cout << "\033[H" << frame << std::flush;


    // update buffer
    last_map      = latest_map;
    last_fg_color = latest_fg_color;
    last_bg_color = latest_bg_color;
}


void View::resetLatest(){
    for (int r = 0; r < GAME_WINDOW_HEIGHT; ++r) {
        std::fill(latest_map[r].begin(),   latest_map[r].end(),   " ");
        std::fill(latest_fg_color[r].begin(), latest_fg_color[r].end(), Color::NOCHANGE);
        std::fill(latest_bg_color[r].begin(), latest_bg_color[r].end(), Color::NOCHANGE);
    }
}
std::pair<int,int> View::get_terminal_size() {
    int rows, cols;
    struct winsize w;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == 0) {
        rows = w.ws_row;
        cols = w.ws_col;
    } else {
        rows = cols = -1; // Error case
    }
    return std::make_pair(rows, cols);
}

