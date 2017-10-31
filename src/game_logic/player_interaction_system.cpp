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

#include "player_interaction_system.hpp"

#include "data/player_data.hpp"
#include "engine/base_components.hpp"
#include "engine/physics_system.hpp"
#include "engine/visual_components.hpp"
#include "game_logic/collectable_components.hpp"
#include "game_logic/damage_components.hpp"
#include "game_logic/entity_factory.hpp"
#include "game_logic/interaction/force_field.hpp"
#include "game_logic/interaction/teleporter.hpp"
#include "game_service_provider.hpp"


namespace rigel { namespace game_logic {

using data::PlayerModel;
using engine::components::AnimationLoop;
using engine::components::BoundingBox;
using engine::components::Sprite;
using engine::components::WorldPosition;
using player::PlayerState;

using namespace game_logic::components;

namespace ex = entityx;

namespace {

const auto NUM_COLLECTABLE_LETTERS = 5;
const auto CORRECT_LETTER_COLLECTION_BONUS = 100000;

const int SCORE_NUMBER_X_OFFSETS[] = {-3, 0, 3, 0};


void spawnScoreNumbers(
  const base::Vector& position,
  int score,
  EntityFactory& entityFactory
) {
  std::vector<ScoreNumberType> numbers;
  for (; score >= 10000; score -= 10000) {
    numbers.push_back(ScoreNumberType::S10000);
  }

  for (; score >= 5000; score -= 5000) {
    numbers.push_back(ScoreNumberType::S5000);
  }

  for (; score >= 2000; score -= 2000) {
    numbers.push_back(ScoreNumberType::S2000);
  }

  for (; score >= 500; score -= 500) {
    numbers.push_back(ScoreNumberType::S500);
  }

  for (; score >= 100; score -= 100) {
    numbers.push_back(ScoreNumberType::S100);
  }

  if (numbers.empty()) {
    return;
  }

  auto yOffset = static_cast<int>(numbers.size() - 1);
  for (const auto numberType : numbers) {
    const auto offset = base::Vector{0, yOffset};
    --yOffset;
    spawnFloatingScoreNumber(entityFactory, numberType, position - offset);
  }
}


bool collectionOrderViolated(
  const std::unordered_set<data::CollectableLetterType>& collectedSoFar,
  const data::CollectableLetterType newLetter
) {
  using LT = data::CollectableLetterType;

  std::unordered_set<data::CollectableLetterType> requiredLetters;
  switch (newLetter) {
    case LT::N: requiredLetters = {}; break;
    case LT::U: requiredLetters = {LT::N}; break;
    case LT::K: requiredLetters = {LT::N, LT::U}; break;
    case LT::E: requiredLetters = {LT::N, LT::U, LT::K}; break;
    case LT::M: requiredLetters = {LT::N, LT::U, LT::K, LT::E}; break;
  }

  return collectedSoFar != requiredLetters;
}

}


PlayerInteractionSystem::PlayerInteractionSystem(
  ex::Entity player,
  PlayerModel* pPlayerModel,
  IGameServiceProvider* pServices,
  EntityFactory* pEntityFactory,
  TeleportCallback teleportCallback
)
  : mPlayer(player)
  , mpPlayerModel(pPlayerModel)
  , mpServiceProvider(pServices)
  , mpEntityFactory(pEntityFactory)
  , mTeleportCallback(teleportCallback)
{
}


void PlayerInteractionSystem::update(entityx::EntityManager& es) {
  assert(mPlayer.has_component<PlayerControlled>());
  auto& state = *mPlayer.component<PlayerControlled>();
  if (state.isPlayerDead()) {
    return;
  }

  if (state.mState != PlayerState::LookingUp) {
    state.mPerformedInteraction = false;
  }

  if (!state.mPerformedInteraction && state.mState == PlayerState::LookingUp) {
    const auto& playerBox = *mPlayer.component<BoundingBox>();
    const auto& playerPos = *mPlayer.component<WorldPosition>();
    const auto worldSpacePlayerBounds =
      engine::toWorldSpace(playerBox, playerPos);
    es.each<Interactable, WorldPosition, BoundingBox>(
      [this, &es, &state, &worldSpacePlayerBounds](
        ex::Entity entity,
        const Interactable& interactable,
        const WorldPosition& pos,
        const BoundingBox& bbox
      ) {
        if (state.mPerformedInteraction) {
          return;
        }
        const auto objectBounds = engine::toWorldSpace(bbox, pos);
        if (worldSpacePlayerBounds.intersects(objectBounds)) {
          performInteraction(es, entity, interactable.mType);
          state.mPerformedInteraction = true;
        }
      });
  }

  // ----------------------------------------------------------------------
  es.each<CollectableItem, WorldPosition, BoundingBox>(
    [this, &es](
      ex::Entity entity,
      const CollectableItem& collectable,
      const WorldPosition& pos,
      const BoundingBox& collisionRect
    ) {
      using namespace data;

      auto worldSpaceBbox = collisionRect;
      worldSpaceBbox.topLeft +=
        base::Vector{pos.x, pos.y - (worldSpaceBbox.size.height - 1)};

      const auto playerPos = *mPlayer.component<WorldPosition>();
      auto playerBBox = *mPlayer.component<BoundingBox>();
      playerBBox.topLeft +=
        base::Vector{playerPos.x, playerPos.y - (playerBBox.size.height - 1)};

      if (worldSpaceBbox.intersects(playerBBox)) {
        bool ignoreScore = false;
        boost::optional<data::SoundId> soundToPlay;

        if (collectable.mGivenCollectableLetter) {
          auto& collectedLetters = mpPlayerModel->mCollectedLetters;

          const auto givenLetter = *collectable.mGivenCollectableLetter;
          mpPlayerModel->mLetterCollectionOrderViolated |=
            collectionOrderViolated(collectedLetters, givenLetter);
          collectedLetters.insert(givenLetter);

          if (
            collectedLetters.size() == NUM_COLLECTABLE_LETTERS &&
            !mpPlayerModel->mLetterCollectionOrderViolated
          ) {
            soundToPlay = SoundId::LettersCollectedCorrectly;
            mpPlayerModel->mScore += CORRECT_LETTER_COLLECTION_BONUS;
            ignoreScore = true;

            for (int i = 0; i < 10; ++i) {
              const auto offset = base::Vector{
                SCORE_NUMBER_X_OFFSETS[i % 4],
                -i};
              spawnFloatingScoreNumber(
                *mpEntityFactory, ScoreNumberType::S10000, pos + offset);
            }
          } else {
            soundToPlay = SoundId::ItemPickup;

            // In the original game, letters only spawn a floating '100',
            // but give 10100 points. This is likely a bug, since the hint
            // message given when collecting the letters in the wrong order
            // mentions getting 10000 points, but there are no additional
            // 10000 points given beyond what you already get when collecting a
            // single letter. This makes me suspect that you're actually supposed
            // to only get 100 for a single letter, and get the 10000 if you
            // collect them out of order.
            spawnFloatingScoreNumber(
              *mpEntityFactory, ScoreNumberType::S100, pos);
          }
        }


        if (collectable.mGivenScore && !ignoreScore) {
          const auto score = *collectable.mGivenScore;
          assert(score > 0);
          mpPlayerModel->mScore += score;

          soundToPlay = SoundId::ItemPickup;

          if (collectable.mSpawnScoreNumbers) {
            spawnScoreNumbers(pos, score, *mpEntityFactory);
          }
        }

        if (collectable.mGivenHealth) {
          assert(*collectable.mGivenHealth > 0);
          mpPlayerModel->mHealth = std::min(
            data::MAX_HEALTH,
            mpPlayerModel->mHealth + *collectable.mGivenHealth);

          soundToPlay = SoundId::HealthPickup;
        }

        if (collectable.mGivenWeapon) {
          mpPlayerModel->switchToWeapon(*collectable.mGivenWeapon);
          soundToPlay = SoundId::WeaponPickup;
        }

        if (collectable.mGivenItem) {
          const auto itemType = *collectable.mGivenItem;
          mpPlayerModel->mInventory.insert(itemType);

          soundToPlay = itemType == InventoryItemType::RapidFire ?
            SoundId::WeaponPickup :
            SoundId::ItemPickup;
        }

        if (soundToPlay) {
          mpServiceProvider->playSound(*soundToPlay);
        }

        es.destroy(entity.id());
      }
    });
}


void PlayerInteractionSystem::performInteraction(
  entityx::EntityManager& es,
  entityx::Entity interactable,
  const components::InteractableType type
) {
  switch (type) {
    case InteractableType::Teleporter:
      mTeleportCallback(interactable);
      break;

    case InteractableType::ForceFieldCardReader:
      if (interaction::disableForceField(es, interactable, mpPlayerModel)) {
        triggerPlayerInteractionAnimation();
      }
      break;
  }
}


void PlayerInteractionSystem::triggerPlayerInteractionAnimation() {
  auto& state = *mPlayer.component<PlayerControlled>();
  state.enterTimedInteractionLock();
}

}}
