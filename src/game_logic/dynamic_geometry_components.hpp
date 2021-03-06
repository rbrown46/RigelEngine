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

#pragma once

#include "data/tile_attributes.hpp"
#include "engine/base_components.hpp"


namespace rigel { namespace game_logic { namespace components {

struct MapGeometryLink {
  explicit MapGeometryLink(engine::components::BoundingBox geometrySection)
    : mLinkedGeometrySection(geometrySection)
  {
  }

  engine::components::BoundingBox mLinkedGeometrySection;
};


struct TileDebris {
  data::map::TileIndex mTileIndex;
};


}}}
