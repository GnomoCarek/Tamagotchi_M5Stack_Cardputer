#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// ==========================================
// CONFIGURAÇÕES DE TELA & HARDWARE
// ==========================================
#define SCREEN_WIDTH  240
#define SCREEN_HEIGHT 135
#define FPS           30
#define FRAME_MS      (1000 / FPS)

// ==========================================
// PALETA DE CORES (RGB565)
// ==========================================
#define COLOR_BG_DAY        0x4DD5 // Azul suave diurno
#define COLOR_BG_NIGHT      0x10AB // Azul escuro noturno
#define COLOR_BG_DAWN       0xE40D // Tom rosado/alvorecer
#define COLOR_BG_DUSK       0x7B90 // Tom arroxeado/crepúsculo
#define COLOR_TEXT_DARK     0x18C3 // Cinza escuro para texto
#define COLOR_TEXT_LIGHT    0xFFFF // Branco
#define COLOR_HEADER_BG     0x2965 // Azul escuro para cabeçalho
#define COLOR_PANEL_BG      0x18E7 // Painel transparente/escuro
#define COLOR_BAR_BG        0x39E7 // Fundo das barras de status
#define COLOR_HEALTH        0xE105 // Vermelho/Rosa saúde
#define COLOR_HUNGER        0xFD20 // Laranja fome
#define COLOR_HAPPY         0xFFE0 // Amarelo felicidade
#define COLOR_ENERGY        0x07E0 // Verde energia
#define COLOR_POOP          0x8A00 // Marrom cocô
#define COLOR_COIN          0xFEA0 // Dourado moedas
#define COLOR_ACCENT        0x3E1F // Roxo moderno de acento

// ==========================================
// TAMAGOTCHI CONSTANTES
// ==========================================
#define STAT_MAX            100
#define STAT_MIN            0
#define DEFAULT_HUNGER      80
#define DEFAULT_HAPPY       80
#define DEFAULT_ENERGY      90
#define DEFAULT_HEALTH      100
#define DEFAULT_WEIGHT      10

#define MAX_POOPS           4
#define MAX_ITEMS           20
#define MAX_ACHIEVEMENTS    10

// Personalidades do Tamagotchi
enum PersonalityType {
    PERSONALITY_ALEGRE = 0,
    PERSONALITY_BRAVO,
    PERSONALITY_DORMINHOCO,
    PERSONALITY_GULOSO,
    PERSONALITY_CURIOSO,
    PERSONALITY_CALMO,
    PERSONALITY_COUNT
};

// Sexo do Pet
enum PetGender {
    GENDER_MALE = 0,
    GENDER_FEMALE
};

// Estágios de Evolução
enum EvolutionStage {
    STAGE_EGG = 0,
    STAGE_BABY,
    STAGE_CHILD,
    STAGE_ADULT,
    STAGE_SENIOR,
    STAGE_COUNT
};

// Variações / Raças de Pet (Unlockable Pets)
enum PetVariant {
    PET_VARIANT_CLASSIC = 0,
    PET_VARIANT_DRAGON,
    PET_VARIANT_CAT,
    PET_VARIANT_COUNT
};

// Estados do Jogo (State Machine)
enum GameState {
    STATE_TITLE = 0,
    STATE_INITIAL_MENU,
    STATE_GENDER_ENTRY,
    STATE_NAME_ENTRY,
    STATE_HATCHING,
    STATE_GAMEPLAY,
    STATE_MENU,
    STATE_SHOP,
    STATE_INVENTORY,
    STATE_STATS,
    STATE_SETTINGS,
    STATE_WIFI_CONFIG,
    STATE_MINIGAME_SELECT,
    STATE_MINIGAME_CATCH,
    STATE_MINIGAME_REFLEX,
    STATE_EVOLUTION,
    STATE_GAME_OVER,
    STATE_ACHIEVEMENTS
};

// Tipos de Efeitos Sonoros
enum SoundType {
    SOUND_CLICK = 0,
    SOUND_FEED,
    SOUND_PLAY,
    SOUND_SLEEP,
    SOUND_SICK,
    SOUND_POOP,
    SOUND_EVOLUTION,
    SOUND_DEATH,
    SOUND_BOOT,
    SOUND_COIN,
    SOUND_ACHIEVEMENT,
    SOUND_CLEAN,
    SOUND_EGG_HATCH,
    SOUND_SCREEN_SLEEP,
    SOUND_SCREEN_WAKE,
    SOUND_LOW_BATTERY,
    SOUND_TYPING,
    SOUND_CORRECT,
    SOUND_WRONG,
    SOUND_WIFI_OK
};

// Modos de Desenvolvimento
#define DEV_MODE_DAY_DURATION_SEC 1800.0f // 1 dia virtual = 30 minutos reais (1800 segundos)

// Idiomas (Suporte extensível, padrão PT-BR)
enum Language {
    LANG_PT_BR = 0,
    LANG_EN
};

#endif // CONFIG_H
