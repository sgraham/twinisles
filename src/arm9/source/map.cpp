#include "lg/lg.h"
#include "map.h"
#include "edge_arrows.h"

Map::Map(const u8* palette, const u8* mapData, const u8* tileData, u32 tileDataSize) : mMapData(mapData)
{
	dmaCopy(palette, BG_PALETTE, 512);
	dmaCopy(tileData, (void*)CHAR_BASE_BLOCK(LG_TILES_OFFSET), tileDataSize);

	u16* mapHardware = MapHW();

	for (int i = 0; i < LG_BACKGROUND_TILES_SIZE * LG_BACKGROUND_TILES_SIZE; ++i)
	{
		mapHardware[i] = -1;
	}

	for (int y = 0; y < TilesHigh(); ++y)
	{
		for (int x = 0; x < TilesWide(); ++x)
		{
			mapHardware[y * LG_BACKGROUND_TILES_SIZE + x] = MapTileData()[y * TilesWide() + x];
		}
	}
}

void Map::SetBuilding(int sx, int sy, BuildingIndex building)
{
	assert(sx >= 0 && sx < SquaresWide());
	assert(sy >= 0 && sy < SquaresHigh());
	assert(building < BuildingIndex_Count);
	vu16* map = MapHW();

	for (int y = 0; y < LG_TILES_PER_SQUARE; ++y)
	{
		for (int x = 0; x < LG_TILES_PER_SQUARE; ++x)
		{
			int hwIndex = (sy * LG_TILES_PER_SQUARE + y) * LG_BACKGROUND_TILES_SIZE + (sx * LG_TILES_PER_SQUARE + x);
			int tileIndex = (FirstBuildingIndex() + building) * (LG_TILES_PER_SQUARE * LG_TILES_PER_SQUARE) + y * LG_TILES_PER_SQUARE + x;
			map[hwIndex] = tileIndex;
		}
	}

	DC_FlushAll();
}

int Map::Flags(const V2& loc) const
{
	assert(loc.x.Int() >= 0 && loc.x < SquaresWide());
	assert(loc.y.Int() >= 0 && loc.y < SquaresHigh());
	return mMapData[6 + loc.y.Int() * SquaresWide() + loc.x.Int()];
}

void Map::ValidateScroll(V2& loc, EdgeArrows& edgeArrows)
{
	if (loc.x.Int() <= 0) { loc.x = 0; edgeArrows.ShowLeft(false); }
	else edgeArrows.ShowLeft(true);

	if (loc.y.Int() <= 0) { loc.y = 0; edgeArrows.ShowUp(false); }
	else edgeArrows.ShowUp(true);

	if (loc.x.Int() >= TilesWide() * 8 - SCREEN_WIDTH) { loc.x = TilesWide() * 8 - SCREEN_WIDTH; edgeArrows.ShowRight(false); }
	else edgeArrows.ShowRight(true);

	if (loc.y.Int() >= TilesHigh() * 8 - SCREEN_HEIGHT) { loc.y = TilesHigh() * 8 - SCREEN_HEIGHT; edgeArrows.ShowDown(false); }
	else edgeArrows.ShowDown(true);
}

void Map::GetMapLocation(const V2& scroll, V2 pixel, V2& square)
{
	pixel += scroll;
	square = (pixel / LG_ART_SQUARE_SIZE).TruncateToInt();
}
