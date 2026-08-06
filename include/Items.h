#ifndef ITEMS_H
#define ITEMS_H

#include <Arduino.h>
#include "Config.h"

enum ItemType {
    ITEM_FOOD = 0,
    ITEM_MEDICINE,
    ITEM_TOY,
    ITEM_DECOR
};

struct Item {
    int id;
    char name[20];
    ItemType type;
    int price;
    int hungerValue;
    int happyValue;
    int healthValue;
    int weightValue;
    char description[40];

    Item() : id(0), type(ITEM_FOOD), price(0), hungerValue(0), happyValue(0), healthValue(0), weightValue(0) {
        name[0] = '\0';
        description[0] = '\0';
    }

    Item(int id_, const char* name_, ItemType type_, int price_, int hunger_, int happy_, int health_, int weight_, const char* desc_)
        : id(id_), type(type_), price(price_), hungerValue(hunger_), happyValue(happy_), healthValue(health_), weightValue(weight_) {
        strncpy(name, name_, sizeof(name) - 1);
        name[sizeof(name) - 1] = '\0';
        strncpy(description, desc_, sizeof(description) - 1);
        description[sizeof(description) - 1] = '\0';
    }
};

#define ITEM_CATALOG_SIZE 12

class ItemsManager {
private:
    Item catalog[ITEM_CATALOG_SIZE];
    int inventory[ITEM_CATALOG_SIZE]; // Quantidade de cada item possuído

public:
    ItemsManager();

    void initCatalog();
    const Item& getItem(int index) const { return catalog[index]; }
    int getCatalogSize() const { return ITEM_CATALOG_SIZE; }

    int getQuantity(int itemId) const;
    bool buyItem(int itemId, int& playerCoins);
    bool useItem(int itemId, class Pet& pet);
    void addQuantity(int itemId, int qty);

    void saveInventory(int* outInv);
    void loadInventory(const int* inInv);
};

#endif // ITEMS_H
