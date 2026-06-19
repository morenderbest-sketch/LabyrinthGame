#include "Game.h"
#include "Key.h"

// ─── Constructor ─────────────────────────────────────────────────────────────
Game::Game() : player_("Hero"), running_(false), won_(false),
               locationDisplayedOnce_(false) {}

// ─── Utilities ───────────────────────────────────────────────────────────────
std::string Game::readLine() {
    std::string s;
    std::getline(std::cin, s);
    return s;
}

int Game::readInt(int min, int max) {
    int v;
    while (!(std::cin >> v) || v < min || v > max) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Введите число от " << min << " до " << max << ": ";
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    return v;
}

// ─── State sync ──────────────────────────────────────────────────────────────
void Game::syncStateToWorld() {
    for (auto& loc : world_.getLocations()) {
        auto it = state_.locationOpen.find(loc->getId());
        if (it != state_.locationOpen.end() && it->second)
            loc->forceOpen();
        for (auto& chest : loc->getChests()) {
            auto cit = state_.chestOpened.find(chest->getId());
            if (cit != state_.chestOpened.end() && cit->second)
                chest->open();
        }
        for (auto& npc : loc->getNPCs()) {
            auto nit = state_.npcStates.find(npc->getId());
            if (nit != state_.npcStates.end()) {
                auto& ns = nit->second;
                npc->setAlive(ns.alive);
                npc->setHp(ns.hp);
                if (ns.hostile) npc->setHostile();
            }
        }
    }
}

void Game::syncWorldToState() {
    for (auto& loc : world_.getLocations()) {
        state_.locationOpen[loc->getId()] = loc->isOpen();
        for (auto& chest : loc->getChests())
            state_.chestOpened[chest->getId()] = chest->isOpened();
        for (auto& npc : loc->getNPCs()) {
            GameState::NpcSave ns;
            ns.alive   = npc->isAlive();
            ns.hp      = npc->getHp();
            ns.hostile = (npc->getState() == NPCState::HOSTILE);
            state_.npcStates[npc->getId()] = ns;
        }
    }
}

// ─── Title ───────────────────────────────────────────────────────────────────
void Game::showTitle() {
    std::cout << R"(
+----------------------------------------------------------+
|                                                          |
|   Д О Б Р О П О Ж А Л О В А Т Ь   В                      |
|                                                          |
|      П О Д З Е М Н Ы Й   Л А Б И Р И Н Т                 |
|                                                          |
|  Ваша цель: найти выход из тёмных катакомб.              |
|  Ищите ключи, решайте загадки, доверяйте инстинктам.     |
|                                                          |
+----------------------------------------------------------+
)";
}

// ─── Init/Load ───────────────────────────────────────────────────────────────
void Game::initOrLoad() {
    if (SaveSystem::hasSave()) {
        std::cout << "Найдено сохранение! Загрузить? (1 - да, 2 - начать заново): ";
        int c = readInt(1, 2);
        if (c == 1) {
            if (SaveSystem::load(player_, state_)) {
                syncStateToWorld();
                int locId = player_.getCurrentLocationId();
                if (state_.playerRow >= 0 && state_.playerCol >= 0)
                    playerPos_[locId] = {state_.playerRow, state_.playerCol};
                Logger::instance().log("Game loaded. Player: " + player_.getName());
                std::cout << "Игра загружена!\n\n";
                locationDisplayedOnce_ = true;
                return;
            }
            std::cout << "Ошибка загрузки. Начинаем новую игру.\n\n";
        }
    }
    std::cout << "Введите имя персонажа: ";
    std::string name = readLine();
    if (name.empty()) name = "Безымянный";
    player_ = Player(name);
    for (auto& loc : world_.getLocations())
        state_.locationOpen[loc->getId()] = loc->isOpen();
    Logger::instance().log("New game started. Player: " + name);
    locationDisplayedOnce_ = false;
}

void Game::run() {
    Logger::instance().init("game_log.txt");
    showTitle();
    initOrLoad();
    running_ = true;
    mainLoop();
}

void Game::mainLoop() {
    while (running_ && player_.isAlive() && !won_) {
        handleMovement();
    }
    if (won_) {
        std::cout << R"(
+----------------------------------------------+
|                                              |
|  П О Б Е Д А !  В Ы  В Ы Р В А Л И С Ь !     |
|                                              |
|  Солнечный свет касается вашего лица.        |
|  Лабиринт позади. Вы свободны!               |
|                                              |
+----------------------------------------------+
)";
        Logger::instance().log("VICTORY!");
    } else if (!player_.isAlive()) {
        std::cout << "\nВы погибли. Лабиринт забрал ещё одну душу...\n";
        Logger::instance().log("Player died.");
    }
}

// ─── Current position ────────────────────────────────────────────────────────
std::pair<int,int>& Game::currentPos() {
    int locId = player_.getCurrentLocationId();
    if (playerPos_.find(locId) == playerPos_.end()) {
        Location* loc = world_.getLocation(locId);
        int r = 1, c = 1;
        if (loc) loc->getSpawn(r, c);
        playerPos_[locId] = {r, c};
    }
    return playerPos_[locId];
}

// ─── Movement ────────────────────────────────────────────────────────────────
void Game::handleMovement() {
    Location* loc = world_.getLocation(player_.getCurrentLocationId());
    if (!loc) return;

    auto& pos = currentPos();
    int& row = pos.first;
    int& col = pos.second;

    if (!locationDisplayedOnce_) {
        loc->displayInfo();
        locationDisplayedOnce_ = true;
    }

    loc->displayMap(row, col);

    // Status bar
    std::cout << " HP: " << player_.getHp() << "/" << player_.getMaxHp()
              << "  Урон: " << player_.getTotalDamage();
    auto eq = player_.getInventory().getEquipped();
    if (eq) std::cout << " [" << eq->getName() << "]";
    // Show keys in inventory briefly
    auto keys = player_.getInventory().getKeys();
    if (!keys.empty()) {
        std::cout << "  Ключи: ";
        for (auto& k : keys) std::cout << "[" << k->getName() << "] ";
    }
    std::cout << "\n";
    std::cout << " w/a/s/d — движение | e — взаимодействие | i — инвентарь | m — сохранить | 0 — выход\n> ";

    std::string input;
    std::getline(std::cin, input);
    if (input.empty()) return;
    for (auto& ch : input) ch = std::tolower((unsigned char)ch);

    if      (input == "w" || input == "up")    { doMove(loc, row, col, -1,  0); }
    else if (input == "s" || input == "down")  { doMove(loc, row, col,  1,  0); }
    else if (input == "a" || input == "left")  { doMove(loc, row, col,  0, -1); }
    else if (input == "d" || input == "right") { doMove(loc, row, col,  0,  1); }
    else if (input == "e") { doInteract(loc, row, col); }
    else if (input == "i") { actionInventory(); }
    else if (input == "m") { actionSave(); }
    else if (input == "0") { running_ = false; }
    else { std::cout << " Неизвестная команда.\n"; }
}

void Game::doMove(Location* loc, int& row, int& col, int dr, int dc) {
    int nr, nc;
    if (loc->tryMove(row, col, dr, dc, nr, nc)) {
        row = nr; col = nc;
    } else {
        std::cout << " Здесь стена.\n";
    }
}

// ─── Interaction scan ────────────────────────────────────────────────────────
void Game::doInteract(Location* loc, int row, int col) {
    struct AdjItem { int dr, dc; std::string dirName; CellType type; int id; };
    std::vector<AdjItem> found;

    // Check current cell first (player standing on D or C)
    {
        int id = -1;
        auto* door = loc->getDoorAt(row, col);
        if (door) { id = door->targetLocationId; found.push_back({0,0,"здесь",CellType::DOOR,id}); }
        auto* chest = loc->getChestAt(row, col);
        if (chest) { found.push_back({0,0,"здесь",CellType::CHEST,chest->getId()}); }
    }

    const int drs[4] = {-1, 1, 0,  0};
    const int dcs[4] = { 0, 0,-1,  1};
    const char* names[4] = {"вверху","внизу","слева","справа"};

    for (int d = 0; d < 4; ++d) {
        int id = -1;
        CellType type = loc->checkAdjacent(row, col, drs[d], dcs[d], id);
        if (type == CellType::CHEST || type == CellType::NPC || type == CellType::DOOR)
            found.push_back({drs[d], dcs[d], names[d], type, id});
    }

    if (found.empty()) { std::cout << " Рядом ничего нет.\n"; return; }

    if (found.size() == 1) {
        interactWith(loc, row, col, found[0].dr, found[0].dc, found[0].type, found[0].id);
        return;
    }

    std::cout << "\n Рядом с вами:\n";
    for (int i = 0; i < (int)found.size(); ++i) {
        std::string label;
        if (found[i].type == CellType::CHEST) {
            Chest* c = loc->getChestAt(row+found[i].dr, col+found[i].dc);
            label = (c && c->isOpened()) ? "Пустой сундук" : "Сундук [C]";
        } else if (found[i].type == CellType::NPC) {
            NPC* n = loc->getNPCAt(row+found[i].dr, col+found[i].dc);
            label = n ? n->getName() : "NPC";
        } else {
            label = (found[i].id == -1) ? "ВЫХОД [E]!" : "Дверь [D]";
        }
        std::cout << "  " << (i+1) << ") " << found[i].dirName << " -- " << label << "\n";
    }
    std::cout << "  0) Otmena\n> ";

    int choice;
    while (!(std::cin >> choice) || choice < 0 || choice > (int)found.size()) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "0.." << found.size() << ": ";
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    if (choice == 0) return;

    auto& it = found[choice-1];
    interactWith(loc, row, col, it.dr, it.dc, it.type, it.id);
}

void Game::interactWith(Location* loc, int row, int col,
                        int dr, int dc, CellType type, int id) {
    if (type == CellType::CHEST) {
        Chest* chest = loc->getChestAt(row+dr, col+dc);
        if (chest) handleChestInteraction(chest);
    } else if (type == CellType::NPC) {
        NPC* npc = loc->getNPCAt(row+dr, col+dc);
        if (npc) handleNPCInteraction(npc);
    } else if (type == CellType::DOOR) {
        handleDoorInteraction(id);
    }
}

// ─── Chest ───────────────────────────────────────────────────────────────────
void Game::handleChestInteraction(Chest* chest) {
    if (chest->isOpened()) { std::cout << " Сундук уже пуст.\n"; return; }

    std::vector<std::shared_ptr<Item>> items;
    if (chest->isLocked()) {
        std::cout << "\n[СУНДУК ЗАКРЫТ НА ЗАМОК]\n";
        Puzzle* p = chest->getPuzzle();
        if (!p) {
            items = chest->open();
        } else {
            p->display();
            std::string answer;
            if (!p->getOptions().empty()) {
                int c = readInt(1, (int)p->getOptions().size());
                answer = std::to_string(c);
            } else {
                answer = readLine();
            }
            items = chest->tryOpen(answer);
            if (!chest->isOpened()) {
                applyDamage(10, "wrong chest answer");
                std::cout << " Неверно! -10 HP. Осталось: " << player_.getHp() << "\n";
                return;
            }
        }
    } else {
        items = chest->open();
    }
    collectItems(items);
    state_.chestOpened[chest->getId()] = true;
}

// ─── NPC ─────────────────────────────────────────────────────────────────────
void Game::handleNPCInteraction(NPC* npc) {
    if (!npc->isAlive()) { std::cout << " " << npc->getName() << " мёртв.\n"; return; }

    if (npc->getState() == NPCState::HOSTILE) {
        std::cout << "\n[БОЙ] " << npc->getName() << " враждебен!\n";
        bool won = CombatSystem::fight(player_, *npc);
        if (!player_.isAlive()) {
            running_ = false;
        } else if (won) {
            // Drop loot from defeated NPC
            auto loot = npc->takeLoot();
            if (!loot.empty()) {
                std::cout << "\n" << npc->getName() << " повержен! Вы обыскиваете тело:\n";
                collectItems(loot);
            }
        }
    } else {
        // Special case: Mortimer (id=4) requires Manuscript (id=301) before giving key
        if (npc->getId() == 4) {
            auto manuscript = player_.getInventory().findKeyFor(0); // targetLoc=0 = quest item
            if (!manuscript) {
                std::cout << "\n" << npc->getName()
                          << ": \"Принеси мне Древний Манускрипт из сундука в этом зале.\n"
                          << " Подсказка к замку: слово 'анером' — прочти справа налево.\"\n";
                return;
            }
            // Has manuscript — remove it, run dialog from node 3 directly
            std::cout << "\n" << npc->getName()
                      << ": \"O! Вы нашли его! Вот ваш ключ от Зала Духов.\"\n";
            player_.getInventory().consumeKey(0); // remove manuscript
            auto keyItem = npc->takeItem(202);
            if (keyItem) {
                player_.getInventory().addItem(keyItem);
                std::cout << "[Получено: " << keyItem->describe() << "]\n";
                Logger::instance().log("Manuscript exchanged for key to loc4.");
            }
            state_.npcStates[npc->getId()] = {npc->isAlive(), npc->getHp(), false};
            return;
        }

        DialogOutcome outcome = npc->runDialog();
        Logger::instance().log("Dialog with " + npc->getName() + " completed.");

        if (outcome.makeHostile) {
            npc->setHostile();
            std::cout << " " << npc->getName() << " стал враждебным! Бой!\n";
            CombatSystem::fight(player_, *npc);
            if (!player_.isAlive()) running_ = false;
        }
        if (outcome.giveItemId != -1) {
            auto item = npc->takeItem(outcome.giveItemId);
            if (item) {
                player_.getInventory().addItem(item);
                std::cout << "[+] Получено: " << item->describe() << "\n";
                Logger::instance().log("Item received: " + item->getName());
            }
        }
        if (outcome.openLocationId != -1) {
            Location* target = world_.getLocation(outcome.openLocationId);
            if (target) {
                target->forceOpen();
                state_.locationOpen[target->getId()] = true;
                std::cout << "[ОТКРЫТО] Локация «" << target->getName() << "» открыта!\n";
                Logger::instance().log("Location " + std::to_string(target->getId()) + " opened by NPC.");
            }
        }
        if (outcome.healPlayer > 0) {
            player_.heal(outcome.healPlayer);
            std::cout << "[+HP] Восстановлено " << outcome.healPlayer << " HP!\n";
        }
    }
    state_.npcStates[npc->getId()] = {
        npc->isAlive(), npc->getHp(), npc->getState() == NPCState::HOSTILE
    };
}

// ─── Door ────────────────────────────────────────────────────────────────────
void Game::handleDoorInteraction(int targetLocId) {
    // EXIT
    if (targetLocId == -1) {
        std::cout << "\n★★★ ВЫ ВИДИТЕ ВЫХОД — СВЕТ, ВОЗДУХ, СВОБОДА! ★★★\n";
        won_ = true; running_ = false;
        return;
    }

    Location* target = world_.getLocation(targetLocId);
    if (!target) return;

    // Already open — just enter
    if (target->isOpen()) {
        enterLocation(target);
        return;
    }

    // Check if location requires a key
    int keyId = target->getKeyItemId();
    if (keyId != -1) {
        // Find key for this location in inventory
        auto key = player_.getInventory().findKeyFor(targetLocId);
        if (key) {
            std::cout << "\nВы используете [" << key->getName() << "]. Дверь открывается!\n";
            player_.getInventory().consumeKey(targetLocId);
            target->forceOpen();
            state_.locationOpen[targetLocId] = true;
            Logger::instance().log("Location " + std::to_string(targetLocId)
                                   + " opened with key.");
            enterLocation(target);
        } else {
            // No key — show hint
            std::cout << "\n[ДВЕРЬ ЗАКРЫТА НА КЛЮЧ]\n";
            if (!target->getHint().empty())
                std::cout << " " << target->getHint() << "\n";
            else
                std::cout << " Вам нужен ключ, чтобы пройти дальше.\n";
        }
        return;
    }

    // Has puzzle
    if (target->getPuzzle()) {
        tryOpenLocation(target);
        return;
    }

    // No lock at all
    target->forceOpen();
    state_.locationOpen[targetLocId] = true;
    enterLocation(target);
}

void Game::enterLocation(Location* target) {
    std::cout << "\n→ Вы переходите в: " << target->getName() << "\n";
    player_.setLocationId(target->getId());
    locationDisplayedOnce_ = false;
    Logger::instance().log("Player entered loc " + std::to_string(target->getId()));
    if (target->isFinal()) { won_ = true; running_ = false; }
}

// ─── Puzzle unlock ───────────────────────────────────────────────────────────
void Game::tryOpenLocation(Location* target) {
    Puzzle* puzzle = target->getPuzzle();
    if (!puzzle) { target->forceOpen(); return; }

    std::cout << "\n[ ГОЛОВОЛОМКА ]\n";
    puzzle->display();

    std::string answer;
    if (!puzzle->getOptions().empty()) {
        int c = readInt(1, (int)puzzle->getOptions().size());
        answer = std::to_string(c);
    } else {
        answer = readLine();
    }

    if (target->tryUnlock(answer)) {
        std::cout << "\n✓ Верно! Дверь открывается!\n";
        state_.locationOpen[target->getId()] = true;
        Logger::instance().log("Location " + std::to_string(target->getId()) + " unlocked.");
        enterLocation(target);
    } else {
        int dmg = target->getPuzzleDamage();
        applyDamage(dmg, "wrong puzzle");
        std::cout << "✗ Неверно! -" << dmg << " HP. Осталось: " << player_.getHp() << "\n";
    }
}

// ─── Inventory ───────────────────────────────────────────────────────────────
void Game::actionInventory() {
    while (true) {
        player_.getInventory().display();
        std::cout << "\n 1) Выпить зелье  2) Экипировать оружие  3) Снять оружие  0) Назад\n> ";
        std::string line = readLine();
        if (line.empty()) continue;
        int c = 0;
        try { c = std::stoi(line); } catch(...) { continue; }
        if (c == 0) return;
        if (c == 1) {
            auto potions = player_.getInventory().getPotions();
            if (potions.empty()) { std::cout << " Нет зелий.\n"; return; }
            std::cout << " Номер зелья (1-" << potions.size() << "): ";
            std::string p = readLine();
            int pi = 0;
            try { pi = std::stoi(p); } catch(...) { continue; }
            if (pi < 1 || pi > (int)potions.size()) continue;
            player_.usePotionFromInventory(pi - 1);
            Logger::instance().log("Player used potion.");
            return;
        } else if (c == 2) {
            auto weapons = player_.getInventory().getWeapons();
            if (weapons.empty()) { std::cout << " Нет оружия.\n"; return; }
            for (int i = 0; i < (int)weapons.size(); ++i)
                std::cout << "  " << (i+1) << ") " << weapons[i]->describe() << "\n";
            std::cout << " Номер (1-" << weapons.size() << "): ";
            std::string w = readLine();
            int wi = 0;
            try { wi = std::stoi(w); } catch(...) { continue; }
            if (wi < 1 || wi > (int)weapons.size()) continue;
            player_.equipWeapon(weapons[wi-1]->getId());
            return;
        } else if (c == 3) {
            player_.unequipWeapon();
            return;
        }
    }
}

// ─── Save ────────────────────────────────────────────────────────────────────
void Game::actionSave() {
    syncWorldToState();
    auto& pos = currentPos();
    state_.playerRow = pos.first;
    state_.playerCol = pos.second;
    if (SaveSystem::save(player_, state_)) {
        std::cout << "✓ Игра сохранена!\n";
        Logger::instance().log("Game saved.");
    } else {
        std::cout << "✗ Ошибка сохранения!\n";
    }
}

// ─── Helpers ─────────────────────────────────────────────────────────────────
void Game::collectItems(const std::vector<std::shared_ptr<Item>>& items) {
    if (items.empty()) return;
    std::cout << "\n[ ВЫ ПОЛУЧАЕТЕ: ]\n";
    for (auto& item : items) {
        std::cout << "  + " << item->describe() << "\n";
        player_.getInventory().addItem(item);
        Logger::instance().log("Item picked up: " + item->getName());
    }
}

void Game::applyDamage(int amount, const std::string& reason) {
    player_.takeDamage(amount);
    Logger::instance().log("Player took " + std::to_string(amount) + " dmg: " + reason);
}
