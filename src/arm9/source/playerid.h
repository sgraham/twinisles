#ifndef LG_INCLUDED_playerid_H
#define LG_INCLUDED_playerid_H

struct PlayerId
{
	int mId;
	explicit PlayerId(int id) : mId(id) {}
	operator int() const { assert(mId == 0 || mId == 1); return mId; }
	PlayerId(const PlayerId& pi) : mId(pi.mId) {}
	void operator=(const PlayerId& rhs) { mId = rhs.mId; }
};

inline bool operator==(const PlayerId& lhs, const int& rhs)
{
	return lhs.mId == rhs;
}

inline bool operator==(const int& lhs, const PlayerId& rhs)
{
	return lhs == rhs.mId;
}

inline bool operator==(const PlayerId& lhs, const PlayerId& rhs)
{
	return lhs.mId == rhs.mId;
}

#endif
