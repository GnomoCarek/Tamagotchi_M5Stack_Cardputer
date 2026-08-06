#ifndef ACHIEVEMENTS_H
#define ACHIEVEMENTS_H

#include <Arduino.h>
#include "Config.h"

struct Achievement {
    int id;
    char title[22];
    char description[32];
    bool unlocked;
    int rewardCoins;

    Achievement() : id(0), unlocked(false), rewardCoins(0) {
        title[0] = '\0';
        description[0] = '\0';
    }

    Achievement(int id_, const char* title_, const char* desc_, bool unlocked_, int reward_)
        : id(id_), unlocked(unlocked_), rewardCoins(reward_) {
        strncpy(title, title_, sizeof(title) - 1);
        title[sizeof(title) - 1] = '\0';
        strncpy(description, desc_, sizeof(description) - 1);
        description[sizeof(description) - 1] = '\0';
    }
};

class AchievementsManager {
private:
    Achievement list[MAX_ACHIEVEMENTS];

public:
    AchievementsManager();

    void initList();
    void checkProgress(const class Pet& pet, int starScore);
    bool unlock(int id, class Pet& pet);
    
    const Achievement& getAchievement(int index) const { return list[index]; }
    int getCount() const { return MAX_ACHIEVEMENTS; }
    
    void saveState(bool* outState);
    void loadState(const bool* inState);
};

#endif // ACHIEVEMENTS_H
