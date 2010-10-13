#include "lg/lg.h"
#include "util.h"
#include "sim.h"

bool CanBuild(const Sim& sim, const Map& map, BuildingIndex buildingIndex, const V2& square, Map::MapFlags localPlayerFlag, Map::MapFlags localPlayerFlagReclaim)
{
	if (buildingIndex == BuildingIndex_Invalid) return false;

	if (buildingIndex == BuildingIndex_ReclaimedLand && map.Flags(square) & localPlayerFlagReclaim) return true;

	if (buildingIndex == BuildingIndex_Bulldozer
			&& ((map.Flags(square) & localPlayerFlag) || map.Flags(square) & localPlayerFlagReclaim)
			&& (sim.At(square) != BuildingIndex_Invalid && sim.At(square) != BuildingIndex_Destroyed))
		return true;

	if (map.Flags(square) & localPlayerFlag) return true;

	if (sim.At(square) == BuildingIndex_ReclaimedLand && map.Flags(square) & localPlayerFlagReclaim) return true;

	if (sim.At(square) != BuildingIndex_Invalid && map.Flags(square) & localPlayerFlagReclaim) return true;
	
	return false;
}
