#ifndef INCLUDDED_buildings_H
#define INCLUDDED_buildings_H

enum BuildingIndex
{
	BuildingIndex_Invalid = -1,

	BuildingIndex_First,

	BuildingIndex_Hut = BuildingIndex_First,
	BuildingIndex_House,
	BuildingIndex_Apartment,
	BuildingIndex_Crops,
	BuildingIndex_Farm,
	BuildingIndex_FoodImport,
	BuildingIndex_WarBoat,
	BuildingIndex_Factory,
	BuildingIndex_Missiles,
	BuildingIndex_School,
	BuildingIndex_University,
	BuildingIndex_Hospital,
	BuildingIndex_Bulldozer,
	BuildingIndex_ReclaimedLand,
	BuildingIndex_Cancel,

	BuildingIndex_LastOnBuyMenu = BuildingIndex_Cancel,

	BuildingIndex_Destroyed,
	BuildingIndex_ExpiredCrops,
	BuildingIndex_LandingPad,

	BuildingIndex_Count
};

#endif
