#include "lg/lg.h"
#include "message.h"
#include "lg/font.h"
#include "lg/draw.h"
#include "techtree.h"
#include <cstring>

using namespace luvafair;

MessageDisplay::MessageDisplay() : mCurPos(-1), mTimer(0)
{
	memset(mActiveMessages, 0, sizeof(mActiveMessages));
}

void MessageDisplay::Add(int player, MessageType mt, int aux0)
{
	assert(mt < MessageType_Count);
	mActiveMessages[player][mt] = true;
	mMessageAux[player][mt] = aux0;
}

void MessageDisplay::Clear()
{
	memset(mActiveMessages, 0, sizeof(mActiveMessages));
	mCurPos = -1;
	mTimer = 0;
}

void MessageDisplay::Update(u16* bg, Font& font, int player)
{
	if (mTimer == 0)
	{
		if (mCurPos >= 0)
		{
			mActiveMessages[player][mCurPos] = false;
		}
		int count = 0;
		for (;;)
		{
			++mCurPos;
			if (mCurPos == ARRAYSIZE(mActiveMessages[player])) mCurPos = 0;
			if (mActiveMessages[player][mCurPos])
			{
				mTimer = 5;
				if (mCurPos == MessageType_ResearchComplete)
				{
					mActiveMessages[player][MessageType_ResearchResult] = true;
					mMessageAux[player][MessageType_ResearchResult] = mMessageAux[player][mCurPos];
				}
				break;
			}
			++count;
			if (count == ARRAYSIZE(mActiveMessages[player]))
			{
				mCurPos = -1;
				break;
			}
		}
	}
	
	char buf[256];
	if (mTimer > 0 && mCurPos >= 0)
	{
		u16 colour = RGB15(25, 10, 10);
		const char* str = 0;
		switch (mCurPos)
		{
			case MessageType_UnhappyFood:
				str = "People are starving";
				break;

			case MessageType_UnhappyHousing:
				str = "People are homeless";
				break;

			case MessageType_RequireChurch:
				str = "The people demand a place of worship";
				break;

			case MessageType_RequireSchool:
				str = "The people demand a school";
				break;

			case MessageType_RequireHospital:
				str = "The people demand a hospital";
				break;

			case MessageType_ResearchComplete:
				sprintf(buf, "Research complete: %s", gTechInfo[mMessageAux[player][mCurPos]].name);
				str = buf;
				colour = RGB15(25, 25, 25);
				break;

			case MessageType_AtWarNoPopulation:
				str = "At war: No population growth";
				colour = RGB15(25, 25, 25);
				break;

			case MessageType_MissileAttacking:
				str = "You have launched a missile attack!";
				break;

			case MessageType_MissileAttacked:
				str = "You are under missile attack!";
				break;

			case MessageType_ResearchResult:
				str = gTechInfo[mMessageAux[player][mCurPos]].result;
				colour = RGB15(25, 25, 25);
				break;

			default:
				assert(false && "unhandled message type");
				return;
		}

		DrawFilledRect(bg, 0, 0, SCREEN_WIDTH, 20, 0x8000);
		font.DrawString(bg, str, SCREEN_WIDTH / 2, 4, 0x8000 | colour, TextAlignment_Centre);
		--mTimer;
	}
}
