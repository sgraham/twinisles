// if tornado destroys, and current building < 20%, stop
//
#include "lg/lg.h"
#include "cpu_player.h"
#include "sim.h"
#include "sim.h"
#include "util.h"
#include "lg/rand.h"
#include "lg/scoped_array.h"

using namespace luvafair;

static BuildingIndex sFoodInOrder[] =
{
	BuildingIndex_FoodImport,
	BuildingIndex_Farm,
	BuildingIndex_Crops,
};

static BuildingIndex sHousingInOrder[] =
{
	BuildingIndex_Apartment,
	BuildingIndex_House,
	BuildingIndex_Hut,
};

static BuildingIndex sSchoolingInOrder[] =
{
	BuildingIndex_University,
	BuildingIndex_School,
};

CpuPlayer::CpuPlayer(PlayerId player, Sim& sim, Map& map, Map::MapFlags flags, Map::MapFlags reclaimFlags) : mId(player), mSim(sim), mMap(map), mFlags(flags), mReclaimFlags(reclaimFlags), mUpdate(0)
{
}

bool CpuPlayer::FindBuilding(BuildingIndex building, V2& result)
{
	for (int x = 0; x < mMap.SquaresWide(); ++x)
	{
		for (int y = 0; y < mMap.SquaresHigh(); ++y)
		{
			V2 pos(x, y);
			if (mSim.At(pos, mId) == building)
			{
				result = pos;
				return true;
			}
		}
	}

	return false;
}

bool CpuPlayer::DestroySomethingFor(BuildingIndex building, V2& result)
{
	// if we have a university or building is a university, destroy a school, if any, and use that
	if (building == BuildingIndex_University || mSim.SumBuildings(mId, BuildingIndex_University, 1) > 0)
	{
		if (FindBuilding(BuildingIndex_School, result))
		{
			mSim.StartBuilding(mId, BuildingIndex_Bulldozer, result);
			return true;
		}
	}

	// if it's food, find a worse food to destroy and use that
	if (building == BuildingIndex_Crops || building == BuildingIndex_Farm || building == BuildingIndex_FoodImport)
	{
		if (building == BuildingIndex_Crops) return false;
		if (building == BuildingIndex_Farm)
		{
			if (FindBuilding(BuildingIndex_Crops, result))
			{
				mSim.StartBuilding(mId, BuildingIndex_Bulldozer, result);
				return true;
			}
		}
		if (building == BuildingIndex_FoodImport)
		{
			if (FindBuilding(BuildingIndex_Crops, result))
			{
				mSim.StartBuilding(mId, BuildingIndex_Bulldozer, result);
				return true;
			}
			if (FindBuilding(BuildingIndex_Farm, result))
			{
				mSim.StartBuilding(mId, BuildingIndex_Bulldozer, result);
				return true;
			}
			if (FindBuilding(BuildingIndex_Hut, result))
			{
				mSim.StartBuilding(mId, BuildingIndex_Bulldozer, result);
				return true;
			}
			if (FindBuilding(BuildingIndex_House, result))
			{
				mSim.StartBuilding(mId, BuildingIndex_Bulldozer, result);
				return true;
			}
		}
	}
	
	// if it's housing, find a worse housing to destroy and use that
	if (building == BuildingIndex_Hut || building == BuildingIndex_House || building == BuildingIndex_Apartment)
	{
		if (building == BuildingIndex_Hut) return false;
		if (building == BuildingIndex_House)
		{
			if (FindBuilding(BuildingIndex_Hut, result))
			{
				mSim.StartBuilding(mId, BuildingIndex_Bulldozer, result);
				return true;
			}
		}
		if (building == BuildingIndex_Apartment)
		{
			if (FindBuilding(BuildingIndex_Hut, result))
			{
				mSim.StartBuilding(mId, BuildingIndex_Bulldozer, result);
				return true;
			}
			if (FindBuilding(BuildingIndex_House, result))
			{
				mSim.StartBuilding(mId, BuildingIndex_Bulldozer, result);
				return true;
			}
			if (FindBuilding(BuildingIndex_Crops, result))
			{
				mSim.StartBuilding(mId, BuildingIndex_Bulldozer, result);
				return true;
			}
			if (FindBuilding(BuildingIndex_Farm, result))
			{
				mSim.StartBuilding(mId, BuildingIndex_Bulldozer, result);
				return true;
			}
		}
	}

	if (building == BuildingIndex_School)
	{
		F32 food;
		F32 housing;
		mSim.GetCurrentValues(mId, &food, &housing, 0, 0, 0, 0, 0, 0);
		if (food > housing)
		{
			if (FindBuilding(BuildingIndex_Crops, result))
			{
				mSim.StartBuilding(mId, BuildingIndex_Bulldozer, result);
				return true;
			}
			if (FindBuilding(BuildingIndex_Farm, result))
			{
				mSim.StartBuilding(mId, BuildingIndex_Bulldozer, result);
				return true;
			}
			if (FindBuilding(BuildingIndex_FoodImport, result))
			{
				mSim.StartBuilding(mId, BuildingIndex_Bulldozer, result);
				return true;
			}
		}
		else
		{
			if (FindBuilding(BuildingIndex_Hut, result))
			{
				mSim.StartBuilding(mId, BuildingIndex_Bulldozer, result);
				return true;
			}
			if (FindBuilding(BuildingIndex_House, result))
			{
				mSim.StartBuilding(mId, BuildingIndex_Bulldozer, result);
				return true;
			}
			if (FindBuilding(BuildingIndex_FoodImport, result))
			{
				mSim.StartBuilding(mId, BuildingIndex_Bulldozer, result);
				return true;
			}
		}
	}

	return false;
}

bool CpuPlayer::FindLocationFor(BuildingIndex building, V2& result)
{
	/*
	bool waterBonus = false;
	if (building == BuildingIndex_Crops || building == BuildingIndex_Apartment)
	{
		waterBonus = true;
	}*/

	ScopedArray<V2> candidates(new V2[mMap.SquaresWide() * mMap.SquaresHigh()]);
	int numCandidates = 0;
	for (int x = 0; x < mMap.SquaresWide(); ++x)
	{
		for (int y = 0; y < mMap.SquaresHigh(); ++y)
		{
			V2 pos(x, y);
			if (CanBuild(mSim, mMap, building, pos, mFlags, mReclaimFlags)
					&& (mSim.At(pos) == BuildingIndex_Invalid || mSim.At(pos) == BuildingIndex_Destroyed))
			{
				candidates[numCandidates++] = pos;
			}
		}
	}

	if (numCandidates == 0)
	{
		return DestroySomethingFor(building, result);
	}

	result = candidates[mRand.Get() % numCandidates];
	return true;
}

bool CpuPlayer::NotTooExpensive(BuildingIndex building) const
{
	F32 prod;
	mSim.GetCurrentValues(mId, 0, 0, 0, &prod, 0, 0, 0, 0);
	return F32(mSim.CostOf(mId, building)) / F32(100) < prod * F32(100);
}

void CpuPlayer::BuildBestFood()
{
	for (int i = 0; i < ARRAYSIZE(sFoodInOrder); ++i)
	{
		if (mSim.CanBuy(mId, sFoodInOrder[i]) && NotTooExpensive(sFoodInOrder[i]))
		{
			V2 pos;
			if (!FindLocationFor(sFoodInOrder[i], pos)) return;
			mSim.StartBuilding(mId, sFoodInOrder[i], pos);
			break;
		}
	}
}

void CpuPlayer::BuildBestHousing()
{
	for (int i = 0; i < ARRAYSIZE(sHousingInOrder); ++i)
	{
		if (mSim.CanBuy(mId, sHousingInOrder[i]) && NotTooExpensive(sHousingInOrder[i]))
		{
			V2 pos;
			if (!FindLocationFor(sHousingInOrder[i], pos)) return;
			mSim.StartBuilding(mId, sHousingInOrder[i], pos);
			break;
		}
	}
}

void CpuPlayer::BuildSchooling()
{
	for (int i = 0; i < ARRAYSIZE(sSchoolingInOrder); ++i)
	{
		if (mSim.CanBuy(mId, sSchoolingInOrder[i]) && NotTooExpensive(sSchoolingInOrder[i]))
		{
			V2 pos;
			if (!FindLocationFor(sSchoolingInOrder[i], pos)) return;
			mSim.StartBuilding(mId, sSchoolingInOrder[i], pos);
			break;
		}
	}
}

void CpuPlayer::BuildSomething()
{
	if (mSim.CurrentBuilding(mId) == BuildingIndex_Invalid)
	{
		F32 food, housing, pop;
		bool kfood, khousing, kschool;

		if (mSim.CanBuy(mId, BuildingIndex_Missiles))
		{
			V2 pos;
			if (FindLocationFor(BuildingIndex_Missiles, pos))
			{
				mSim.StartBuilding(mId, BuildingIndex_Missiles, pos);
				return;
			}
		}

		mSim.GetKilledOff(mId, &kfood, &khousing, &kschool);
		if (kschool)
		{
			BuildSchooling();
		}
		else
		{
			mSim.GetCurrentValues(mId, &food, &housing, &pop, 0, 0, 0, 0, 0);
			if (food < housing)
			{
				BuildBestFood();
			}
			else
			{
				BuildBestHousing();
			}
		}
	}
}

void CpuPlayer::ResearchSomething()
{
	if (mSim.CurrentResearch(mId) == Tech_Invalid)
	{
		Technology best = Tech_Invalid;
		u16 bestCost = 65535;

		static const Technology tryFirst[] = 
		{
			Tech_Farming,
			Tech_Construction,
			Tech_Alphabet,
		};

		for (int i = 0; i < ARRAYSIZE(tryFirst); ++i)
		{
			if (!mSim.GetHaveTechnology(mId, tryFirst[i]) && mSim.GetHavePrerequisitesForTechnology(mId, tryFirst[i]))
			{
				if (mSim.NumBuildings(mId) > 20)
				{
					mSim.StartResearch(mId, tryFirst[i]);
					return;
				}
			}
		}

		for (int i = 0; i < Tech_Count; ++i)
		{
			if (!mSim.GetHaveTechnology(mId, (Technology)i) && mSim.GetHavePrerequisitesForTechnology(mId, (Technology)i))
			{
				if (gTechInfo[i].cost < bestCost)
				{
					best = (Technology)i;
					bestCost = gTechInfo[i].cost;
				}
			}
		}

		if (best != Tech_Invalid)
		{
			mSim.StartResearch(mId, best);
		}
	}
}

void CpuPlayer::DeclareWar()
{
	if (!mSim.CanBuy(mId, BuildingIndex_WarBoat)) return;

	F32 myPop, otherPop;
	mSim.GetCurrentValues(mId, 0, 0, &myPop, 0, 0, 0, 0, 0);
	mSim.GetCurrentValues(PlayerId(mId.mId ^ 1), 0, 0, &otherPop, 0, 0, 0, 0, 0);
	if (myPop > otherPop && mRand.Get() % (60*60*4) == 0)
	{
		mSim.StartBuilding(mId, BuildingIndex_WarBoat, V2());
	}
}

void CpuPlayer::Update()
{
	if (++mUpdate % 10 != 0) return;

	BuildSomething();

	ResearchSomething();

	DeclareWar();

	// when to reclaim land?
	// when to build factory?
	// when to destroy to replace? maybe only when full? how to decide what to destroy?
	// how to do difficulty levels? force production lower maybe?
}
