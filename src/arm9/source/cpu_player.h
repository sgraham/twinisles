#ifndef LG_INCLUDED_cpu_player_H
#define LG_INCLUDED_cpu_player_H

#include "playerid.h"
#include "buildings.h"
#include "lg/v2.h"
#include "map.h"
#include "lg/rand.h"

class Sim;

class CpuPlayer
{
	PlayerId mId;
	Sim& mSim;
	Map& mMap;
	Map::MapFlags mFlags;
	Map::MapFlags mReclaimFlags;
	int mUpdate;
	luvafair::Rand mRand;

	void BuildSomething();
	void ResearchSomething();

	void BuildBestHousing();
	void BuildBestFood();
	void BuildSchooling();
	void DeclareWar();
	bool FindLocationFor(BuildingIndex building, V2& result);
	bool DestroySomethingFor(BuildingIndex building, V2& result);
	bool FindBuilding(BuildingIndex building, V2& result);
	bool NotTooExpensive(BuildingIndex building) const;

	public:
		CpuPlayer(PlayerId player, Sim& sim, Map& map, Map::MapFlags flags, Map::MapFlags reclaimFlags);
		void Update();
};

#endif
