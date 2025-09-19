#ifndef HERO_SELECTION_H
#define HERO_SELECTION_H

#include "game_types.h"

void InitHeroTypes(void);
void StartHeroSelection(void);
void UpdateHeroSelection(void);
void DrawHeroSelection(void);
void SelectHero(int heroType);
HeroType* GetHeroType(int type);

#endif
