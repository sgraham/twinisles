#ifndef LG_INCLUDED_lg_v2_H
#define LG_INCLUDED_lg_v2_H

#include "lg/lg.h"
#include "lg/f32.h"

class V2NoCtor
{
	public:
		luvafair::F32 x;
		luvafair::F32 y;

	void operator+=(const V2NoCtor& rhs)
	{
		x += rhs.x;
		y += rhs.y;
	}

	void operator-=(const V2NoCtor& rhs)
	{
		x -= rhs.x;
		y -= rhs.y;
	}

	V2NoCtor operator-(const V2NoCtor& rhs) const
	{
		V2NoCtor ret(*this);
		ret -= rhs;
		return ret;
	}

	V2NoCtor operator+(const V2NoCtor& rhs) const
	{
		V2NoCtor ret(*this);
		ret += rhs;
		return ret;
	}

	V2NoCtor operator+(luvafair::F32 d) const
	{
		V2NoCtor ret(*this);
		ret.x += d;
		ret.y += d;
		return ret;
	}

	V2NoCtor operator-(luvafair::F32 d) const
	{
		V2NoCtor ret(*this);
		ret.x -= d;
		ret.y -= d;
		return ret;
	}

	V2NoCtor operator/(luvafair::F32 d) const
	{
		V2NoCtor ret(*this);
		assert(d != 0);
		ret.x /= d;
		ret.y /= d;
		return ret;
	}

	V2NoCtor operator*(luvafair::F32 d) const
	{
		V2NoCtor ret(*this);
		ret.x *= d;
		ret.y *= d;
		return ret;
	}

	void operator*=(luvafair::F32 d)
	{
		x *= d;
		y *= d;
	}

	void operator/=(luvafair::F32 d)
	{
		x /= d;
		y /= d;
	}

	luvafair::F32 LengthSq() const
	{
		luvafair::F32 ret = x * x + y * y;
		return ret;
	}

	luvafair::F32 DistanceSq(const V2NoCtor& other) const
	{
		V2NoCtor diff = other - *this;
		return diff.LengthSq();
	}

	V2NoCtor Normalize() const
	{
		int len = LengthSq().Int();
		if (len <= 1) return *this;
		len = swiSqrt(len);
		V2NoCtor ret(*this);
		ret /= luvafair::F32(len);
		return ret;
	}

	luvafair::F32 Dot(const V2NoCtor& rhs) const
	{
		luvafair::F32 ret = x * rhs.x + y * rhs.y;
		return ret;
	}

	V2NoCtor TruncateToInt() const
	{
		V2NoCtor ret;
		ret.x = x.Int();
		ret.y = y.Int();
		return ret;
	}
};

class V2 : public V2NoCtor
{
	public:
		V2() { x = 0; y = 0; }
		V2(int x_, int y_) { x = x_; y = y_; }
		V2(luvafair::F32 x_, luvafair::F32 y_) { x = x_; y = y_; }
		V2(const touchPosition& tp) { x = tp.px; y = tp.py; }
		V2(const V2NoCtor& v2)
		{
			x = v2.x;
			y = v2.y;
		}
};

#endif
