#include <iostream>
#include <ctime>
#include <thread>
#include <unistd.h>
#include <termios.h>
#include <random>
#include <algorithm>
#include <vector>
#include <iomanip>

#include "controller.h"
#include "snakeGameObject.h"
#include "foodGameObject.h"
#include "environment.h"
#include "AnsiPrint.h"
#include "iconFactory.h"


const int FOOD_LIFETIME = 60; // 每顆食物存活 步
const int ENEMY_RESPAWN_STEPS = 100; // 敵方蛇死後隔幾步復活

Controller::Controller(View& view) : _view(view) {}

static struct termios old_termios, new_termios;

void reset_terminal() {
    printf("\e[m");
    printf("\e[?25h");
    fflush(stdout);
    tcsetattr(STDIN_FILENO, TCSANOW, &old_termios);
}

void configure_terminal() {
    tcgetattr(STDIN_FILENO, &old_termios);
    new_termios = old_termios;
    new_termios.c_lflag &= ~(ICANON | ECHO);
    new_termios.c_cc[VMIN] = 0;
    new_termios.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSANOW, &new_termios);
    printf("\e[?25l");
    std::atexit(reset_terminal);
}

int read_input() {
    fflush(stdout);
    char buf[4096];
    int n = read(STDIN_FILENO, buf, sizeof(buf));
    return n > 0 ? buf[n - 1] : -1;
}

// ==== 食物產生（不和任一蛇及食物重疊，含保鮮期）====
FoodGameObject create_food(const std::deque<Position>& snake_body,
                          const std::deque<Position>& enemy_body,
                          const std::vector<FoodGameObject>& foods) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist_x(0, GAME_WINDOW_WIDTH - 1);
    std::uniform_int_distribution<> dist_y(0, GAME_WINDOW_HEIGHT - 1);
    while (true) {
        Position p(dist_x(gen), dist_y(gen));
        bool overlap = false;
        for (const auto& s : snake_body)
            if (p == s) { overlap = true; break; }
        for (const auto& e : enemy_body)
            if (p == e) { overlap = true; break; }
        for (const auto& f : foods)
            if (p == f.getPosition()) { overlap = true; break; }
        if (!overlap) return FoodGameObject(p, FOOD_LIFETIME);
    }
}

// ==== 讓敵方蛇隨機決定合法方向（不可原地反向）====
Direction random_direction(Direction now_dir) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::vector<Direction> dirs;
    for (Direction d : {Direction::UP, Direction::DOWN, Direction::LEFT, Direction::RIGHT}) {
        if ((now_dir == Direction::UP && d == Direction::DOWN) ||
            (now_dir == Direction::DOWN && d == Direction::UP) ||
            (now_dir == Direction::LEFT && d == Direction::RIGHT) ||
            (now_dir == Direction::RIGHT && d == Direction::LEFT))
            continue;
        dirs.push_back(d);
    }
    std::uniform_int_distribution<> dist(0, dirs.size() - 1);
    return dirs[dist(gen)];
}

void Controller::run() {
    std::cin.tie(0);
    std::ios::sync_with_stdio(0);
    configure_terminal();

    int score = 0;
    SnakeGameObject snake(Position(GAME_WINDOW_WIDTH/2, GAME_WINDOW_HEIGHT/2));
    SnakeGameObject enemySnake(Position(GAME_WINDOW_WIDTH/4, GAME_WINDOW_HEIGHT/4));
    bool enemy_alive = true;
    int enemy_respawn_counter = 0; // === 新增：敵方蛇死亡後倒數

    std::vector<FoodGameObject> foods;
    foods.emplace_back(create_food(snake.body(), enemySnake.body(), foods));

    // 動態速度設定
    const double SPF_START = 0.5;
    const double SPF_MIN   = 0.08;
    const double SPF_DECAY = 0.015;
    bool running = true;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> food_num_dist(1, 3);

    while (running) {
        double SPF = std::max(SPF_MIN, SPF_START - score * SPF_DECAY);

        clock_t start = clock();
        int input = read_input();
        switch (input) {
            case 'w': snake.setDirection(Direction::UP); break;
            case 's': snake.setDirection(Direction::DOWN); break;
            case 'a': snake.setDirection(Direction::LEFT); break;
            case 'd': snake.setDirection(Direction::RIGHT); break;
            case 27:  running = false; continue;
        }
        snake.update();

        // === 敵方蛇移動，若已死亡則倒數重生 ===
        if (enemy_alive) {
            Direction next_dir = random_direction(enemySnake.getDirection());
            enemySnake.setDirection(next_dir);
            enemySnake.update();
        } else if (enemy_respawn_counter >= 0) {
            --enemy_respawn_counter;
            if (enemy_respawn_counter <= 0) {
				
				std::cout<< " 嘗試隨機放一條新敵方蛇!! " << std::endl;
                // 嘗試隨機放一條新敵方蛇
                Position newEnemyPos;
                bool found = false;
                std::uniform_int_distribution<> dist_x(0, GAME_WINDOW_WIDTH - 1);
                std::uniform_int_distribution<> dist_y(0, GAME_WINDOW_HEIGHT - 1);
                for (int tries = 0; tries < 100; ++tries) {
                    newEnemyPos = Position(dist_x(gen), dist_y(gen));
                    bool overlap = false;
                    for (const auto& s : snake.body()) if (s == newEnemyPos) { overlap = true; break; }
                    for (const auto& f : foods) if (f.getPosition() == newEnemyPos) { overlap = true; break; }
                    if (!overlap) { found = true; break; }
                }
                if (found) {
                    enemySnake = SnakeGameObject(newEnemyPos);
                    enemy_alive = true;
                }
                // 找不到就下回合再試
            }
        }

        // ==== 兩蛇互撞判斷 ====
        Position playerHead = snake.head();
        Position enemyHead = enemySnake.head();
        bool playerCrash = false;
        bool enemyCrash = false;

        if (enemy_alive) {
            for (const auto& seg : enemySnake.body()) {
                if (playerHead == seg) {
                    playerCrash = true;
                    break;
                }
            }
        }
        for (const auto& seg : snake.body()) {
            if (enemy_alive && enemyHead == seg) {
                enemyCrash = true;
                break;
            }
        }
        if (enemy_alive && playerHead == enemyHead) {
            playerCrash = true;
            enemyCrash = true;
        }

        // ==== 死亡判斷與處理 ====
		
		if (foods.empty()) {
			_view.render();
			std::cout << AnsiPrint("Game Over! ", YELLOW, RED, true, true)
					  << "（餓死，場上已無食物） Score: " << score << std::endl;
			break;
		}
		
        if (snake.hitWall()) {
            _view.render();
            std::cout << AnsiPrint("Game Over! ", YELLOW, RED, true, true)
                      << "（撞牆） Score: " << score << std::endl;
            break;
        }
        if (snake.hitSelf()) {
            _view.render();
            std::cout << AnsiPrint("Game Over! ", YELLOW, RED, true, true)
                      << "（咬到自己） Score: " << score << std::endl;
            break;
        }
        if (playerCrash) {
            _view.render();
            if (enemyCrash)
                std::cout << AnsiPrint("Game Over! ", YELLOW, RED, true, true)
                          << "（雙頭對撞） Score: " << score << std::endl;
            else
                std::cout << AnsiPrint("Game Over! ", YELLOW, RED, true, true)
                          << "（撞到敵方蛇） Score: " << score << std::endl;
            break;
        }
        // === 敵方蛇死亡就倒數 respawn ===
        if ((enemyCrash && enemy_alive) || (enemy_alive && enemySnake.hitWall()) || (enemy_alive && enemySnake.hitSelf())) {
            enemy_alive = false;
            enemy_respawn_counter = ENEMY_RESPAWN_STEPS;
        }

        // ==== 食物 decay & 消失 ====
        for (auto it = foods.begin(); it != foods.end(); ) {
            it->decay();
            if (it->isSpoiled()) {
                it = foods.erase(it);
            } else {
                ++it;
            }
        }

        // ==== 吃食物判斷 ====
        bool player_ate_food = false, enemy_ate_food = false;
        for (auto it = foods.begin(); it != foods.end(); ) {
            bool erased = false;
            if (snake.head() == it->getPosition()) {
                snake.grow();
                score++;
                it = foods.erase(it);
                player_ate_food = true;
                erased = true;
            }
            if (!erased && enemy_alive && enemySnake.head() == it->getPosition()) {
                enemySnake.grow();
                it = foods.erase(it);
                enemy_ate_food = true;
                erased = true;
            }
            if (!erased) ++it;
        }
        if (player_ate_food || enemy_ate_food) {
            int new_food_num = food_num_dist(gen);
            for (int i = 0; i < new_food_num; ++i) {
                foods.emplace_back(create_food(snake.body(),
                                              enemy_alive ? enemySnake.body() : std::deque<Position>{},
                                              foods));
            }
        }

        // ==== 畫面顯示 ====
        _view.resetLatest();
        // 玩家蛇
        bool first = true;
        for (const auto& seg : snake.body()) {
            GameObject part;
            part.setPosition(seg);
            if (first)
                part.setIcon(IconFactory::createBlockIcon(1, 1, '@', Color::GREEN));
            else
                part.setIcon(IconFactory::createBlockIcon(1, 1, 'o', Color::GREEN));
            _view.updateGameObject(&part);
            first = false;
        }
        // 敵方蛇
        if (enemy_alive) {
            bool enemy_first = true;
            for (const auto& seg : enemySnake.body()) {
                GameObject part;
                part.setPosition(seg);
                if (enemy_first)
                    part.setIcon(IconFactory::createBlockIcon(1, 1, '@', Color::RED));
                else
                    part.setIcon(IconFactory::createBlockIcon(1, 1, 'o', Color::RED));
                _view.updateGameObject(&part);
                enemy_first = false;
            }
        }
        // 食物（保鮮顏色）
        for (const auto& food : foods) {
            int remain = food.lifetime();
            Color food_color = (remain > FOOD_LIFETIME*2/3) ? Color::YELLOW :
                               (remain > FOOD_LIFETIME/3)  ? Color::PINK :
                               Color::RED;
            GameObject food_disp;
            food_disp.setPosition(food.getPosition());
            food_disp.setIcon(IconFactory::createBlockIcon(1, 1, '*', food_color));
            _view.updateGameObject(&food_disp);
        }

        _view.render();

        // 分數與速度
        std::cout << "\nScore: " << score << " | Speed: "
                  //<< std::fixed << std::setprecision(3)
                  //<< (1.0/SPF) << " steps/sec"
				  << SPF << " src per step"
                  << " | Foods: " << foods.size()
                  << " | Enemy: " << (enemy_alive ? "ACTIVE" : ("respawn in " + std::to_string(enemy_respawn_counter)))
                  << std::endl;

        clock_t end = clock();
        double time_taken = ((double)(end - start)) / CLOCKS_PER_SEC;
        int frameDelay = int((SPF - time_taken) * 1000);
        if (frameDelay > 0)
            std::this_thread::sleep_for(std::chrono::milliseconds(frameDelay));
    }
}
