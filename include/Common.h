#pragma once
#include <string>
#include <vector>
#include <memory>
#include <map>
#include <functional>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <sstream>
#include <ctime>
#include <iomanip>
#include <limits>

class Location;
class Item;
class Potion;
class Weapon;
class Key;
class Chest;
class NPC;
class Player;

enum class ItemType { POTION, WEAPON, KEY };
enum class NPCState { FRIENDLY, HOSTILE };
enum class LocationState { OPEN, CLOSED };
enum class PuzzleType { MATH, RIDDLE, SEQUENCE, WORD, CHOICE };
enum class CellType { FLOOR, WALL, CHEST, NPC, DOOR, EXIT, EMPTY };
