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

#include "respawn_checkpoint.hpp"

#include "engine/base_components.hpp"
#include "engine/physical_components.hpp"
#include "game_logic/player/components.hpp"

#include "global_level_events.hpp"


namespace rigel { namespace game_logic { namespace interaction {

RespawnCheckpointSystem::RespawnCheckpointSystem(
  entityx::Entity player,
  entityx::EventManager* pEvents
)
  : mPlayer(player)
  , mpEvents(pEvents)
{
}


void RespawnCheckpointSystem::update(entityx::EntityManager& es) {
  using namespace engine::components;
  using components::RespawnCheckpoint;

  const auto& playerBox = *mPlayer.component<BoundingBox>();
  const auto& playerPos = *mPlayer.component<WorldPosition>();
  const auto worldSpacePlayerBounds =
    engine::toWorldSpace(playerBox, playerPos);

  es.each<RespawnCheckpoint, WorldPosition, BoundingBox>(
    [this, &worldSpacePlayerBounds](
      entityx::Entity entity,
      const RespawnCheckpoint&,
      const WorldPosition& position,
      const BoundingBox& bbox
    ) {
      const auto worldSpaceBbox = engine::toWorldSpace(bbox, position);
      if (worldSpaceBbox.intersects(worldSpacePlayerBounds)) {
        mpEvents->emit(rigel::events::CheckPointActivated{position});
        entity.remove<RespawnCheckpoint>();
      }
    });
}

}}}
