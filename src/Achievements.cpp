#include "Achievements.h"
#include "Pet.h"

AchievementsManager::AchievementsManager() {
    initList();
}

void AchievementsManager::initList() {
    list[0] = Achievement(0, "Primeira Refeicao", "Alimente seu pet 1 vez", false, 10);
    list[1] = Achievement(1, "Gourmet",           "Alimente seu pet 15 vezes", false, 25);
    list[2] = Achievement(2, "Brincalhao",        "Brinque 10 vezes", false, 20);
    list[3] = Achievement(3, "Estrela Cadente",   "Pegue 20 estrelas", false, 30);
    list[4] = Achievement(4, "Limpinho",          "Limpe o pet 10 vezes", false, 15);
    list[5] = Achievement(5, "Doutor",            "Curou o pet de doencas", false, 25);
    list[6] = Achievement(6, "Vida Longa",        "Alcançou 3 dias de vida", false, 50);
    list[7] = Achievement(7, "Magnata",           "Acumulou 150 moedas", false, 40);
    list[8] = Achievement(8, "Crescimento",       "Evoluiu para Adulto", false, 50);
    list[9] = Achievement(9, "Mestre Tama",       "Cuidou com amor e saude", false, 100);
}

void AchievementsManager::checkProgress(const Pet& pet, int starScore) {
    if (pet.mealsEaten >= 1) unlock(0, (Pet&)pet);
    if (pet.mealsEaten >= 15) unlock(1, (Pet&)pet);
    if (pet.gamesPlayed >= 10) unlock(2, (Pet&)pet);
    if (starScore >= 20) unlock(3, (Pet&)pet);
    if (pet.bathsTaken >= 10) unlock(4, (Pet&)pet);
    if (pet.medicinesGiven >= 1) unlock(5, (Pet&)pet);
    if (pet.ageDays >= 3) unlock(6, (Pet&)pet);
    if (pet.coins >= 150) unlock(7, (Pet&)pet);
    if (pet.stage >= STAGE_ADULT) unlock(8, (Pet&)pet);
    if (pet.happiness >= 90 && pet.health >= 90) unlock(9, (Pet&)pet);
}

bool AchievementsManager::unlock(int id, Pet& pet) {
    if (id < 0 || id >= MAX_ACHIEVEMENTS) return false;
    if (!list[id].unlocked) {
        list[id].unlocked = true;
        pet.addCoins(list[id].rewardCoins);
        return true; // Recém desbloqueado!
    }
    return false;
}

void AchievementsManager::saveState(bool* outState) {
    for (int i = 0; i < MAX_ACHIEVEMENTS; i++) {
        outState[i] = list[i].unlocked;
    }
}

void AchievementsManager::loadState(const bool* inState) {
    for (int i = 0; i < MAX_ACHIEVEMENTS; i++) {
        list[i].unlocked = inState[i];
    }
}
