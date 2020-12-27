#include <iostream>
#include <locale>
#include <string>
#include <vector>
using namespace std;

string rune_names[4] = { "Rune of air ",      "Rune of earth ", "Rune of fire ", "Rune of water " };
struct Coin { unsigned int count; };
enum class Element { FIRE, WATER, EARTH, AIR };
struct Rune { unsigned int level;  Element element; };
struct Weapon { unsigned int damage; unsigned int critical;   unsigned int durability; };
struct Armor { unsigned int guard;  unsigned int durability; };

enum class ItemType { COIN, RUNE, WEAPON, ARMOR };
struct Item {
    ItemType itemtype;
    union Drop { Coin coin; Rune rune; Weapon weapon; Armor armor; } drop;
};

typedef vector<Item> LootBox;
struct chance_probability { Item item; float chance; };
struct chance_probability ch[] = {
    { { ItemType::COIN,   {.coin = {  1    }               } }, 50.00 },
    { { ItemType::RUNE,   {.rune = {  1,   Element::FIRE}  } },  6.00 },
    { { ItemType::RUNE,   {.rune = {  1,   Element::WATER} } }, 13.00 },
    { { ItemType::RUNE,   {.rune = {  1,   Element::EARTH} } },  7.00 },
    { { ItemType::RUNE,   {.rune = {  1,   Element::AIR}   } }, 14.00 },
    { { ItemType::RUNE,   {.rune = {  5,   Element::FIRE}  } },  0.60 },
    { { ItemType::RUNE,   {.rune = {  5,   Element::WATER} } },  1.30 },
    { { ItemType::RUNE,   {.rune = {  5,   Element::EARTH} } },  0.70 },
    { { ItemType::RUNE,   {.rune = {  5,   Element::AIR}   } },  1.40 },
    { { ItemType::RUNE,   {.rune = { 10,   Element::FIRE}  } },  0.06 },
    { { ItemType::RUNE,   {.rune = { 10,   Element::WATER} } },  0.13 },
    { { ItemType::RUNE,   {.rune = { 10,   Element::EARTH} } },  0.07 },
    { { ItemType::RUNE,   {.rune = { 10,   Element::AIR}   } },  0.14 },
    { { ItemType::WEAPON, {.weapon = {100,   0, 100}         } },  1.40 },
    { { ItemType::WEAPON, {.weapon = { 75,  50, 100}         } },  1.40 },
    { { ItemType::ARMOR,  {.armor = { 50, 100}              } },  2.80 }
}; unsigned int const ch_size = sizeof(ch) / sizeof(chance_probability);

ostream& operator<< (ostream& out, Item it) {
    switch (it.itemtype) {
    case ItemType::COIN:   out << it.drop.coin.count << " gold\n"; break;
    case ItemType::RUNE:   out << rune_names[(int)it.drop.rune.element] << it.drop.rune.level << " lvl\n"; break;
    case ItemType::WEAPON: out << ((!it.drop.weapon.critical) ? "God Killer" : "Demon Slayer") << "\n"; break;
    case ItemType::ARMOR:  out << "Bronezhiletka" << "\n"; break;
    } return out;
}
Item operator++ (Item& it) {
    float chance = (float)(rand() % 10000) / 100, sum = 0;
    short i;
    for (i = 0; i < ch_size; i++) {
        sum += ch[i].chance;
        if (chance <= sum) break;
    }
    it = ch[i].item;
    if (it.itemtype == ItemType::COIN) it.drop.coin.count = 1 + (rand() % (512));
    return it;
}
LootBox& operator<<(LootBox& lb, Item it) {
    lb.push_back(it);
    return lb;
}
ostream& operator<<(ostream& out, LootBox lb) {
    for (int i = 0; i < lb.size(); i++) out << lb[i];
    return out;
}
LootBox generateLootBox() {
    LootBox LB; Item it;
    for (short i = 0; i < 3; i++) LB << ++it;
    return LB;
}

int main() {
    srand(time(NULL));
    setlocale(LC_ALL, "Russian");
    string res;
    while (true) {
        cout << "Открыть лутбокс? Yes/No\n";
        cin >> res; for (auto& c : res) c = tolower(c);

        if (res == "y" || res == "yes")     cout << generateLootBox();
        else if (res == "n" || res == "no") break;

        cin.clear(); cin.ignore(32767, '\n');
    }
    return 0;
}