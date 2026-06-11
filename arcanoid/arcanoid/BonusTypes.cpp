#include "BonusTypes.h"
#include "Game.h"

void FloorBonus::applyEffect(Game& game) { game.activateSafetyFloor(); }
void ShrinkBonus::applyEffect(Game& game) { game.activateShrinkCarriage(); }
void StickyBonus::applyEffect(Game& game) { game.activateStickyCarriage(); }
void SecondBallBonus::applyEffect(Game& game) { game.spawnSecondBall(); }