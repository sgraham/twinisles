#include "lg/lg.h"
#include "scenario.h"
#include "sim.h"
#include "lg/font.h"

using namespace luvafair;

Scenario* gScenario;

ScenarioResult ScenarioTimedPopulation::Completed(const Sim& sim)
{
	int tick = sim.GetCurrentSimTick();
	if (tick > mTicksBronze)
	{
		return ScenarioResult_Failed;
	}
	
	F32 pop;
	sim.GetCurrentValues(PlayerId(mPlayer), 0, 0, &pop, 0, 0, 0, 0, 0);
	if (pop > mPopulation)
	{
		if (tick < mTicksGold)
		{
			return ScenarioResult_CompletedGold;
		}
		else if (tick < mTicksSilver)
		{
			return ScenarioResult_CompletedSilver;
		}
		else
		{
			return ScenarioResult_CompletedBronze;
		}
	}

	return ScenarioResult_NotComplete;
}

int ScenarioTimedPopulation::GetTargetTicks(const Sim& sim) const
{
	int tick = sim.GetCurrentSimTick();
	int targetTicks = mTicksBronze;
	if (tick < mTicksGold)
	{
		targetTicks = mTicksGold;
	}
	else if (tick < mTicksSilver)
	{
		targetTicks = mTicksSilver;
	}
	return targetTicks;
}

const char* ScenarioTimedPopulation::GetTargetMedal(const Sim& sim) const
{
	int tick = sim.GetCurrentSimTick();
	const char* type = "Bronze";
	if (tick < mTicksGold)
	{
		type = "Gold";
	}
	else if (tick < mTicksSilver)
	{
		type = "Silver";
	}
	return type;
}

String ScenarioTimedPopulation::GetDescription(const Sim& sim) const
{
	return Format("Grow your population to %0. For a %2 medal, complete in %1.",
			mPopulation,
			TicksAsTime(GetTargetTicks(sim)),
			GetTargetMedal(sim));
}

void ScenarioTimedPopulation::Render(u16* bg, luvafair::Font& font, const Sim& sim) const
{
	int remaining = GetTargetTicks(sim) - sim.GetCurrentSimTick();
	const char* str = GetTargetMedal(sim);
	font.DrawString(bg, (String(str) + ": " + TicksAsTime(remaining)).c_str(), 180 + 16, 100, 0x8000 | RGB15(15, 15, 31), TextAlignment_Centre);
}
