#include "Items.h"
#include "Pet.h"

ItemsManager::ItemsManager() {
    for (int i = 0; i < ITEM_CATALOG_SIZE; i++) {
        inventory[i] = 0;
    }
    initCatalog();
    // Quantidades iniciais grátis
    inventory[0] = 3; // 3 Maçãs
    inventory[4] = 1; // 1 Vitamina
}

void ItemsManager::initCatalog() {
    // Comidas
    catalog[0] = Item(0, "Maca",       ITEM_FOOD, 5,  15, 2,  0,  0, "+15 Fome");
    catalog[1] = Item(1, "Hamburguer", ITEM_FOOD, 15, 40, 5,  0,  2, "+40 Fome, +2kg");
    catalog[2] = Item(2, "Bolo",       ITEM_FOOD, 20, 25, 25, 0,  3, "+25 Fome, +25 Feliz");
    catalog[3] = Item(3, "Salada",     ITEM_FOOD, 10, 20, 5,  5, -1, "+20 Fome, -1kg");

    // Remédios
    catalog[4] = Item(4, "Vitamina",   ITEM_MEDICINE, 15, 0, 0, 25, 0, "+25 Saude");
    catalog[5] = Item(5, "Xarope",     ITEM_MEDICINE, 25, 0, 0, 50, 0, "Cura doencas leves");
    catalog[6] = Item(6, "Injecao",    ITEM_MEDICINE, 40, 0, 0, 100, 0, "Cura total e saude");

    // Brinquedos
    catalog[7] = Item(7, "Bola",       ITEM_TOY, 30, 0, 30, 0, -1, "+30 Feliz, -1kg");
    catalog[8] = Item(8, "GameBoy",    ITEM_TOY, 60, 0, 60, 0, -2, "+60 Feliz, -2kg");

    // Decorações
    catalog[9]  = Item(9,  "Planta",   ITEM_DECOR, 40, 0, 10, 0, 0, "Decora o quarto");
    catalog[10] = Item(10, "Tapete",   ITEM_DECOR, 50, 0, 15, 0, 0, "Deixa o chao bonito");
    catalog[11] = Item(11, "Quadro",   ITEM_DECOR, 70, 0, 25, 0, 0, "Quadro na parede");
}

int ItemsManager::getQuantity(int itemId) const {
    if (itemId >= 0 && itemId < ITEM_CATALOG_SIZE) {
        return inventory[itemId];
    }
    return 0;
}

bool ItemsManager::buyItem(int itemId, int& playerCoins) {
    if (itemId < 0 || itemId >= ITEM_CATALOG_SIZE) return false;
    if (playerCoins >= catalog[itemId].price) {
        playerCoins -= catalog[itemId].price;
        inventory[itemId]++;
        return true;
    }
    return false;
}

bool ItemsManager::useItem(int itemId, Pet& pet) {
    if (itemId < 0 || itemId >= ITEM_CATALOG_SIZE) return false;
    if (inventory[itemId] <= 0) return false;

    const Item& item = catalog[itemId];

    if (item.type == ITEM_FOOD) {
        if (!pet.feed(item.hungerValue, item.weightValue)) return false;
    } else if (item.type == ITEM_MEDICINE) {
        pet.cure();
        pet.health = min(STAT_MAX, pet.health + item.healthValue);
    } else if (item.type == ITEM_TOY) {
        if (!pet.play(item.happyValue, 0)) return false;
    } else if (item.type == ITEM_DECOR) {
        pet.petCare(); // Aumenta felicidade
    }

    inventory[itemId]--;
    return true;
}

void ItemsManager::addQuantity(int itemId, int qty) {
    if (itemId >= 0 && itemId < ITEM_CATALOG_SIZE) {
        inventory[itemId] += qty;
    }
}

void ItemsManager::saveInventory(int* outInv) {
    for (int i = 0; i < ITEM_CATALOG_SIZE; i++) {
        outInv[i] = inventory[i];
    }
}

void ItemsManager::loadInventory(const int* inInv) {
    for (int i = 0; i < ITEM_CATALOG_SIZE; i++) {
        inventory[i] = inInv[i];
    }
}
