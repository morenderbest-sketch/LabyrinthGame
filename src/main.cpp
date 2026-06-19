#include "Game.h"

int main() {
    // Force UTF-8 output on Windows if needed
    std::system("chcp 65001");

    Game game;
    game.run();
    return 0;
}
