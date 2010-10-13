#ifndef INCLUDED_map_H
#define INCLUDED_map_H

#include "memory_config.h"
#include "buildings.h"
#include "lg/v2.h"

class EdgeArrows;

class Map
{
	const u8* mMapData;

	u16* MapHW() const { return (u16*)SCREEN_BASE_BLOCK(LG_MAP_OFFSET); }
	const u16* MapTileData() const { return (const u16*)&mMapData[6 + SquaresWide() * SquaresHigh()]; }

	public:
		enum MapFlags
		{
			Flag_Player1 = 0x1,
			Flag_Player2 = 0x2,
			Flag_Water = 0x4,
			Flag_DeepWater = 0x8,
			Flag_ReclaimPlayer1 = 0x10,
			Flag_ReclaimPlayer2 = 0x20,
		};

		Map(const u8* palette, const u8* mapData, const u8* tileData, u32 tileDataSize);
		void SetBuilding(int sx, int sy, BuildingIndex building);
		int SquaresWide() const { return ((u16*)mMapData)[0]; }
		int SquaresHigh() const { return ((u16*)mMapData)[1]; }
		int TilesWide() const { return SquaresWide() * LG_TILES_PER_SQUARE; }
		int TilesHigh() const { return SquaresHigh() * LG_TILES_PER_SQUARE; }
		int FirstBuildingIndex() const { return ((u16*)mMapData)[2]; }
		int Flags(const V2& loc) const;
		void ValidateScroll(V2& loc, EdgeArrows& edgeArrows);
		void GetMapLocation(const V2& scroll, V2 pixel, V2& square);
};

#endif
