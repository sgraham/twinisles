#ifndef LG_INCLUDED_scenario_H
#define LG_INCLUDED_scenario_H

class Sim;
namespace luvafair { class Font; }
#include "lg/string.h"

enum ScenarioResult
{
	ScenarioResult_NotComplete,
	ScenarioResult_Failed,
	ScenarioResult_CompletedGold,
	ScenarioResult_CompletedSilver,
	ScenarioResult_CompletedBronze,
};

class Scenario
{
	public:
		virtual ~Scenario() {}
		virtual ScenarioResult Completed(const Sim& sim) = 0;
		virtual luvafair::String GetDescription(const Sim& sim) const = 0;
		virtual void Render(u16* bg, luvafair::Font& font, const Sim& sim) const = 0;
};

extern Scenario* gScenario;

class ScenarioTimedPopulation : public Scenario
{
	int mPlayer;
	int mPopulation;
	int mTicksGold;
	int mTicksSilver;
	int mTicksBronze;

	int GetTargetTicks(const Sim& sim) const;
	const char* GetTargetMedal(const Sim& sim) const;

	public:
		ScenarioTimedPopulation(int player, int pop, int gold, int silver, int bronze)
			: mPlayer(player)
			, mPopulation(pop)
			, mTicksGold(gold)
			, mTicksSilver(silver)
			, mTicksBronze(bronze)
		{}
		virtual ~ScenarioTimedPopulation() {}

		virtual ScenarioResult Completed(const Sim& sim);
		virtual luvafair::String GetDescription(const Sim& sim) const;
		virtual void Render(u16* bg, luvafair::Font& font, const Sim& sim) const;
};

#endif
