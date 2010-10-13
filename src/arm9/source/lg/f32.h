#ifndef LG_INCLUDED_lg_f32_H
#define LG_INCLUDED_lg_f32_H

#include <nds.h>

namespace luvafair
{

class F32
{
	public:
		int32 v;

		F32() : v(0) {}
		F32(int val) : v(inttof32(val)) {}
		F32(float val) : v(floattof32(val)) {}

		inline F32 operator*(const F32& rhs) const
		{
			F32 ret;
			ret.v = mulf32(v, rhs.v);
			return ret;
		}

		inline F32 operator/(const F32& rhs) const
		{
			F32 ret;
			ret.v = divf32(v, rhs.v);
			return ret;
		}

		inline F32 operator+(const F32& rhs) const
		{
			F32 ret;
			ret.v = v + rhs.v;
			return ret;
		}

		inline F32 operator-(const F32& rhs) const
		{
			F32 ret;
			ret.v = v - rhs.v;
			return ret;
		}

		inline void operator+=(const F32& rhs)
		{
			v += rhs.v;
		}

		inline void operator-=(const F32& rhs)
		{
			v -= rhs.v;
		}

		inline void operator*=(const F32& rhs)
		{
			v = mulf32(v, rhs.v);
		}

		inline void operator/=(const F32& rhs)
		{
			v = divf32(v, rhs.v);
		}

		inline int Int() const
		{
			return f32toint(v);
		}

		inline bool operator<(const F32& rhs) const
		{
			return v < rhs.v;
		}

		inline bool operator>(const F32& rhs) const
		{
			return v > rhs.v;
		}

		inline bool operator>=(const F32& rhs) const
		{
			return v >= rhs.v;
		}

		inline bool operator<=(const F32& rhs) const
		{
			return v <= rhs.v;
		}

		inline bool operator!=(const F32& rhs) const
		{
			return v != rhs.v;
		}

		inline F32 operator-() const
		{
			F32 ret;
			ret.v = -v;
			return ret;
		}

		inline bool operator==(const F32& rhs) const
		{
			return v == rhs.v;
		}
};

}

#endif
