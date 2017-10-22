/* Copyright (C) 2016, Nikolai Wuttke. All rights reserved.
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

#include "entity_factory.hpp"

#include "data/game_traits.hpp"
#include "data/unit_conversions.hpp"
#include "engine/life_time_components.hpp"
#include "engine/physics_system.hpp"
#include "engine/sprite_tools.hpp"
#include "game_logic/ai/blue_guard.hpp"
#include "game_logic/ai/hover_bot.hpp"
#include "game_logic/ai/laser_turret.hpp"
#include "game_logic/ai/messenger_drone.hpp"
#include "game_logic/ai/prisoner.hpp"
#include "game_logic/ai/red_bird.hpp"
#include "game_logic/ai/rocket_turret.hpp"
#include "game_logic/ai/security_camera.hpp"
#include "game_logic/ai/simple_walker.hpp"
#include "game_logic/ai/sliding_door.hpp"
#include "game_logic/ai/slime_blob.hpp"
#include "game_logic/ai/slime_pipe.hpp"
#include "game_logic/ai/spike_ball.hpp"
#include "game_logic/collectable_components.hpp"
#include "game_logic/damage_components.hpp"
#include "game_logic/dynamic_geometry_components.hpp"
#include "game_logic/effect_components.hpp"
#include "game_logic/item_container.hpp"
#include "game_logic/interaction/elevator.hpp"
#include "game_logic/interaction/force_field.hpp"
#include "game_logic/player_movement_system.hpp"
#include "game_logic/trigger_components.hpp"

RIGEL_DISABLE_WARNINGS
#include <boost/fusion/adapted/std_tuple.hpp>
#include <boost/fusion/include/for_each.hpp>
RIGEL_RESTORE_WARNINGS

#include <tuple>
#include <utility>


namespace ex = entityx;


namespace rigel { namespace game_logic {

using namespace data;
using namespace loader;
using namespace std;

using data::ActorID;

using namespace engine::components;
using namespace game_logic::components;


namespace {

// Assign gravity affected moving body component
template<typename EntityLike>
void addDefaultMovingBody(
  EntityLike& entity,
  const BoundingBox& boundingBox
) {
  using namespace engine::components::parameter_aliases;

  entity.template assign<MovingBody>(
    MovingBody{Velocity{0.0f, 0.0f}, GravityAffected{true}});
  entity.template assign<BoundingBox>(boundingBox);
  entity.template assign<ActivationSettings>(
    ActivationSettings::Policy::AlwaysAfterFirstActivation);
}


auto toPlayerProjectileType(const ProjectileType type) {
  using PType = components::PlayerProjectile::Type;
  static_assert(
    int(PType::Normal) == int(ProjectileType::PlayerRegularShot) &&
    int(PType::Laser) == int(ProjectileType::PlayerLaserShot) &&
    int(PType::Rocket) == int(ProjectileType::PlayerRocketShot) &&
    int(PType::Flame) == int(ProjectileType::PlayerFlameShot), "");
  return static_cast<PType>(static_cast<int>(type));
}


const base::Point<float> FLY_RIGHT[] = {
  {3.0f, 0.0f},
  {3.0f, 0.0f},
  {3.0f, 0.0f},
  {2.0f, 0.0f},
  {2.0f, 1.0f},
  {2.0f, 1.0f},
  {2.0f, 2.0f},
  {1.0f, 2.0f},
  {1.0f, 3.0f},
  {1.0f, 3.0f}
};


const base::Point<float> FLY_UPPER_RIGHT[] = {
  {3.0f, -3.0f},
  {2.0f, -2.0f},
  {2.0f, -1.0f},
  {1.0f,  0.0f},
  {1.0f,  0.0f},
  {1.0f,  1.0f},
  {1.0f,  2.0f},
  {1.0f,  2.0f},
  {1.0f,  3.0f},
  {1.0f,  3.0f}
};


const base::Point<float> FLY_UP[] = {
  {0.0f, -3.0f},
  {0.0f, -2.0f},
  {0.0f, -2.0f},
  {0.0f, -1.0f},
  {0.0f, 0.0f},
  {0.0f, 1.0f},
  {0.0f, 1.0f},
  {0.0f, 2.0f},
  {0.0f, 3.0f},
  {0.0f, 3.0f}
};


const base::Point<float> FLY_UPPER_LEFT[] = {
  {-3.0f, -3.0f},
  {-2.0f, -2.0f},
  {-2.0f, -1.0f},
  {-1.0f, 0.0f},
  {-1.0f, 0.0f},
  {-1.0f, 1.0f},
  {-1.0f, 2.0f},
  {-1.0f, 3.0f},
  {-1.0f, 4.0f},
  {-1.0f, 4.0f}
};


const base::Point<float> FLY_LEFT[] = {
  {-3.0f, 0.0f},
  {-3.0f, 0.0f},
  {-3.0f, 0.0f},
  {-2.0f, 0.0f},
  {-2.0f, 1.0f},
  {-2.0f, 1.0f},
  {-2.0f, 2.0f},
  {-1.0f, 3.0f},
  {-1.0f, 3.0f},
  {-1.0f, 3.0f}
};


const base::Point<float> FLY_DOWN[] = {
  {0.0f, 1.0f},
  {0.0f, 2.0f},
  {0.0f, 2.0f},
  {0.0f, 2.0f},
  {0.0f, 3.0f},
  {0.0f, 3.0f},
  {0.0f, 3.0f},
  {0.0f, 3.0f},
  {0.0f, 3.0f},
  {0.0f, 3.0f}
};


const base::Point<float> SWIRL_AROUND[] = {
  {-2.0f, 1.0f},
  {-2.0f, 1.0f},
  {-2.0f, 1.0f},
  {-1.0f, 1.0f},
  {0.0f, 1.0f},
  {1.0f, 1.0f},
  {2.0f, 0.0f},
  {1.0f, -1.0f},
  {-2.0f, -1.0f},
  {-2.0f, 1.0f}
};


const base::ArrayView<base::Point<float>> MOVEMENT_SEQUENCES[] = {
  FLY_RIGHT,
  FLY_UPPER_RIGHT,
  FLY_UP,
  FLY_UPPER_LEFT,
  FLY_LEFT,
  FLY_DOWN,
  SWIRL_AROUND
};

}


#include "entity_configuration.ipp"


EntityFactory::EntityFactory(
  engine::Renderer* pRenderer,
  ex::EntityManager* pEntityManager,
  const ActorImagePackage* pSpritePackage,
  const data::Difficulty difficulty)
  : mpRenderer(pRenderer)
  , mpEntityManager(pEntityManager)
  , mpSpritePackage(pSpritePackage)
  , mDifficulty(difficulty)
{
}


Sprite EntityFactory::createSpriteForId(const ActorID actorID) {
  auto sprite = createSpriteComponent(actorID);
  configureSprite(sprite, actorID);
  return sprite;
}


Sprite EntityFactory::createSpriteComponent(const ActorID mainId) {
  auto iData = mSpriteDataCache.find(mainId);
  if (iData == mSpriteDataCache.end()) {
    engine::SpriteDrawData drawData;

    int lastDrawOrder = 0;
    int lastFrameCount = 0;
    std::vector<int> framesToRender;

    const auto actorParts = actorIDListForActor(mainId);
    for (const auto part : actorParts) {
      const auto& actorData = mpSpritePackage->loadActor(part);
      lastDrawOrder = actorData.mDrawIndex;

      for (const auto& frameData : actorData.mFrames) {
        auto texture = engine::OwningTexture{
          mpRenderer, frameData.mFrameImage};
        drawData.mFrames.emplace_back(
          std::move(texture), frameData.mDrawOffset);
      }

      framesToRender.push_back(lastFrameCount);
      lastFrameCount = int(actorData.mFrames.size());
    }

    drawData.mDrawOrder = adjustedDrawOrder(mainId, lastDrawOrder);

    if (mainId == 5 || mainId == 6) {
      for (int i=0; i<39; ++i) {
        drawData.mFrames[i].mDrawOffset.x -= 1;
      }
    }

    iData = mSpriteDataCache.emplace(
      mainId,
      SpriteData{std::move(drawData), std::move(framesToRender)}
    ).first;
  }

  const auto& data = iData->second;
  return {&data.mDrawData, data.mInitialFramesToRender};
}


entityx::Entity EntityFactory::createSprite(
  const data::ActorID actorID,
  const bool assignBoundingBox
) {
  auto entity = mpEntityManager->create();
  auto sprite = createSpriteForId(actorID);
  entity.assign<Sprite>(sprite);

  if (assignBoundingBox) {
    entity.assign<BoundingBox>(engine::inferBoundingBox(sprite));
  }
  return entity;
}

entityx::Entity EntityFactory::createSprite(
  const data::ActorID actorID,
  const base::Vector& position,
  const bool assignBoundingBox
) {
  auto entity = createSprite(actorID, assignBoundingBox);
  entity.assign<WorldPosition>(position);
  return entity;
}


entityx::Entity EntityFactory::createProjectile(
  const ProjectileType type,
  const WorldPosition& pos,
  const ProjectileDirection direction
) {
  auto entity = createActor(actorIdForProjectile(type, direction), pos);
  entity.assign<Active>();

  configureProjectile(
    entity,
    type,
    pos,
    direction,
    *entity.component<BoundingBox>());

  return entity;
}


entityx::Entity EntityFactory::createActor(
  const data::ActorID id,
  const base::Vector& position
) {
  auto entity = createSprite(id, position);
  auto& sprite = *entity.component<Sprite>();
  const auto boundingBox = engine::inferBoundingBox(sprite);

  configureEntity(entity, id, boundingBox);

  return entity;
}


void EntityFactory::configureProjectile(
  entityx::Entity entity,
  const ProjectileType type,
  WorldPosition position,
  const ProjectileDirection direction,
  const BoundingBox& boundingBox
) {
  using namespace engine::components::parameter_aliases;
  using namespace game_logic::components::parameter_aliases;

  const auto isGoingLeft = direction == ProjectileDirection::Left;

  // Position adjustment for the flame thrower shot
  if (type == ProjectileType::PlayerFlameShot) {
    if (isHorizontal(direction)) {
      position.y += 1;
    } else {
      position.x -= 1;
    }
  }

  // Position adjustment for left-facing projectiles. We want the incoming
  // position to always represent the projectile's origin, which means we need
  // to adjust the position by the projectile's length to match the left-bottom
  // corner positioning system.
  if (isHorizontal(direction) && isGoingLeft) {
    position.x -= boundingBox.size.width - 1;

    if (type == ProjectileType::PlayerFlameShot) {
      position.x += 3;
    }
  }

  const auto speed = speedForProjectileType(type);
  const auto damageAmount = damageForProjectileType(type);

  entity.assign<MovingBody>(
    Velocity{directionToVector(direction) * speed},
    GravityAffected{false},
    IsPlayer{false});
  if (isPlayerProjectile(type)) {
    // Some player projectiles do have collisions with walls, but that's
    // handled by player::ProjectileSystem.
    entity.component<MovingBody>()->mIgnoreCollisions = true;

    entity.assign<DamageInflicting>(damageAmount, DestroyOnContact{false});
    entity.assign<PlayerProjectile>(toPlayerProjectileType(type));

    entity.assign<AutoDestroy>(AutoDestroy{
      AutoDestroy::Condition::OnLeavingActiveRegion});
  } else {
    entity.assign<PlayerDamaging>(damageAmount, false, true);

    entity.assign<AutoDestroy>(AutoDestroy{
      AutoDestroy::Condition::OnWorldCollision,
      AutoDestroy::Condition::OnLeavingActiveRegion});
  }

  // For convenience, the enemy laser shot muzzle flash is created along with
  // the projectile.
  if (type == ProjectileType::EnemyLaserShot) {
    const auto muzzleFlashSpriteId = static_cast<data::ActorID>(
      direction == ProjectileDirection::Left ? 147 : 148);
    auto muzzleFlash = createSprite(muzzleFlashSpriteId);
    muzzleFlash.assign<WorldPosition>(position);
    muzzleFlash.assign<AutoDestroy>(AutoDestroy::afterTimeout(1));
  }
}


entityx::Entity EntityFactory::createEntitiesForLevel(
  const data::map::ActorDescriptionList& actors
) {
  entityx::Entity playerEntity;

  for (const auto& actor : actors) {
    // Difficulty/section markers should never appear in the actor descriptions
    // coming from the loader, as they are handled during pre-processing.
    assert(
      actor.mID != 82 && actor.mID != 83 &&
      actor.mID != 103 && actor.mID != 104);

    auto entity = mpEntityManager->create();

    auto position = actor.mPosition;
    if (actor.mAssignedArea) {
      // For dynamic geometry, the original position refers to the top-left
      // corner of the assigned area, but it refers to the bottom-left corner
      // for all other entities. Adjust the position here so that it's also
      // bottom-left.
      position.y += actor.mAssignedArea->size.height - 1;
    }
    entity.assign<WorldPosition>(position);

    BoundingBox boundingBox;
    if (actor.mAssignedArea) {
      const auto mapSectionRect = *actor.mAssignedArea;
      entity.assign<MapGeometryLink>(mapSectionRect);

      boundingBox = mapSectionRect;
      boundingBox.topLeft = {0, 0};
    } else if (hasAssociatedSprite(actor.mID)) {
      const auto sprite = createSpriteForId(actor.mID);
      boundingBox = engine::inferBoundingBox(sprite);
      entity.assign<Sprite>(sprite);
    }

    configureEntity(entity, actor.mID, boundingBox);

    const auto isPlayer = actor.mID == 5 || actor.mID == 6;
    if (isPlayer) {
      game_logic::initializePlayerEntity(entity, actor.mID == 6);
      playerEntity = entity;
    }
  }

  return playerEntity;
}


entityx::Entity createOneShotSprite(
  EntityFactory& factory,
  const ActorID id,
  const base::Vector& position
) {
  auto entity = factory.createSprite(id, position, true);
  engine::startAnimationLoop(entity, 1, 0, boost::none);
  const auto numAnimationFrames = static_cast<int>(
    entity.component<Sprite>()->mpDrawData->mFrames.size());
  entity.assign<AutoDestroy>(AutoDestroy::afterTimeout(numAnimationFrames));
  return entity;
}


entityx::Entity createFloatingOneShotSprite(
  EntityFactory& factory,
  const data::ActorID id,
  const base::Vector& position
) {
  using namespace engine::components::parameter_aliases;

  auto entity = createOneShotSprite(factory, id, position);
  entity.assign<MovingBody>(MovingBody{
    Velocity{0, -1.0f},
    GravityAffected{false},
    IsPlayer{false},
    IgnoreCollisions{true}});
  return entity;
}


entityx::Entity spawnMovingEffectSprite(
  EntityFactory& factory,
  const ActorID id,
  const SpriteMovement movement,
  const base::Vector& position
) {
  auto entity = factory.createSprite(id, position, true);
  configureMovingEffectSprite(entity, movement);
  entity.assign<AnimationLoop>(1);
  return entity;
}


void spawnFloatingScoreNumber(
  EntityFactory& factory,
  const ScoreNumberType type,
  const base::Vector& position
) {
  using namespace engine::components::parameter_aliases;

  auto entity = factory.createSprite(scoreNumberActor(type), position, true);
  engine::startAnimationSequence(entity, SCORE_NUMBER_ANIMATION_SEQUENCE);
  entity.assign<MovementSequence>(SCORE_NUMBER_MOVE_SEQUENCE);
  entity.assign<MovingBody>(
    Velocity{},
    GravityAffected{false},
    IsPlayer{false},
    IgnoreCollisions{true});
  entity.assign<AutoDestroy>(AutoDestroy::afterTimeout(SCORE_NUMBER_LIFE_TIME));
  entity.assign<Active>();
}

}}
