#include <iostream>
#include "src/Game.h"

int main() {
    // 設定隨機種子（已在 Game 建構子中設定）
    Game game;
    game.run();
    return 0;
}
