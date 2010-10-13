#ifndef LG_INCLUDED_saved_inputs_H
#define LG_INCLUDED_saved_inputs_H

#include "sim.h"
#include "fe.h"

enum SavedInputCommand
{
	SavedInput_StartBuilding,
	SavedInput_StartResearch,
	SavedInput_Complete
};

struct SavedInputHeader
{
	char magic[4];
	int length;
	u32 randSeed;
	GameMode mode;
	int scenario;
};


class SavedInputPlayer
{
	Sim* mSim;
	u8* mData;
	bool mDeleteData;
	int mPos;
	SavedInputHeader mHeader;

	int GetInt();
	int PeekInt();
	u8 GetByte();

	public:
		SavedInputPlayer();
		~SavedInputPlayer();
		bool Update();
		void SetSim(Sim* sim) { mSim = sim; }
		static bool HaveSavedReplay();
		static bool HaveSavedGame();

		uint32 GetRandSeed() const;
		GameMode GetGameMode() const;
		int GetScenario() const;
};

class SavedInputSaver : public SimSerializer
{
	enum
	{
		MAX_SAVE_SIZE = 65536 - sizeof(SavedInputHeader)
	};
	const Sim* mSim;
	u8* mData;
	int mPos;
	SavedInputHeader mHeader;

	void Save(u8 b)
	{
		assert(mPos < MAX_SAVE_SIZE);
		mData[mPos++] = b;
	}

	void SaveCurrentTick(int tick, int command);

	public:

		virtual void SaveStartBuilding(int player, BuildingIndex building, const V2& location);
		virtual void SaveStartResearch(int player, Technology tech);
		virtual void SaveComplete();
		virtual ~SavedInputSaver();
		bool WriteToSRAM(bool replay);
		bool WriteToFAT(bool replay);
		SavedInputSaver(GameMode mode, int scenario);
		void SetSim(const Sim* sim) { mSim = sim; }

		unsigned int GetRandSeed() const { return mHeader.randSeed; }
};

#endif
