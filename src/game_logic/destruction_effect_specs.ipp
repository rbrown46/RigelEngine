/* Copyright (C) 2017, Nikolai Wuttke. All rights reserved.
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

// This file is meant to be included into entity_configuration.ipp. It's only
// a separate file to make the amount of code in one file more manageable.

using EffectMovement = effects::EffectSprite::Movement;


const effects::EffectSpec HOVER_BOT_KILL_EFFECT_SPEC[] = {
  {effects::SpriteCascade{3}, 0},
  {effects::EffectSprite{{0, -2}, 12, EffectMovement::FlyUp}, 0},
  {effects::EffectSprite{{}, 13, EffectMovement::FlyDown}, 0},
  {effects::Particles{{1, 0}, loader::INGAME_PALETTE[3]}, 0},
  {effects::RandomExplosionSound{}, 0}
};


const effects::EffectSpec SIMPLE_TECH_KILL_EFFECT_SPEC[] = {
  {effects::SpriteCascade{3}, 0},
  {effects::Particles{{1, 0}}, 0},
  {effects::RandomExplosionSound{}, 0}
};


const effects::EffectSpec NAPALM_BOMB_KILL_EFFECT_SPEC[] = {
  {effects::RandomExplosionSound{}, 0},
  {effects::Particles{{1, 0}, loader::INGAME_PALETTE[15]}, 0}
};


const effects::EffectSpec SPIDER_KILL_EFFECT_SPEC[] = {
  {effects::EffectSprite{{-1, 1}, 1, EffectMovement::None}, 0},
  {effects::RandomExplosionSound{}, 0}
};


const effects::EffectSpec RED_BIRD_KILL_EFFECT_SPEC[] = {
  {effects::Particles{{}, loader::INGAME_PALETTE[5]}, 0},
  {effects::EffectSprite{{}, 1, EffectMovement::None}, 0},
  {effects::RandomExplosionSound{}, 0}
};


const effects::EffectSpec SKELETON_KILL_EFFECT_SPEC[] = {
  {effects::SpriteCascade{3}, 0},
  {effects::RandomExplosionSound{}, 0},
  {effects::Particles{{1, 0}}, 0}
};


const effects::EffectSpec SODA_SIX_PACK_KILL_EFFECT_SPEC[] = {
  {effects::RandomExplosionSound{}, 0},
  {effects::EffectSprite{{0, 0}, 169, EffectMovement::FlyRight}, 0},
  {effects::EffectSprite{{0, 1}, 169, EffectMovement::FlyUpperRight}, 0},
  {effects::EffectSprite{{1, 0}, 169, EffectMovement::FlyUp}, 0},
  {effects::EffectSprite{{0, 1}, 169, EffectMovement::FlyUpperLeft}, 0},
  {effects::EffectSprite{{1, 0}, 169, EffectMovement::FlyLeft}, 0},
  {effects::EffectSprite{{0, 1}, 169, EffectMovement::FlyDown}, 0},
  {effects::EffectSprite{{0, 0}, 170, EffectMovement::FlyRight}, 0},
  {effects::EffectSprite{{0, 1}, 170, EffectMovement::FlyUpperRight}, 0},
  {effects::EffectSprite{{1, 0}, 170, EffectMovement::FlyUp}, 0},
  {effects::EffectSprite{{0, 1}, 170, EffectMovement::FlyUpperLeft}, 0},
  {effects::EffectSprite{{1, 0}, 170, EffectMovement::FlyLeft}, 0},
  {effects::EffectSprite{{0, 1}, 170, EffectMovement::FlyDown}, 0},
  {effects::ScoreNumber{{}, ScoreNumberType::S10000}, 0}
};


const effects::EffectSpec EYE_BALL_THROWER_KILL_EFFECT_SPEC[] = {
  {effects::Sound{data::SoundId::BiologicalEnemyDestroyed}, 0},
  {effects::EffectSprite{{0, -6}, 100, EffectMovement::FlyUp}, 0},
  {effects::EffectSprite{{0, -5}, 100, EffectMovement::FlyLeft}, 1},
  {effects::EffectSprite{{0, -4}, 100, EffectMovement::FlyRight}, 1},
  {effects::EffectSprite{{0, -3}, 100, EffectMovement::FlyUpperLeft}, 1},
  {effects::EffectSprite{{0, -1}, 100, EffectMovement::FlyUp}, 0},
  {effects::Particles{{}, loader::INGAME_PALETTE[13]}, 0}
};


const effects::EffectSpec LIVING_TURKEY_KILL_EFFECT_SPEC[] = {
  {effects::Sound{data::SoundId::BiologicalEnemyDestroyed}, 0},
  {effects::EffectSprite{{}, 84, EffectMovement::None}, 0}
};


const effects::EffectSpec BOSS4_PROJECTILE_KILL_EFFECT_SPEC[] = {
  {effects::SpriteCascade{3}, 0},
  {effects::Particles{{1, 0}, loader::INGAME_PALETTE[3]}, 0},
  {effects::RandomExplosionSound{}, 0}
};


#define M_TECH_KILL_EFFECT_SPEC_DEFINITION \
  {effects::RandomExplosionSound{}, 0}, \
  {effects::EffectSprite{{}, 1, EffectMovement::None}, 0}, \
  {effects::RandomExplosionSound{}, 2}, \
  {effects::EffectSprite{{-1, -2}, 1, EffectMovement::None}, 2}, \
  {effects::RandomExplosionSound{}, 4}, \
  {effects::EffectSprite{{1, -3}, 1, EffectMovement::None}, 4}, \
  {effects::Particles{{}}, 0}, \
  {effects::Particles{{-1, -1}, -1}, 0}, \
  {effects::Particles{{1, -2}, 1}, 0}


const effects::EffectSpec TECH_KILL_EFFECT_SPEC[] = {
  M_TECH_KILL_EFFECT_SPEC_DEFINITION
};


const effects::EffectSpec RADAR_DISH_KILL_EFFECT_SPEC[] = {
  M_TECH_KILL_EFFECT_SPEC_DEFINITION,
  {effects::ScoreNumber{{}, ScoreNumberType::S2000}, 0}
};


const effects::EffectSpec EXIT_SIGN_KILL_EFFECT_SPEC[] = {
  M_TECH_KILL_EFFECT_SPEC_DEFINITION,
  {effects::ScoreNumber{{}, ScoreNumberType::S10000}, 0}
};


const effects::EffectSpec FLOATING_ARROW_KILL_EFFECT_SPEC[] = {
  M_TECH_KILL_EFFECT_SPEC_DEFINITION,
  {effects::ScoreNumber{{}, ScoreNumberType::S500}, 0}
};


#undef M_TECH_KILL_EFFECT_SPEC_DEFINITION


const effects::EffectSpec SPIKE_BALL_KILL_EFFECT_SPEC[] = {
  {effects::Particles{{1, 0}, loader::INGAME_PALETTE[15]}, 0},
  {effects::EffectSprite{{-1, 1}, 1, EffectMovement::None}, 0},
  {effects::RandomExplosionSound{}, 0}
};


// The bonus globes have one additional destruction effect, which is handled
// separately - see configureBonusGlobe() in entity_configuration.ipp
const effects::EffectSpec BONUS_GLOBE_KILL_EFFECT_SPEC[] = {
  {effects::EffectSprite{{}, 72, EffectMovement::FlyLeft}, 0},
  {effects::EffectSprite{{}, 73, EffectMovement::FlyRight}, 0},
  {effects::ScoreNumber{{}, ScoreNumberType::S100}, 0},
  {effects::Sound{data::SoundId::GlassBreaking}, 0},
  {effects::Particles{{1, 0}, loader::INGAME_PALETTE[15]}, 0}
};


const effects::EffectSpec BIOLOGICAL_ENEMY_KILL_EFFECT_SPEC[] = {
  {effects::SpriteCascade{3}, 0},
  {effects::Particles{{1, 0}}, 0},
  {effects::Sound{data::SoundId::BiologicalEnemyDestroyed}, 0},
  {effects::EffectSprite{{1, 2}, 94, EffectMovement::FlyUp}, 0},
  {effects::EffectSprite{{}, 94, EffectMovement::FlyUpperRight}, 1},
  {effects::EffectSprite{{-1, 1}, 94, EffectMovement::FlyUpperLeft}, 2},
  {effects::EffectSprite{{1, -1}, 94, EffectMovement::FlyDown}, 3},
  {effects::EffectSprite{{-1, 2}, 94, EffectMovement::FlyUpperRight}, 4},
  {effects::EffectSprite{{1, 2}, 94, EffectMovement::FlyUpperLeft}, 5}
};


const effects::EffectSpec EXTENDED_BIOLOGICAL_ENEMY_KILL_EFFECT_SPEC[] = {
  {effects::SpriteCascade{3}, 0},
  {effects::Particles{{1, 0}}, 0},
  {effects::Sound{data::SoundId::BiologicalEnemyDestroyed}, 0},
  {effects::EffectSprite{{1, 2}, 94, EffectMovement::FlyUp}, 0},
  {effects::EffectSprite{{}, 94, EffectMovement::FlyUpperRight}, 1},
  {effects::EffectSprite{{-1, 1}, 94, EffectMovement::FlyUpperLeft}, 2},
  {effects::EffectSprite{{1, -1}, 94, EffectMovement::FlyDown}, 3},
  {effects::EffectSprite{{-1, 2}, 94, EffectMovement::FlyUpperRight}, 4},
  {effects::EffectSprite{{1, 2}, 94, EffectMovement::FlyUpperLeft}, 5},

  {effects::EffectSprite{{-1, 0}, 94, EffectMovement::FlyUp}, 0},
  {effects::EffectSprite{{-2, -2}, 94, EffectMovement::FlyUpperRight}, 1},
  {effects::EffectSprite{{-3, -1}, 94, EffectMovement::FlyUpperLeft}, 2},
  {effects::EffectSprite{{-1, -3}, 94, EffectMovement::FlyDown}, 3},
  {effects::EffectSprite{{-3, 0}, 94, EffectMovement::FlyUpperRight}, 4},
  {effects::EffectSprite{{-1, 0}, 94, EffectMovement::FlyUpperLeft}, 5},
  {effects::EffectSprite{{3, -2}, 94, EffectMovement::FlyUp}, 0},
  {effects::EffectSprite{{2, -4}, 94, EffectMovement::FlyUpperRight}, 1},
  {effects::EffectSprite{{1, -3}, 94, EffectMovement::FlyUpperLeft}, 2},
  {effects::EffectSprite{{3, -5}, 94, EffectMovement::FlyDown}, 3},
  {effects::EffectSprite{{1, -2}, 94, EffectMovement::FlyUpperRight}, 4},
  {effects::EffectSprite{{3, -2}, 94, EffectMovement::FlyUpperLeft}, 5}
};


const effects::EffectSpec CAMERA_KILL_EFFECT_SPEC[] = {
  {effects::Particles{{}}, 0},
  {effects::ScoreNumber{{}, ScoreNumberType::S100}, 0},
  {effects::RandomExplosionSound{}, 0}
};


const effects::EffectSpec BLUE_GUARD_KILL_EFFECT_SPEC[] = {
  {effects::Particles{{1, 0}, loader::INGAME_PALETTE[11]}, 0},
  {effects::RandomExplosionSound{}, 0},
  {effects::SpriteCascade{3}, 0}
};


const effects::EffectSpec NUCLEAR_WASTE_BARREL_KILL_EFFECT_SPEC[] = {
  {effects::RandomExplosionSound{}, 0},
  {effects::Particles{{1, 0}, loader::INGAME_PALETTE[4]}, 0},
  {effects::Particles{{1, 0}, loader::INGAME_PALETTE[15]}, 0},
  {effects::Particles{{1, 0}, loader::INGAME_PALETTE[11]}, 0},
  {effects::EffectSprite{{}, 18, EffectMovement::FlyUp}, 0},
  {effects::EffectSprite{{}, 17, EffectMovement::FlyDown}, 0},
  {effects::EffectSprite{{}, 15, EffectMovement::FlyUpperLeft}, 0},
  {effects::EffectSprite{{}, 16, EffectMovement::FlyUpperRight}, 0},
  {effects::EffectSprite{{}, 84, EffectMovement::None}, 0}
};


const effects::EffectSpec CONTAINER_BOX_KILL_EFFECT_SPEC[] = {
  {effects::RandomExplosionSound{}, 0},
  {effects::Particles{{1, 0}, loader::INGAME_PALETTE[4]}, 0},
  {effects::Particles{{1, 0}, loader::INGAME_PALETTE[15]}, 0},
  {effects::Particles{{1, 0}, loader::INGAME_PALETTE[11]}, 0},

  // TODO: Delay by 1 when container white-flash frame implemented
  {effects::EffectSprite{{1, -1}, 165, EffectMovement::FlyUp}, 0},
  {effects::EffectSprite{{1, -1}, 166, EffectMovement::FlyUpperLeft}, 0},
  {effects::EffectSprite{{1, -1}, 167, EffectMovement::FlyUpperRight}, 0},
  {effects::EffectSprite{{1, -1}, 166, EffectMovement::FlyDown}, 0},
};


const effects::EffectSpec SLIME_CONTAINER_KILL_EFFECT_SPEC[] = {
  {effects::Sound{data::SoundId::GlassBreaking}, 0},
  {effects::Particles{{1, 0}, loader::INGAME_PALETTE[15]}, 0}
};


