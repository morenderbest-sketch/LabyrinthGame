#include "CombatSystem.h"
#include "Logger.h"

bool CombatSystem::fight(Player& player, NPC& npc) {
    Logger::instance().log("Combat started: " + player.getName() + " vs " + npc.getName());
    std::cout << "\n⚔️  НАЧИНАЕТСЯ БОЙ: " << player.getName() << " vs " << npc.getName() << "!\n";

    while (player.isAlive() && npc.isAlive()) {
        displayCombatStatus(player, npc);
        std::cout << "\nВаш ход:\n"
                  << "  1) Атаковать\n"
                  << "  2) Выпить зелье\n"
                  << "  3) Убежать\n"
                  << "Выбор: ";
        int choice;
        while (!(std::cin >> choice) || choice < 1 || choice > 3) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Введите 1, 2 или 3: ";
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        if (choice == 1) {
            int dmg = player.getTotalDamage();
            npc.takeDamage(dmg);
            std::cout << "Вы наносите " << dmg << " урона " << npc.getName() << "!\n";
            Logger::instance().log("Player attacks " + npc.getName() + " for " + std::to_string(dmg));
            if (!npc.isAlive()) {
                std::cout << "🎉 " << npc.getName() << " повержен!\n";
                Logger::instance().log(npc.getName() + " defeated.");
                return true;
            }
        } else if (choice == 2) {
            auto potions = player.getInventory().getPotions();
            if (potions.empty()) {
                std::cout << "У вас нет зелий!\n";
                continue;
            }
            std::cout << "Выберите зелье:\n";
            for (size_t i = 0; i < potions.size(); ++i)
                std::cout << "  " << (i+1) << ") " << potions[i]->describe() << "\n";
            std::cout << "Выбор: ";
            int pi;
            while (!(std::cin >> pi) || pi < 1 || pi > (int)potions.size()) {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cout << "Введите число: ";
            }
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            player.usePotionFromInventory(pi - 1);
        } else {
            std::cout << "Вы спасаетесь бегством!\n";
            Logger::instance().log("Player fled from " + npc.getName());
            return false;
        }

        // NPC attacks back
        if (npc.isAlive()) {
            int ndmg = npc.getDamage();
            player.takeDamage(ndmg);
            std::cout << npc.getName() << " атакует вас на " << ndmg << " урона!\n";
            Logger::instance().log(npc.getName() + " attacks player for " + std::to_string(ndmg));
            if (!player.isAlive()) {
                std::cout << "💀 Вы погибли в бою...\n";
                Logger::instance().log("Player died in combat.");
                return false;
            }
        }
    }
    return !npc.isAlive();
}

void CombatSystem::displayCombatStatus(const Player& player, const NPC& npc) {
    std::cout << "\n--- БОЙ ---\n";
    std::cout << player.getName() << " HP: " << player.getHp()
              << " | Урон: " << player.getTotalDamage() << "\n";
    npc.displayStatus();
    std::cout << "-----------\n";
}
