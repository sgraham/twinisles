#include "lg/lg.h"
#include "saved_inputs.h"
#include <time.h>
#include "lg/rand.h"
#include <cstring>

using namespace luvafair;

static char SavedInputsReplayMagic[4] = { 'T', 'I', 'S', '2' };
static char SavedInputsGameMagic[4] = { 'T', 'I', 'G', '2' };
static const char* saveGameName = "fat:/twnisles.sg";

static int CheckMemoryHeader(char* magic)
{
	assert(sizeof(SavedInputsReplayMagic) == sizeof(SavedInputsGameMagic));

	bool haveInSRAM = true;
	for (int i = 0; i < (int)sizeof(SavedInputsReplayMagic); ++i)
	{
		if (SRAM[i] != magic[i])
		{
			haveInSRAM = false;
			break;
		}
	}
	if (haveInSRAM)
	{
		return 1;
	}

	FILE* in = fopen(saveGameName, "rb");
	if (!in) return 0;
	bool haveInFAT = true;
	char data[sizeof(SavedInputsReplayMagic)];
	fread(data, sizeof(SavedInputsReplayMagic), 1, in);
	fclose(in);
	for (int i = 0; i < (int)sizeof(SavedInputsReplayMagic); ++i)
	{
		if (data[i] != magic[i])
		{
			haveInFAT = false;
		}
	}

	return haveInFAT ? 2 : 0;
}

SavedInputPlayer::SavedInputPlayer() : mPos(0)
{
	int checkedReplay = CheckMemoryHeader(SavedInputsReplayMagic);
	int checkedGame = CheckMemoryHeader(SavedInputsGameMagic);
	if (checkedReplay == 1 || checkedGame == 1)
	{
		mData = (u8*)SRAM;
		mDeleteData = false;
	}
	else if (checkedReplay == 2 || checkedGame == 2)
	{
		SavedInputHeader header;
		FILE* in = fopen(saveGameName, "rb");
		fread(&header, sizeof(SavedInputHeader), 1, in);
		fclose(in);
		mData = new u8[header.length];
		in = fopen(saveGameName, "rb");
		fread(mData, header.length, 1, in);
		fclose(in);
		mDeleteData = true;
	}


	for (int i = 0; i < (int)sizeof(mHeader); ++i)
	{
		((char*)&mHeader)[i] = mData[i];
	}

	if (memcmp(mHeader.magic, SavedInputsGameMagic, sizeof(SavedInputsGameMagic)) != 0
		&& memcmp(mHeader.magic, SavedInputsReplayMagic, sizeof(SavedInputsReplayMagic)) != 0)
	{
		mPos = 0;
		mHeader.length = 0;
		return;
	}

	mPos += sizeof(SavedInputHeader);
}

SavedInputPlayer::~SavedInputPlayer()
{
	if (mDeleteData)
	{
		delete [] mData;
	}
}

int SavedInputPlayer::GetInt()
{
	int ret;
	u8* r = (u8*)&ret;
	r[0] = mData[mPos + 0];
	r[1] = mData[mPos + 1];
	r[2] = mData[mPos + 2];
	r[3] = mData[mPos + 3];
	mPos += sizeof(int);
	return ret;
}

int SavedInputPlayer::PeekInt()
{
	int ret = GetInt();
	mPos -= sizeof(int);
	return ret;
}

u8 SavedInputPlayer::GetByte()
{
	return mData[mPos++];
}

bool SavedInputPlayer::Update()
{
	if (mPos == mHeader.length) return true;

	assert(mPos < mHeader.length);
	while (mSim->GetCurrentSimTick() == PeekInt())
	{
		GetInt();
		u8 command = GetByte();

		switch (command)
		{
			case SavedInput_StartBuilding:
			{
				u8 player = GetByte();
				assert(player == 0 || player == 1);
				BuildingIndex building = (BuildingIndex)((s8)GetByte());
				u8 x = GetByte();
				u8 y = GetByte();
				V2 location(x, y);
				mSim->StartBuilding(PlayerId(player), building, location);
				break;
			}

			case SavedInput_StartResearch:
			{
				u8 player = GetByte();
				assert(player == 0 || player == 1);
				Technology tech = (Technology)((s8)GetByte());
				mSim->StartResearch(PlayerId(player), tech);
				break;
			}

			case SavedInput_Complete:
				return true;

			default:
				assert(false && "unhandled saved input command");
				break;
		}
	}

	return false;
}

/*static*/ bool SavedInputPlayer::HaveSavedReplay()
{
	return CheckMemoryHeader(SavedInputsReplayMagic) > 0;
}

/*static*/ bool SavedInputPlayer::HaveSavedGame()
{
	return CheckMemoryHeader(SavedInputsGameMagic) > 0;
}

uint32 SavedInputPlayer::GetRandSeed() const
{
	return mHeader.randSeed;
}

GameMode SavedInputPlayer::GetGameMode() const
{
	return mHeader.mode;
}

int SavedInputPlayer::GetScenario() const
{
	return mHeader.scenario;
}

void SavedInputSaver::SaveCurrentTick(int tick, int command)
{
	u8* ct = (u8*)&tick;
	Save(ct[0]);
	Save(ct[1]);
	Save(ct[2]);
	Save(ct[3]);
	assert(command <= 0xff);
	Save((u8)command);
}

void SavedInputSaver::SaveStartBuilding(int player, BuildingIndex buildingIndex, const V2& location)
{
	//assert(buildingIndex < 128);
	assert(player == 0 || player == 1);
	SaveCurrentTick(mSim->GetCurrentSimTick(), SavedInput_StartBuilding);
	Save((u8)player);
	Save((u8)buildingIndex);
	assert(location.x.Int() < 256);
	Save((u8)location.x.Int());
	assert(location.y.Int() < 256);
	Save((u8)location.y.Int());
}

void SavedInputSaver::SaveStartResearch(int player, Technology tech)
{
	//assert(tech < 128);
	assert(player == 0 || player == 1);
	SaveCurrentTick(mSim->GetCurrentSimTick(), SavedInput_StartResearch);
	Save((u8)player);
	Save((u8)tech);
}

void SavedInputSaver::SaveComplete()
{
	SaveCurrentTick(mSim->GetCurrentSimTick(), SavedInput_Complete);
}

SavedInputSaver::~SavedInputSaver()
{
	delete [] mData;
}


bool SavedInputSaver::WriteToSRAM(bool replay)
{
	if (replay) memcpy(mHeader.magic, SavedInputsReplayMagic, sizeof(SavedInputsReplayMagic));
	else memcpy(mHeader.magic, SavedInputsGameMagic, sizeof(SavedInputsGameMagic));
	mHeader.length = mPos + sizeof(SavedInputHeader);

	vuint8* sram = (vuint8*)SRAM;
	for (int i = 0; i < (int)sizeof(SavedInputHeader); ++i)
	{
		sram[i] = ((char*)&mHeader)[i];
	}

	for (int i = 0; i < mPos; ++i)
	{
		sram[i + sizeof(SavedInputHeader)] = mData[i];
	}


	// verify
	for (int i = 0; i < (int)sizeof(SavedInputHeader); ++i)
	{
		if (sram[i] != ((uint8*)&mHeader)[i]) return WriteToFAT(replay);
	}
	for (int i = 0; i < mPos; ++i)
	{
		if (sram[i + sizeof(SavedInputHeader)] != mData[i]) return WriteToFAT(replay);
	}

	return true;
}

bool SavedInputSaver::WriteToFAT(bool replay)
{
	if (replay) memcpy(mHeader.magic, SavedInputsReplayMagic, sizeof(SavedInputsReplayMagic));
	else memcpy(mHeader.magic, SavedInputsGameMagic, sizeof(SavedInputsGameMagic));
	mHeader.length = mPos + sizeof(SavedInputHeader);

	FILE* out = fopen(saveGameName, "wb");
	if (!out) return false;
	fwrite(&mHeader, sizeof(SavedInputHeader), 1, out);
	fwrite(mData, mPos, 1, out);
	fclose(out);

	return true;
}

SavedInputSaver::SavedInputSaver(GameMode mode, int scenario) : mSim(0), mData(new u8[MAX_SAVE_SIZE]), mPos(0)
{
	uint32 seed = time(0);
	mHeader.randSeed = seed;
	mHeader.mode = mode;
	mHeader.scenario = scenario;
}
