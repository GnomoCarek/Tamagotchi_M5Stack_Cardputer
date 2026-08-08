#include "Items.h"
#include "Pet.h"

ItemsManager::ItemsManager() {
    for (int i = 0; i < ITEM_CATALOG_SIZE; i++) {
        inventory[i] = 0;
    }
    initCatalog();
    // Quantidades iniciais grátis
    inventory[0] = 3; // 3 Maçãs
    inventory[2] = 2; // 2 Pães
    inventory[7] = 1; // 1 Vitamina
    inventory[12] = 1; // 1 Sabão
}

void ItemsManager::initCatalog() {
    // Alimentos (ITEM_FOOD)
    catalog[0] = Item(0, "Maca",       ITEM_FOOD, 5,  15, 2,  0,  0, "+15 Fome");
    catalog[1] = Item(1, "Carne",      ITEM_FOOD, 15, 35, 5,  0,  2, "+35 Fome, +2kg");
    catalog[2] = Item(2, "Pao",        ITEM_FOOD, 10, 25, 3,  0,  1, "+25 Fome, +1kg");
    catalog[3] = Item(3, "Bolo",       ITEM_FOOD, 20, 25, 25, 0,  3, "+25 Fome, +25 Feliz");
    catalog[4] = Item(4, "Leite",      ITEM_FOOD, 8,  18, 8,  5,  1, "+18 Fome, +5 Saude");
    catalog[5] = Item(5, "Hamburguer", ITEM_FOOD, 18, 40, 5,  0,  2, "+40 Fome, +2kg");
    catalog[6] = Item(6, "Salada",     ITEM_FOOD, 12, 20, 5,  5, -1, "+20 Fome, -1kg");

    // Medicamentos (ITEM_MEDICINE)
    catalog[7] = Item(7, "Vitamina",   ITEM_MEDICINE, 15, 0, 0, 25, 0, "+25 Saude");
    catalog[8] = Item(8, "Xarope",     ITEM_MEDICINE, 25, 0, 0, 50, 0, "Cura doencas leves");
    catalog[9] = Item(9, "Injecao",    ITEM_MEDICINE, 40, 0, 0, 100, 0, "Cura total e saude");

    // Brinquedos (ITEM_TOY)
    catalog[10] = Item(10, "Bola",     ITEM_TOY, 30, 0, 30, 0, -1, "+30 Feliz, -1kg");
    catalog[11] = Item(11, "GameBoy",  ITEM_TOY, 60, 0, 60, 0, -2, "+60 Feliz, -2kg");

    // Higiene (ITEM_HYGIENE)
    catalog[12] = Item(12, "Sabao",    ITEM_HYGIENE, 12, 0, 10, 10, 0, "Lava o pet (Banho)");
    catalog[13] = Item(13, "Shampoo",  ITEM_HYGIENE, 22, 0, 20, 15, 0, "Banho super cheiroso");

    // Limpeza (ITEM_CLEANING)
    catalog[14] = Item(14, "Vassoura", ITEM_CLEANING, 10, 0, 5, 0, 0, "Limpa o coco do chao");
    catalog[15] = Item(15, "Esponja",  ITEM_CLEANING, 15, 0, 10, 0, 0, "Limpa ambiente e coco");

    // Itens Especiais / Decorações (ITEM_SPECIAL)
    catalog[16] = Item(16, "Planta",   ITEM_SPECIAL, 40, 0, 10, 0, 0, "Decora o quarto");
    catalog[17] = Item(17, "Tapete",   ITEM_SPECIAL, 50, 0, 15, 0, 0, "Deixa o chao bonito");
    catalog[18] = Item(18, "Quadro",   ITEM_SPECIAL, 70, 0, 25, 0, 0, "Quadro na parede");
}

int ItemsManager::getCategoryItemCount(ItemType type) const {
    int count = 0;
    for (int i = 0; i < ITEM_CATALOG_SIZE; i++) {
        if (catalog[i].type == type) count++;
    }
    return count;
}

int ItemsManager::getCategoryItemId(ItemType type, int indexInCategory) const {
    int current = 0;
    for (int i = 0; i < ITEM_CATALOG_SIZE; i++) {
        if (catalog[i].type == type) {
            if (current == indexInCategory) return catalog[i].id;
            current++;
        }
    }
    return 0;
}

const char* ItemsManager::getCategoryName(ItemType type) const {
    switch (type) {
        case ITEM_FOOD:     return "Alimentos";
        case ITEM_MEDICINE: return "Medicamentos";
        case ITEM_TOY:      return "Brinquedos";
        case ITEM_HYGIENE:  return "Higiene";
        case ITEM_CLEANING: return "Limpeza";
        case ITEM_SPECIAL:  return "Especiais";
        default:            return "Geral";
    }
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
    } else if (item.type == ITEM_HYGIENE) {
        if (!pet.giveBath()) return false;
    } else if (item.type == ITEM_CLEANING) {
        if (!pet.cleanPoop()) return false;
    } else if (item.type == ITEM_SPECIAL) {
        pet.petCare(); // Aumenta felicidade/afeto
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
