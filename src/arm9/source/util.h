#ifndef LG_INCLUDED_util_H
#define LG_INCLUDED_util_H

#include "buildings.h"
#include "map.h"
class Sim;

bool CanBuild(const Sim& sim, const Map& map, BuildingIndex buildingIndex, const V2& square, Map::MapFlags localPlayerFlag, Map::MapFlags localPlayerFlagReclaim);

#endif
