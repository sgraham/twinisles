#ifndef LG_INCLUDED_message_H
#define LG_INCLUDED_message_H

namespace luvafair { class Font; }

enum MessageType
{
	MessageType_UnhappyFood,
	MessageType_UnhappyHousing,

	MessageType_RequireSchool,
	MessageType_RequireChurch,
	MessageType_RequireHospital,

	MessageType_ResearchComplete,
	MessageType_ResearchResult,

	MessageType_AtWarNoPopulation,

	MessageType_MissileAttacking,
	MessageType_MissileAttacked,

	MessageType_Count
};

class MessageDisplay
{
	bool mActiveMessages[2][MessageType_Count];
	int mMessageAux[2][MessageType_Count];
	int mCurPos;
	int mTimer;

	public:
		MessageDisplay();

		void Add(int player, MessageType mt, int aux0 = -1);

		void Update(u16* bg, luvafair::Font& font, int player);

		void Clear();
};

#endif
