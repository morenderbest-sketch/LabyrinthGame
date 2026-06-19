#include "SaveSystem.h"
#include <fstream>

bool SaveSystem::save(const Player& player, const GameState& state,
                      const std::string& filename) {
    std::ofstream f(filename);
    if (!f.is_open()) return false;

    f << "PLAYER:" << player.serialize() << "\n";

    f << "LOCATIONS:";
    bool first = true;
    for (auto& [id, open] : state.locationOpen) {
        if (!first) f << ",";
        f << id << "=" << (open ? 1 : 0);
        first = false;
    }
    f << "\n";

    f << "CHESTS:";
    first = true;
    for (auto& [id, opened] : state.chestOpened) {
        if (!first) f << ",";
        f << id << "=" << (opened ? 1 : 0);
        first = false;
    }
    f << "\n";

    f << "NPCS:";
    first = true;
    for (auto& [id, ns] : state.npcStates) {
        if (!first) f << ",";
        f << id << "=" << (ns.alive ? 1 : 0)
          << ";" << ns.hp
          << ";" << (ns.hostile ? 1 : 0);
        first = false;
    }
    f << "\n";

    f << "POS:" << state.playerRow << "," << state.playerCol << "\n";
    return true;
}

bool SaveSystem::load(Player& player, GameState& state,
                      const std::string& filename) {
    std::ifstream f(filename);
    if (!f.is_open()) return false;

    std::string line;
    while (std::getline(f, line)) {
        if (line.substr(0, 7) == "PLAYER:") {
            player.deserialize(line.substr(7));
        } else if (line.substr(0, 10) == "LOCATIONS:") {
            std::istringstream iss(line.substr(10));
            std::string token;
            while (std::getline(iss, token, ',')) {
                auto eq = token.find('=');
                if (eq == std::string::npos) continue;
                state.locationOpen[std::stoi(token.substr(0, eq))] =
                    token.substr(eq+1) == "1";
            }
        } else if (line.substr(0, 7) == "CHESTS:") {
            std::istringstream iss(line.substr(7));
            std::string token;
            while (std::getline(iss, token, ',')) {
                auto eq = token.find('=');
                if (eq == std::string::npos) continue;
                state.chestOpened[std::stoi(token.substr(0, eq))] =
                    token.substr(eq+1) == "1";
            }
        } else if (line.substr(0, 5) == "NPCS:") {
            std::istringstream iss(line.substr(5));
            std::string token;
            while (std::getline(iss, token, ',')) {
                auto eq = token.find('=');
                if (eq == std::string::npos) continue;
                int id = std::stoi(token.substr(0, eq));
                std::istringstream rs(token.substr(eq+1));
                std::string p;
                std::vector<std::string> ps;
                while (std::getline(rs, p, ';')) ps.push_back(p);
                if (ps.size() < 3) continue;
                GameState::NpcSave ns;
                ns.alive   = ps[0] == "1";
                ns.hp      = std::stoi(ps[1]);
                ns.hostile = ps[2] == "1";
                state.npcStates[id] = ns;
            }
        } else if (line.substr(0, 4) == "POS:") {
            std::istringstream iss(line.substr(4));
            std::string r, c;
            std::getline(iss, r, ',');
            std::getline(iss, c);
            state.playerRow = std::stoi(r);
            state.playerCol = std::stoi(c);
        }
    }
    return true;
}

bool SaveSystem::hasSave(const std::string& filename) {
    std::ifstream f(filename);
    return f.good();
}
