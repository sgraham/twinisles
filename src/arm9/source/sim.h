#ifndef INCLUDED_sim_H
#define INCLUDED_sim_H

class Map;
class ObjectMgr;
class Render;
class WorldObject;
class MessageDisplay;
class Tips;
#include "lg/v2.h"
#include "lg/f32.h"
#include "buildings.h"
#include "lg/sprite.h"
#include "techtree.h"
#include "playerid.h"
#include "lg/rand.h"

enum WinType
{
	WinType_Invalid,
	WinType_Missiles,
	WinType_Population,
};

class Construction
{
	BuildingIndex mBuilding;
	luvafair::F32 mPercentage;
	luvafair::F32 mCurrentMoney;
	V2 mLocation;

	Construction()
		: mBuilding(BuildingIndex_Invalid)
		, mPercentage(0)
		, mCurrentMoney(0)
		, mLocation(0,0)
	{
	}

	friend class Player;
};

class Player
{
	enum { MAX_SHIPS = 16 };
	int mIndex;
	luvafair::F32 mMoney;
	int mPopulation;
	luvafair::F32 mHappiness;
	int mNumShips;
	WorldObject* mShips[MAX_SHIPS];
	V2 mShipTarget[MAX_SHIPS];
	bool mShipHasTarget[MAX_SHIPS];
	bool mTechnologies[Tech_Count];
	Technology mCurrentResearch;
	luvafair::F32 mCurrentResearchRequired;
	Construction mConstruction;
	WorldObject* mWarBoat;

	public:
	Player();
	void Init(int index, ObjectMgr& om, Render& render);

	// todo;
	luvafair::F32 mAverageHappy;
	luvafair::F32 mHappyVelocity;
	luvafair::F32 mProduction;
	luvafair::F32 mProductionRBonus;
	luvafair::F32 mProductionBBonus;
	luvafair::F32 mProductionUpkeep;
	bool mKilledOffPeopleByStarving;
	bool mKilledOffPeopleByHomeless;
	bool mKilledOffPeopleBySchooling;
	int mWarBoatTargetMoveIndex;
	int mWarBoatTargetMoveIndexDelta;
	int mMissileLaunchCount;

	luvafair::F32 GetMoney() const { return mMoney; }
	void SetMoney(luvafair::F32 val) { mMoney = val; }

	int GetPopulation() const { return mPopulation; }
	void SetPopulation(int val) { mPopulation = Max(val, 0); }
	void AddPopulation(int val) { mPopulation += val; }
	void SubtractPopulation(int val) { mPopulation -= val; if (mPopulation < 1) mPopulation = 1; }

	luvafair::F32 GetHappiness() const { return mHappiness; }
	void SetHappiness(luvafair::F32 val) { mHappiness = Max(Min(val, luvafair::F32(2000)), luvafair::F32(0)); }

	luvafair::F32 GetProduction() const { return mProduction; }
	void SetProduction(luvafair::F32 val) { mProduction = val; }

	bool CanSpend(luvafair::F32 money) const { return mMoney >= money; }
	void Spend(luvafair::F32 money) { mMoney -= money; }
	void Reimburse(luvafair::F32 money) { mMoney += money; }

	WorldObject* GetShip(int ship) const { assert(ship >=0 && ship < mNumShips); return mShips[ship]; }
	void AddShip(WorldObject* wo);
	int NumShips() const { return mNumShips; }
	void SetShipTarget(int ship, const V2& target) { assert(ship >= 0 && ship < mNumShips); mShipTarget[ship] = target; mShipHasTarget[ship] = true; }
	const V2& GetShipTarget(int ship) const { assert(ship >= 0 && ship < mNumShips); return mShipTarget[ship]; }
	void SetShipHasTarget(int ship, bool has) { assert(ship >= 0 && ship < mNumShips); mShipHasTarget[ship] = has; }
	bool GetShipHasTarget(int ship) const { assert(ship >= 0 && ship < mNumShips); return mShipHasTarget[ship]; }

	int TappedOnShip(const V2& touchPos) const;

	void SetHaveTechnology(Technology tech) { mTechnologies[tech] = true; }
	bool GetHaveTechnology(Technology tech) const { return mTechnologies[tech]; }

	Technology GetCurrentResearch() const { return mCurrentResearch; }
	void SetCurrentResearch(Technology tech, int required) { mCurrentResearch = tech; mCurrentResearchRequired = required; }
	luvafair::F32 GetRemainingResearch() const { return mCurrentResearchRequired; }
	void Research();

	BuildingIndex GetCurrentBuilding() const { return mConstruction.mBuilding; }
	luvafair::F32 GetPercentageOfBuildingBuilt() const { return mConstruction.mPercentage; }
	const V2& GetBuildLocation() const { return mConstruction.mLocation; }

	BuildingIndex Construct(V2& loc);
	void StartBuilding(BuildingIndex, const V2& location);

	u16 CostOf(BuildingIndex i) const;
	bool IsAvailable(BuildingIndex bi) const;

	bool IsAtWar() const;
	void StartWar();
	WorldObject* GetWarBoat() const { return mWarBoat; }
};

class Buildings
{
	struct Building
	{
		u8 mOwner;
		BuildingIndex mType;
		int mBuiltOn;
		Building() : mOwner(0xff), mType(BuildingIndex_Invalid), mBuiltOn(-1) {}
	};

	Building* mBuildings;
	Map& mMap;

	public:
	Buildings(Map& map);
	~Buildings();

	int Sum(BuildingIndex lookFor, int owner, int add) const;

	void Add(int owner, BuildingIndex building, const V2& location, int currentRound);
	void Destroy(const V2& location, BuildingIndex bi = BuildingIndex_Destroyed);
	BuildingIndex At(const V2& location, int* owner = 0, int* builtOn = 0) const;
	int NumBuildings(int player) const;
};

class WanderingWorldObject
{
private:
	WorldObject* mWO;
	V2 mDelta;
	V2 mBias;
	bool mVisible;
	int mFrameCount;
public:
	WanderingWorldObject() : mWO(0), mDelta(0,0), mVisible(false), mFrameCount(0) {}
	void SetWO(WorldObject* wo);
	WorldObject* GetWO() { return mWO; }
	void Show();
	void Hide();
	bool IsVisible() const { return mVisible; }

	void SetDelta(const V2& d) { mDelta = d; } 
	const V2& GetDelta() const { return mDelta; }

	void SetMoveBias(const V2& bias) { mBias = bias; }
	const V2& GetMoveBias() const { return mBias; }
};

struct Cloud : public WanderingWorldObject
{
	int cropCount;

	Cloud() : cropCount(0) {}
};

struct SimSerializer
{
	virtual void SaveStartBuilding(int player, BuildingIndex building, const V2& location) = 0;
	virtual void SaveStartResearch(int player, Technology tech) = 0;
	virtual void SaveComplete() = 0;
	virtual ~SimSerializer() {}
};

class Sim
{
	Map& mMap;
	ObjectMgr& mObjectMgr;
	Render& mRender;
	MessageDisplay& mMessageDisplay;
	Tips& mTips;
	int mCurrentTick;

	Player mPlayers[2];
	Buildings mBuildings;
	int mRound;
	int mTimePerRound;
	int mRemainingTime;

	WorldObject* mTornado;
	luvafair::SpriteMatrix mTornadoRotation;
	V2 mTornadoDelta;
	bool mTornadoIsMoving;
	int mTornadoAngle;
	V2 mWarBoatPosition;

	luvafair::Rand& mRand;
	SimSerializer* mSaveInputs;
	
	int mWinningPlayer;
	WinType mWinType;

	Cloud mClouds[5];

	void EndRound();
	void UpdateAnimations();
	void UpdateHappiness();
	void UpdatePopulation();
	void ExpireCrops();
	int NumWatersAdjacent(const V2& pos);

	int FindNumberOfHousing(int player) const;
	int FindNumberOfFood(int player) const;
	int FindNumberOfSchooling(int player) const;
	int FindNumberOfHospitals(int player) const;
	int FindNumberOfFactories(int player) const;
	int FindNumberOfFoodImports(int player) const;

	bool IsOffMap(WorldObject* wo) const;
	bool IsInMap(const V2& pos) const;

	void DoTornado();

	void ActivateRandomCloud();
	void DoClouds();

	void MoveShips();
	void MoveWarBoats();
	bool FindRandomBuilding(int player, V2& location);

	void DoResearchAndBuilding();
	void DoMissiles();
	void CheckForPopulationWin();

	public:
		Sim(Map& map, ObjectMgr& om, Render& render, MessageDisplay& md, Tips& tips, luvafair::Rand& rand, SimSerializer* saveInputs = 0);

		// these commands need to be saved/networked
		void StartBuilding(PlayerId player, BuildingIndex buidingIndex, const V2& location);
		void StartResearch(PlayerId player, Technology tech);

		// main 60Hz tick
		void Update();

		// data access; don't change state/need to be saved
		bool CanBuy(PlayerId player, BuildingIndex buildingIndex) const;

		void GetCurrentValues(PlayerId player,
				luvafair::F32* food,
				luvafair::F32* housing,
				luvafair::F32* pop,
				luvafair::F32* prod,
				luvafair::F32* happy,
				luvafair::F32* prodRbonus,
				luvafair::F32* prodBbonus,
				luvafair::F32* prodUpenalty
				) const;

		void GetKilledOff(PlayerId player, bool* food, bool* housing, bool* school) const;

		luvafair::F32 GetPercentageOfBuildingBuilt(PlayerId player) const;
		BuildingIndex CurrentBuilding(PlayerId player) const { assert(player == 0 || player == 1); return mPlayers[player].GetCurrentBuilding(); }
		const V2& BuildLocation(PlayerId player) const { assert(CurrentBuilding(player) != BuildingIndex_Invalid); return mPlayers[player].GetBuildLocation(); }

		bool GetHaveTechnology(PlayerId player, Technology tech) const;
		bool GetHavePrerequisitesForTechnology(PlayerId player, Technology tech) const;

		int TappedOnBoat(PlayerId player, const V2& location) const;

		Technology CurrentResearch(PlayerId player) const;
		luvafair::F32 GetPercentageOfResearch(PlayerId player) const;

		BuildingIndex At(const V2& location) const;
		BuildingIndex At(const V2& location, PlayerId id) const;
		int SumBuildings(PlayerId player, BuildingIndex building, int per);
		
		int NumBuildings(PlayerId player) const { return mBuildings.NumBuildings(player); }

		u16 CostOf(PlayerId player, BuildingIndex building) const { return mPlayers[player].CostOf(building); }

		int GetCurrentSimTick() const { return mCurrentTick; }

		int GetWinningPlayer() const { return mWinningPlayer; }
		int GetWinType() const { return mWinType; }
};

const char* NameOf(BuildingIndex i);
const char* NameOfShort(BuildingIndex i);

#endif
