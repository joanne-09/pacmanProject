// Functions without 'static', 'extern' prefixes is just a normal
// function, they can be accessed by other files using 'extern'.
// Define your normal function prototypes below.

#ifndef SCENE_GAME_H
#define SCENE_GAME_H
#include "game.h"

bool allowCheat;
bool mute;
bool changeMusic;
//0:WASD 1:arrow
int keyset;
int key[4];
//0:yellow 1:blue 2:christmas
int character;

Scene scene_main_create(void);

int64_t get_power_up_timer_tick();
int64_t get_power_up_duration();
int64_t get_game_timer();

#endif
