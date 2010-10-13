#include "lg/lg.h"
#include "sim.h"
#include "render.h"
#include "object_mgr.h"
#include "memory_config.h"
#include "map.h"
#include "lg/v2.h"
#include "lg/rand.h"
#include "message.h"
#include "tips.h"
#include "sound_effects.h"
#include "saved_inputs.h"
#include "lg/scoped_array.h"
#include <string.h>

using namespace luvafair;

u16 Player::CostOf(BuildingIndex i) const
{
	if (i == BuildingIndex_Crops)
	{
		if (GetHaveTechnology(Tech_Mining)) return 35;
		else return 50;
	}
	else if (i == BuildingIndex_Hut)
	{
		if (GetHaveTechnology(Tech_Mining)) return 50;
		else return 70;
	}
	else if (i == BuildingIndex_House)
	{
		return 750;
	}
	else if (i == BuildingIndex_Farm)
	{
		return 450;
	}
	else if (i == BuildingIndex_School)
	{
		return 1000;
	}
	else if (i == BuildingIndex_Factory)
	{
		return 1400;
	}
	else if (i == BuildingIndex_Apartment)
	{
		return 8500;
	}
	else if (i == BuildingIndex_ReclaimedLand)
	{
		return 2500;
	}
	else if (i == BuildingIndex_University)
	{
		return 9000;
	}
	else if (i == BuildingIndex_FoodImport)
	{
		return 5000;
	}
	else if (i == BuildingIndex_Hospital)
	{
		return 12000;
	}
	else if (i == BuildingIndex_WarBoat)
	{
		return 1;
	}
	else if (i == BuildingIndex_Missiles)
	{
		return 40000;
	}
	return 65535;
}

static V2 ToWorld(const V2& square)
{
	return square * LG_ART_SQUARE_SIZE + (LG_ART_SQUARE_SIZE/2);
}

static V2 ToMap(const V2& pixels)
{
	return ((pixels - (LG_ART_SQUARE_SIZE/2)) / LG_ART_SQUARE_SIZE).TruncateToInt();
}

bool Player::IsAvailable(BuildingIndex bi) const
{
	bool building = GetCurrentBuilding() != BuildingIndex_Invalid;
	if (bi == BuildingIndex_Hut || bi == BuildingIndex_Crops) return !building;
	else if (bi == BuildingIndex_House) return !building && GetHaveTechnology(Tech_Construction);
	else if (bi == BuildingIndex_Apartment) return !building && GetHaveTechnology(Tech_Advanced_Construction);
	else if (bi == BuildingIndex_Farm) return !building && GetHaveTechnology(Tech_Farming);
	else if (bi == BuildingIndex_FoodImport) return !building && GetHaveTechnology(Tech_Trade_Routes);
	else if (bi == BuildingIndex_Missiles) return !building && GetHaveTechnology(Tech_Fusion);
	else if (bi == BuildingIndex_WarBoat) return !building && GetHaveTechnology(Tech_Compass) && !IsAtWar();
	else if (bi == BuildingIndex_Factory) return !building && GetHaveTechnology(Tech_Fossil_Fuels);
	else if (bi == BuildingIndex_Hospital) return !building && GetHaveTechnology(Tech_Medicine);
	else if (bi == BuildingIndex_School) return !building && GetHaveTechnology(Tech_Alphabet);
	else if (bi == BuildingIndex_University) return !building && GetHaveTechnology(Tech_Education);
	else if (bi == BuildingIndex_Bulldozer) return true;
	else if (bi == BuildingIndex_ReclaimedLand) return !building && GetHaveTechnology(Tech_Engineering);
	else if (bi == BuildingIndex_Cancel) return building;
	assert(false && "unhandled building availability");
	return false;
}

static V2NoCtor DockSquares[] =
{
	{ 4, 8 },
	{ 16, 7 }
};

static V2NoCtor WarPath[] =
{
	{ 4, 8 },
	{ 2, 8 },
	{ 2, 11 },
	{ 14, 11 },
	{ 14, 7 },
	{ 16, 7 },
	
};

bool Player::IsAtWar() const
{
	return !mWarBoat->IsHidden();
}

void Player::StartWar()
{
	assert(!IsAtWar());
	mWarBoat->pos = ToWorld(DockSquares[mIndex]);
	mWarBoat->Show();
	mWarBoatTargetMoveIndex = mIndex == 0 ? 0 : ARRAYSIZE(WarPath) - 1;
	mWarBoatTargetMoveIndexDelta = mIndex == 0 ? 1 : -1;
}

const char *NameOfShort(BuildingIndex i)
{
	switch (i)
	{
		case BuildingIndex_Hut: return "Hut";
		case BuildingIndex_House: return "House";
		case BuildingIndex_Apartment: return "Apt";
		case BuildingIndex_Crops: return "Crops";
		case BuildingIndex_Farm: return "Farm";
		case BuildingIndex_FoodImport: return "Fd Imp";
		case BuildingIndex_Missiles: return "Missile";
		case BuildingIndex_WarBoat: return "War";
		case BuildingIndex_Factory: return "Fctry";
		case BuildingIndex_Hospital: return "Hosp";
		case BuildingIndex_School: return "Sch";
		case BuildingIndex_University: return "Univ";
		case BuildingIndex_Bulldozer: return "Destroy";
		case BuildingIndex_ReclaimedLand: return "Reclm";
		case BuildingIndex_Cancel: return "Cancel";
		default: return "???";
	}
}

const char *NameOf(BuildingIndex i)
{
	switch (i)
	{
		case BuildingIndex_Hut: return "Hut";
		case BuildingIndex_House: return "House";
		case BuildingIndex_Apartment: return "Apartment";
		case BuildingIndex_Crops: return "Crops";
		case BuildingIndex_Farm: return "Farm";
		case BuildingIndex_FoodImport: return "Food Import";
		case BuildingIndex_Missiles: return "Missiles";
		case BuildingIndex_WarBoat: return "War Boat";
		case BuildingIndex_Factory: return "Factory";
		case BuildingIndex_Hospital: return "Hospital";
		case BuildingIndex_School: return "School";
		case BuildingIndex_University: return "University";
		case BuildingIndex_Bulldozer: return "Bulldozer";
		case BuildingIndex_ReclaimedLand: return "Reclaimed Land";
		case BuildingIndex_Cancel: return "Cancel";
		default: return "???";
	}
}

Player::Player()
	: mMoney(100)
	, mPopulation(0)
	, mHappiness(1000)
	, mNumShips(0)
	, mCurrentResearch(Tech_Invalid)
	, mCurrentResearchRequired(0)
	, mAverageHappy(0)
	, mHappyVelocity(0)
	, mProduction(0)
	, mKilledOffPeopleByStarving(false)
	, mKilledOffPeopleByHomeless(false)
	, mKilledOffPeopleBySchooling(false)
	, mMissileLaunchCount(60)
{
	memset(mTechnologies, 0, sizeof(mTechnologies));
	mTechnologies[Tech_Compass] = 1;
}

void Player::Init(int index, ObjectMgr& om, Render& render)
{
	mIndex = index;
	mWarBoat = om.Add(V2(0,0), render.CreatePirate(), true);
	mWarBoat->Hide();
}

void Player::AddShip(WorldObject* ship)
{
	assert(mNumShips < MAX_SHIPS);
	mShips[mNumShips++] = ship;
}

int Player::TappedOnShip(const V2& touchPos) const
{
	for (int i = 0; i < mNumShips; ++i)
	{
		if (Abs(mShips[i]->pos.x - touchPos.x) < mShips[i]->GetSpriteInstance()->GetData()->GetWidth() / 2
				&& Abs(mShips[i]->pos.y - touchPos.y) < mShips[i]->GetSpriteInstance()->GetData()->GetHeight() / 2)
		{
			return i;
		}
	}

	return -1;
}

void Player::Research()
{
	if (mCurrentResearch != Tech_Invalid)
	{
		F32 prod = mProduction;
		if (GetCurrentBuilding() != BuildingIndex_Invalid)
			prod /= 2;

		if (GetHaveTechnology(Tech_Mathematics))
		{
			mProductionRBonus = prod * F32(.5f);
			prod *= F32(1.5f);
		}
		if (mCurrentResearchRequired > 0)
		{
			mCurrentResearchRequired -= prod;
		}
		else
		{
			SetHaveTechnology(mCurrentResearch);
			mCurrentResearch = Tech_Invalid;
		}
	}
}

Buildings::Buildings(Map& map) : mMap(map)
{
	mBuildings = new Building[map.SquaresWide() * map.SquaresHigh()];
}

Buildings::~Buildings()
{
	delete [] mBuildings;
}

int Buildings::Sum(BuildingIndex lookFor, int owner, int add) const
{
	int sum = 0;

	for (int y = 0; y < mMap.SquaresHigh(); ++y)
	{
		for (int x = 0; x < mMap.SquaresWide(); ++x)
		{
			Building& b = mBuildings[y * mMap.SquaresWide() + x];
			if (b.mOwner == owner && b.mType == lookFor)
			{
				sum += add;
			}
		}
	}

	return sum;
}

int Buildings::NumBuildings(int player) const
{
	int count = 0;

	for (int y = 0; y < mMap.SquaresHigh(); ++y)
	{
		for (int x = 0; x < mMap.SquaresWide(); ++x)
		{
			Building& b = mBuildings[y * mMap.SquaresWide() + x];
			if (b.mOwner == player) ++count;
		}
	}

	return count;
}

void Buildings::Add(int owner, BuildingIndex building, const V2& loc, int currentRound)
{
	Building& b = mBuildings[loc.y.Int() * mMap.SquaresWide() + loc.x.Int()];
	b.mOwner = owner;
	b.mType = building;
	b.mBuiltOn = currentRound;
	mMap.SetBuilding(loc.x.Int(), loc.y.Int(), building);
}

void Buildings::Destroy(const V2& loc, BuildingIndex destroyed)
{
	Building& b = mBuildings[loc.y.Int() * mMap.SquaresWide() + loc.x.Int()];
	b.mOwner = -1;
	b.mType = BuildingIndex_Destroyed;
	b.mBuiltOn = -1;
	mMap.SetBuilding(loc.x.Int(), loc.y.Int(), destroyed);
}

BuildingIndex Buildings::At(const V2& location, int* owner, int* builtOn) const
{
	assert(location.x.Int() >= 0 && location.x.Int() < mMap.SquaresWide());
	assert(location.y.Int() >= 0 && location.y.Int() < mMap.SquaresHigh());
	Building* b = &mBuildings[location.y.Int() * mMap.SquaresWide() + location.x.Int()];
	if (owner) *owner = b->mOwner;
	if (builtOn) *builtOn = b->mBuiltOn;
	return b->mType;
}

void WanderingWorldObject::SetWO(WorldObject* wo)
{
	mWO = wo;
	Hide();
}

void WanderingWorldObject::Show()
{
	mWO->Show();
	mVisible = true;
}

void WanderingWorldObject::Hide()
{
	mWO->Hide();
	mVisible = false;
}


Sim::Sim(Map& map, ObjectMgr& om, Render& render, MessageDisplay& md, Tips& tips, Rand& rand, SimSerializer* saveInputs)
	: mMap(map)
	, mObjectMgr(om)
	, mRender(render)
	, mMessageDisplay(md)
	, mTips(tips)
	, mCurrentTick(0)
	, mBuildings(map)
	, mRound(0)
	, mTimePerRound(60 * 40)
	, mRemainingTime(mTimePerRound)
	, mTornado(mObjectMgr.Add(V2(0,0), mRender.CreateTornado(), true))
	, mRand(rand)
	, mSaveInputs(saveInputs)
	, mWinningPlayer(-1)
	, mWinType(WinType_Invalid)
{
	mTornado->GetSpriteInstance()->SetMatrix(&mTornadoRotation);
	mTornado->Hide();

	mClouds[0].SetWO(mObjectMgr.Add(V2(0,0), mRender.CreateRainCloud(), true));
	mClouds[1].SetWO(mObjectMgr.Add(V2(0,0), mRender.CreateRainCloud(), true));
	mClouds[2].SetWO(mObjectMgr.Add(V2(0,0), mRender.CreateRainCloud(), true));
	mClouds[3].SetWO(mObjectMgr.Add(V2(0,0), mRender.CreateStormCloud(), true));
	mClouds[4].SetWO(mObjectMgr.Add(V2(0,0), mRender.CreateStormCloud(), true));

	for (int i = 0; i < ARRAYSIZE(mPlayers); ++i)
	{
		mPlayers[i].Init(i, mObjectMgr, mRender);
	}

	mTips.Add(Tip_Welcome1);
	mTips.Add(Tip_Welcome2);
	mTips.Add(Tip_Welcome3);
}

int Sim::NumWatersAdjacent(const V2& pos)
{
	int count = 0;
	if ((mMap.Flags(pos + V2(-1, 0)) & Map::Flag_Water) && At(pos) != BuildingIndex_ReclaimedLand) ++count;
	if ((mMap.Flags(pos + V2(1, 0)) & Map::Flag_Water) && At(pos) != BuildingIndex_ReclaimedLand) ++count;
	if ((mMap.Flags(pos + V2(0, 1)) & Map::Flag_Water) && At(pos) != BuildingIndex_ReclaimedLand) ++count;
	if ((mMap.Flags(pos + V2(0, -1)) & Map::Flag_Water) && At(pos) != BuildingIndex_ReclaimedLand) ++count;
	return count;
}

void Sim::ExpireCrops()
{
	for (int y = 0; y < mMap.SquaresHigh(); ++y)
	{
		for (int x = 0; x < mMap.SquaresWide(); ++x)
		{
			V2 p(x, y);
			int owner;
			int builton;
			if (mBuildings.At(p, &owner, &builton) == BuildingIndex_Crops
					&& mRound - builton > 1)
			{
				if (mPlayers[owner].GetHaveTechnology(Tech_Irrigation)) continue;

				int factor = 5;
				factor *= 1 + NumWatersAdjacent(p);
				if (mRand.Get() % factor == 0)
				{
					mBuildings.Destroy(p, BuildingIndex_ExpiredCrops);
					if (owner == 0) // todo; player
						mTips.Add(Tip_CropsNearWater);
				}
			}
		}
	}
}

void Sim::EndRound()
{
	if (mRemainingTime == 0)
	{
		ExpireCrops();

		++mRound;
		mRemainingTime = mTimePerRound;

		if (mRound == 9)
		{
			mTips.Add(Tip_DoResearch);
		}
	}
}

void Sim::UpdateAnimations()
{
	// todo;
}

template <class T> void Sort(T* array, int size)
{
	for (int i = 0; i < size; ++i)
	{
		for (int j = 0; j < i; ++j)
		{
			if (array[i] < array[j])
			{
				T temp = array[j];
				array[j] = array[i];
				array[i] = temp;
			}
		}
	}
}

int Sim::FindNumberOfHousing(int player) const
{
	return
		mBuildings.Sum(BuildingIndex_Hut, player, 1 * (mPlayers[player].GetHaveTechnology(Tech_Masonry) ? 135 : 100)) + 
		mBuildings.Sum(BuildingIndex_House, player, 12 * 100) + 
		mBuildings.Sum(BuildingIndex_Apartment, player, 100 * 100);
}

int Sim::FindNumberOfFood(int player) const
{
	int ret =
		mBuildings.Sum(BuildingIndex_Crops, player, 1 * (mPlayers[player].GetHaveTechnology(Tech_Agriculture) ? 50 : 40)) +
		mBuildings.Sum(BuildingIndex_Farm, player, 10 * (mPlayers[player].GetHaveTechnology(Tech_Irrigation) ? 45 : 35)) +
		mBuildings.Sum(BuildingIndex_FoodImport, player, 100 * 40);
	return ret;
}

int Sim::FindNumberOfSchooling(int player) const
{
	return
		mBuildings.Sum(BuildingIndex_School, player, 1500) +
		mBuildings.Sum(BuildingIndex_University, player, mPlayers[player].GetPopulation())
		+ 1000;
}

int Sim::FindNumberOfFactories(int player) const
{
	return mBuildings.Sum(BuildingIndex_Factory, player, 2200);
}

int Sim::FindNumberOfFoodImports(int player) const
{
	return mBuildings.Sum(BuildingIndex_FoodImport, player, 1);
}

const F32 TIME_SCALE = F32(1)/F32(250);

void Sim::UpdateHappiness()
{
	if (mCurrentTick % 30 != 0) return;

	for (int i = 0; i < ARRAYSIZE(mPlayers); ++i)
	{
		F32 supportedFood = FindNumberOfFood(i);
		// todo; polluted housing
		F32 supportedHousing = FindNumberOfHousing(i);
		F32 supportedSchooling = FindNumberOfSchooling(i);

		F32 pop = mPlayers[i].GetPopulation();
		F32 factories = Min(pop, F32(FindNumberOfFactories(i)));

		F32 h = (Min(supportedFood, pop) + Min(supportedHousing, pop)) / pop;

		F32 animalHusbandryBonus = F32(mBuildings.Sum(BuildingIndex_Crops, i, mPlayers[i].GetHaveTechnology(Tech_Animal__Husbandry) ? 1 : 0)) / F32(400);
		F32 wheelBonus = GetHaveTechnology(PlayerId(i), Tech_Wheel) ? F32(0.025f) : 0;
		mPlayers[i].mProduction = F32(0.05f) + (pop / F32(10000)) + animalHusbandryBonus + (factories / F32(17000)) + wheelBonus;

		F32 numFoodImports = FindNumberOfFoodImports(i);
		mPlayers[i].mProductionUpkeep = numFoodImports / F32(10);

		mPlayers[i].mProduction -= mPlayers[i].mProductionUpkeep;

		//mPlayers[i].mProduction = 50;

		// random speed up tuning factor
		mPlayers[i].mProduction *= F32(1.3f);

		mPlayers[i].mAverageHappy += mPlayers[i].mHappyVelocity;
		if (mPlayers[i].mAverageHappy < 0) mPlayers[i].mAverageHappy = 0;

		F32 hv = (h - mPlayers[i].mAverageHappy) / F32(40);
		if (hv > 0)
		{
			mPlayers[i].mHappyVelocity += F32(0.005f);
		}
		else
		{
			mPlayers[i].mHappyVelocity = hv;
		}

		if (mPlayers[i].mHappyVelocity > F32(0.1f)) mPlayers[i].mHappyVelocity = F32(.1f);
		if (mPlayers[i].mHappyVelocity < -F32(0.1f)) mPlayers[i].mHappyVelocity = -F32(.1f);
		if (supportedFood < pop)
		{
			mMessageDisplay.Add(i, MessageType_UnhappyFood);
			mPlayers[i].mHappyVelocity = -F32(.025f);
			mPlayers[i].mKilledOffPeopleByStarving = true;
		}
		if (supportedHousing < pop)
		{
			mMessageDisplay.Add(i, MessageType_UnhappyHousing);
			mPlayers[i].mHappyVelocity = -F32(.025f);
			mPlayers[i].mKilledOffPeopleByHomeless = true;
		}
		if (supportedSchooling < pop)
		{
			mMessageDisplay.Add(i, MessageType_RequireSchool);
			mPlayers[i].mHappyVelocity = -F32(.025f);
			mPlayers[i].mKilledOffPeopleBySchooling = true;
			if (i == 0) // todo; player
				mTips.Add(Tip_School);
		}


		if (mPlayers[i].mKilledOffPeopleByStarving || mPlayers[i].mKilledOffPeopleByHomeless || mPlayers[i].mKilledOffPeopleBySchooling)
		{
			mPlayers[i].mAverageHappy = Min(mPlayers[i].mAverageHappy, F32(1));
		}
	}
}

static int PopFactor(int curPop)
{
	return 4 * (1 + (curPop / 400));
}

void Sim::UpdatePopulation()
{
	if (mCurrentTick % 67 == 0) // keep out of sync with other updates
	{
		for (int i = 0; i < ARRAYSIZE(mPlayers); ++i)
		{
			if (mPlayers[i].mAverageHappy > F32(1.3f))
			{
				if (mPlayers[i].IsAtWar())
				{
					mMessageDisplay.Add(i, MessageType_AtWarNoPopulation);
					continue;
				}
				F32 popFactor = PopFactor(mPlayers[i].GetPopulation());
				if (mPlayers[i].GetHaveTechnology(Tech_Religion))
				{
					popFactor *= F32(1.5f);
				}
				mPlayers[i].AddPopulation(mRand.Get() % popFactor.Int());
			}
			else if (mPlayers[i].mAverageHappy < F32(.6f))
			{
				mPlayers[i].SubtractPopulation(mRand.Get() % PopFactor(mPlayers[i].GetPopulation()));
			}
		}
	}
}

bool Sim::IsOffMap(WorldObject* wo) const
{
	return (wo->pos.x.Int() > (mMap.SquaresWide() * LG_ART_SQUARE_SIZE) + wo->GetSpriteInstance()->GetData()->GetWidth() / 2
			|| wo->pos.y.Int() > (mMap.SquaresHigh() * LG_ART_SQUARE_SIZE) + wo->GetSpriteInstance()->GetData()->GetHeight() / 2
			|| wo->pos.x.Int() < -wo->GetSpriteInstance()->GetData()->GetWidth() / 2
			|| wo->pos.y.Int() < -wo->GetSpriteInstance()->GetData()->GetHeight() / 2);
}

bool Sim::IsInMap(const V2& pos) const
{
	return (pos.x.Int() >= 0
			&& pos.x.Int() < mMap.SquaresWide()
			&& pos.y.Int() >= 0
			&& pos.y.Int() < mMap.SquaresHigh());
}

void Sim::DoTornado()
{
	if (mRound > 3 && !mTornadoIsMoving && mRand.Get() % (60 * 60 * 6) == 0)
	{
		mTornadoIsMoving = true;
		mTornado->pos = ToMap(V2(4, 0));
		mTornadoDelta = V2(mRand.Getf() - F32(.25f), mRand.Getf() - F32(.25f));
		mTornado->Show();
		mTornadoAngle = 0;
	}

	if (mTornadoIsMoving)
	{
		if (mRand.Get() % 60 == 0)
		{
			mTornadoDelta = V2(mRand.Getf() - F32(.25f), mRand.Getf() - F32(.25f));
		}
		mTornado->pos += mTornadoDelta;
		mTornadoRotation.SetRotation(mTornadoAngle);
		mTornadoAngle += mRand.Get() % 5 + 30;
		if (IsOffMap(mTornado))
		{
			mTornadoIsMoving = false;
		}

		V2 mp = ToMap(mTornado->pos);
		if (IsInMap(mp))
		{
			int owner;
			if (mBuildings.At(ToMap(mTornado->pos), &owner) != BuildingIndex_Invalid)
			{
				if (!mPlayers[owner].GetHaveTechnology(Tech_Tornado__Resistance))
				{
					if (mRand.Get() % 30 == 0)
					{
						mBuildings.Destroy(ToMap(mTornado->pos));
						Play(Sound_Destroy);
						if (owner == 0) // todo; player
							mTips.Add(Tip_AboutTornados);
					}
				}
			}
		}
	}

}

void Sim::ActivateRandomCloud()
{
	for (int i = 0; i < ARRAYSIZE(mClouds); ++i)
	{
		if (!mClouds[i].IsVisible())
		{
			int x = mRand.Get() % mMap.SquaresWide();
			int y = mRand.Get() % mMap.SquaresHigh();
			mClouds[i].GetWO()->pos = ToWorld(V2(x, y));
			const F32 BIAS(.25f);
			if (x > mMap.SquaresWide() / 2 && y > mMap.SquaresHigh() / 2)
			{
				mClouds[i].SetMoveBias(V2(-BIAS, -BIAS));
			}
			else if (x > mMap.SquaresWide() / 2)
			{
				mClouds[i].SetMoveBias(V2(-BIAS, BIAS));
			}
			else if (y > mMap.SquaresHigh() / 2)
			{
				mClouds[i].SetMoveBias(V2(BIAS, -BIAS));
			}
			else
			{
				mClouds[i].SetMoveBias(V2(BIAS, BIAS));
			}
			mClouds[i].Show();
			return;
		}
	}
}

void Sim::DoClouds()
{
	// keep on average 3 active at a time, with a bit of a lag so that it's not always 3

	int count = 0;
	for (int i = 0; i < ARRAYSIZE(mClouds); ++i)
	{
		if (mClouds[i].IsVisible())
		{
			++count;
		}
	}

	if (count < 3 && mRand.Get() % (60 * 30) == 0)
	{
		ActivateRandomCloud();
	}
	else if (count < 4 && mRand.Get() % (60 * 60) == 0)
	{
		ActivateRandomCloud();
	}
	else if (count < 5 && mRand.Get() % (60 * 120) == 0)
	{
		ActivateRandomCloud();
	}

	const F32 scale(.3333f);
	for (int i = 0; i < ARRAYSIZE(mClouds); ++i)
	{
		if (mClouds[i].IsVisible())
		{
			if (mRand.Get() % 60 == 0)
			{
				V2 d(mRand.Getf() - F32(.5f), mRand.Getf() - F32(.5f));
				d += mClouds[i].GetMoveBias();
				d *= scale;
				mClouds[i].SetDelta(d);
			}
			mClouds[i].GetWO()->pos += mClouds[i].GetDelta();
			V2 mapPos = ToMap(mClouds[i].GetWO()->pos);
			int owner = -1;
			if (IsInMap(mapPos) && mBuildings.At(mapPos, &owner) == BuildingIndex_Crops)
			{
				if (mClouds[i].cropCount > 0)
				{
					--mClouds[i].cropCount;
				}
				else
				{
					mPlayers[owner].Reimburse(1);
					mClouds[i].cropCount = 120;
				}
			}

			if (IsOffMap(mClouds[i].GetWO()))
			{
				mClouds[i].Hide();
			}
		}
	}
}

static int MapDeltaToBoatAnimation(const V2& delta)
{
	if (Abs(delta.y) > Abs(delta.x))
	{
		if (delta.y > 0) return 1;
		else return 0;
	}
	else
	{
		if (delta.x > 0) return 3;
		else return 2;
	}
}

BuildingIndex Player::Construct(V2& loc)
{
	BuildingIndex ret = BuildingIndex_Invalid;

	if (GetCurrentBuilding() != BuildingIndex_Invalid)
	{
		F32 prod = mProduction;
		if (GetCurrentResearch() != Tech_Invalid)
			prod /= 2;
		if (GetHaveTechnology(Tech_Machinery))
		{
			mProductionBBonus = prod * F32(.2f);
			prod *= F32(1.4f);
		}
		mConstruction.mCurrentMoney += prod;
		F32 cost = CostOf(mConstruction.mBuilding);
		if (mConstruction.mCurrentMoney >= cost)
		{
			mConstruction.mCurrentMoney = cost;
			loc = mConstruction.mLocation;
			ret = mConstruction.mBuilding;
			mConstruction.mBuilding = BuildingIndex_Invalid;
		}
		mConstruction.mPercentage = mConstruction.mCurrentMoney / cost * 100;
	}

	return ret;
}

void Sim::MoveShips()
{
	for (int i = 0; i < ARRAYSIZE(mPlayers); ++i)
	{
		for (int j = 0; j < mPlayers[i].NumShips(); ++j)
		{
			if (mPlayers[i].GetShipHasTarget(j))
			{
				WorldObject* ship = mPlayers[i].GetShip(j);
				V2 target = mPlayers[i].GetShipTarget(j);
				V2 direction = (target - ship->pos).Normalize();
				V2 delta = direction * F32(.35f);
				V2 mappos = ToMap(ship->pos + delta);
				ship->GetSpriteInstance()->SetAnimation(MapDeltaToBoatAnimation(delta));
				if (mMap.Flags(mappos) & Map::Flag_Water)
				{
					ship->pos += delta;
					if ((ship->pos - target).LengthSq() < 2)
					{
						mPlayers[i].SetShipHasTarget(j, false);
					}
				}
			}
		}
	}
}

bool Sim::FindRandomBuilding(int player, V2& location)
{
	ScopedArray<V2> candidates(new V2[mMap.SquaresWide() * mMap.SquaresHigh()]);
	int numCandidates = 0;
	for (int x = 0; x < mMap.SquaresWide(); ++x)
	{
		for (int y = 0; y < mMap.SquaresHigh(); ++y)
		{
			V2 pos = V2(x, y);
			if (At(pos, PlayerId(player)) != BuildingIndex_Invalid)
			{
				candidates[numCandidates++] = pos;
			}
		}
	}

	if (numCandidates == 0) return false;

	location = candidates[mRand.Get() % numCandidates];
	return true;
}

void Sim::MoveWarBoats()
{
	for (int i = 0; i < ARRAYSIZE(mPlayers); ++i)
	{
		if (mPlayers[i].IsAtWar())
		{
			WorldObject* wo = mPlayers[i].GetWarBoat();
			V2 target = ToWorld(WarPath[mPlayers[i].mWarBoatTargetMoveIndex]);
			V2 direction = (target - wo->pos).Normalize();
			V2 delta = direction * F32(.2f);
			V2 mappos = ToMap(wo->pos + delta);
			wo->GetSpriteInstance()->SetAnimation(MapDeltaToBoatAnimation(delta));
			wo->pos += delta;
			if ((wo->pos - target).LengthSq() < 2)
			{
				mPlayers[i].mWarBoatTargetMoveIndex += mPlayers[i].mWarBoatTargetMoveIndexDelta;
				if (mPlayers[i].mWarBoatTargetMoveIndex < 0 || mPlayers[i].mWarBoatTargetMoveIndex >= ARRAYSIZE(WarPath))
				{
					Play(Sound_War);
					wo->Hide();

					// attacker_pop / (attacker_pop + defender_pop) % chance of destroying up to N buildings
					// cost is that attacker's population doesn't grow while boat is travelling
					// todo; perhaps some tech related thing? stealing a tech? un-researching a tech?
					int other = i ^ 1;
					F32 chance = F32(mPlayers[i].GetPopulation()) / F32(mPlayers[i].GetPopulation() + mPlayers[other].GetPopulation());
					if (mPlayers[other].GetHaveTechnology(Tech_Metal_Work))
					{
						chance -= F32(.1f);
					}
					int numBuildings = 6;
					if (mPlayers[i].GetHaveTechnology(Tech_War_Boats)) numBuildings *= 2;
					for (int i = 0; i < numBuildings; ++i)
					{
						if (mRand.Getf() < chance)
						{
							V2 location;
							if (FindRandomBuilding(other, location))
							{
								mBuildings.Destroy(location);
							}
						}
					}
				}
			}
		}
	}
}


void Sim::DoResearchAndBuilding()
{
	for (int i = 0; i < ARRAYSIZE(mPlayers); ++i)
	{
		V2 loc;
		BuildingIndex bi = mPlayers[i].Construct(loc);
		if (bi != BuildingIndex_Invalid)
		{
			mBuildings.Add(i, bi, loc, mRound);
			if (!mTips.HaveDisplayed(Tip_TechUp) && mBuildings.NumBuildings(0) >= 20) // todo; player
			{
				mTips.Add(Tip_TechUp);
			}
			if (mPlayers[i].mKilledOffPeopleByStarving &&
					(bi == BuildingIndex_Crops ||
					 bi == BuildingIndex_Farm ||
					 bi == BuildingIndex_FoodImport))
			{
				mPlayers[i].mKilledOffPeopleByStarving = false;
			}
			if (mPlayers[i].mKilledOffPeopleByHomeless &&
					(bi == BuildingIndex_Hut ||
					 bi == BuildingIndex_House ||
					 bi == BuildingIndex_Apartment))
			{
				mPlayers[i].mKilledOffPeopleByHomeless = false;
			}
			if (mPlayers[i].mKilledOffPeopleBySchooling &&
					(bi == BuildingIndex_School ||
					 bi == BuildingIndex_University))
			{
				mPlayers[i].mKilledOffPeopleBySchooling = false;
			}
		}
		mPlayers[i].Research();
	}
}

void Sim::DoMissiles()
{
	for (int i = 0; i < ARRAYSIZE(mPlayers); ++i)
	{
		if (mPlayers[i].GetHaveTechnology(Tech_Fusion) && mBuildings.Sum(BuildingIndex_Missiles, i, 1) > 0)
		{
			int other = i ^ 1;
			mMessageDisplay.Add(other, MessageType_MissileAttacked);
			mMessageDisplay.Add(i, MessageType_MissileAttacking);
			if (mPlayers[i].mMissileLaunchCount-- <= 0)
			{
				V2 location;
				if (FindRandomBuilding(other, location))
				{
					mBuildings.Destroy(location);
					Play(Sound_Destroy);
				}
				else
				{
					mWinningPlayer = i;
					mWinType = WinType_Missiles;
				}
				mPlayers[i].mMissileLaunchCount = 60;
			}
		}
	}
}

void Sim::CheckForPopulationWin()
{
	for (int i = 0; i < ARRAYSIZE(mPlayers); ++i)
	{
		if (mPlayers[i].GetPopulation() > 100000)
		{
			mWinningPlayer = i;
			mWinType = WinType_Population;
		}
	}
}

void Sim::Update()
{
	++mCurrentTick;

	DoTornado();
	DoClouds();
	MoveShips();
	MoveWarBoats();
	DoResearchAndBuilding();
	DoMissiles();
	CheckForPopulationWin();

	if (!mTips.HaveDisplayed(Tip_FirstBuildings)
			&& FindNumberOfFood(0) > 0 && FindNumberOfHousing(0) > 0) // todo; player
	{
		mTips.Add(Tip_FirstBuildings);
		mTips.Add(Tip_InterfaceMove);
	}

	UpdateAnimations();

	--mRemainingTime;

	UpdateHappiness();
	UpdatePopulation();

	EndRound();
}

int Sim::TappedOnBoat(PlayerId player, const V2& location) const
{
	return mPlayers[player].TappedOnShip(location);
}

void Sim::GetCurrentValues(PlayerId player, F32* food, F32* housing, F32* pop, F32* prod, F32* happy, F32* prodRbonus, F32* prodBbonus, F32* prodUpenalty) const
{
	if (food) *food = FindNumberOfFood(player);
	if (housing) *housing = FindNumberOfHousing(player);
	if (pop) *pop = mPlayers[player].GetPopulation();
	if (prod) *prod = mPlayers[player].mProduction;
	if (happy) *happy = mPlayers[player].mAverageHappy;
	if (prodRbonus) *prodRbonus = mPlayers[player].mProductionRBonus;
	if (prodBbonus) *prodBbonus = mPlayers[player].mProductionBBonus;
	if (prodUpenalty) *prodUpenalty = mPlayers[player].mProductionUpkeep;
}

void Sim::GetKilledOff(PlayerId player, bool* food, bool* housing, bool* school) const
{
	if (food) *food = mPlayers[player].mKilledOffPeopleByStarving;
	if (housing) *housing = mPlayers[player].mKilledOffPeopleByHomeless;
	if (school) *school = mPlayers[player].mKilledOffPeopleBySchooling;
}

bool Sim::CanBuy(PlayerId player, BuildingIndex buildingIndex) const
{
	return mPlayers[player].IsAvailable(buildingIndex);
}


void Sim::StartBuilding(PlayerId player, BuildingIndex buildingIndex, const V2& location)
{
	if (mSaveInputs)
	{
		mSaveInputs->SaveStartBuilding(player, buildingIndex, location);
	}

	if (buildingIndex == BuildingIndex_Bulldozer)
	{
		mBuildings.Destroy(location);
		Play(Sound_Destroy, player);
	}
	else if (buildingIndex == BuildingIndex_WarBoat)
	{
		Play(Sound_War);
		mPlayers[player].StartWar();
	}
	else
	{
		mPlayers[player].StartBuilding(buildingIndex, location);
		Play(Sound_PlaceBuilding, player);
	}
}

void Player::StartBuilding(BuildingIndex buildingIndex, const V2& location)
{
	mConstruction.mBuilding = buildingIndex;
	mConstruction.mPercentage = 0;
	mConstruction.mCurrentMoney = 0;
	mConstruction.mLocation = location;
}

luvafair::F32 Sim::GetPercentageOfBuildingBuilt(PlayerId player) const
{
	return mPlayers[player].GetPercentageOfBuildingBuilt();
}

bool Sim::GetHaveTechnology(PlayerId player, Technology tech) const
{
	assert(tech != Tech_Invalid && tech < Tech_Count);
	return mPlayers[player].GetHaveTechnology(tech);
}

bool Sim::GetHavePrerequisitesForTechnology(PlayerId player, Technology tech) const
{
	for (int i = 0; i < ARRAYSIZE(gTechInfo[tech].dependents); ++i)
	{
		Technology dep = gTechInfo[tech].dependents[i];
		if (dep != Tech_Invalid && !GetHaveTechnology(player, dep))
		{
			return false;
		}
	}
	return true;
}

void Sim::StartResearch(PlayerId player, Technology tech)
{
	if (mSaveInputs)
	{
		mSaveInputs->SaveStartResearch(player, tech);
	}

	if (tech == Tech_Invalid)
	{
		mPlayers[player].SetCurrentResearch(tech, 0);
	}
	else
	{
		assert(player == 0 || player == 1);
		assert(tech >= 0 && tech < Tech_Count);
		assert(GetHavePrerequisitesForTechnology(player, tech));
		assert(!GetHaveTechnology(player, tech));

		mPlayers[player].SetCurrentResearch(tech, gTechInfo[tech].cost);
	}
}

Technology Sim::CurrentResearch(PlayerId player) const
{
	return mPlayers[player].GetCurrentResearch();
}

F32 Sim::GetPercentageOfResearch(PlayerId player) const
{
	F32 c(gTechInfo[CurrentResearch(player)].cost);
	return (F32(c - mPlayers[player].GetRemainingResearch()) / c) * 100;
}

BuildingIndex Sim::At(const V2& location) const
{
	return mBuildings.At(location);
}

BuildingIndex Sim::At(const V2& location, PlayerId id) const
{
	int player;
	BuildingIndex bi = mBuildings.At(location, &player);
	if (id == player) return bi;
	return BuildingIndex_Invalid;
}

int Sim::SumBuildings(PlayerId player, BuildingIndex building, int per)
{
	return mBuildings.Sum(building, player, per);
}
