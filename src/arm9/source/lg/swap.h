#ifndef LG_INCLUDED_lg_swap_H
#define LG_INCLUDED_lg_swap_H

namespace luvafair
{
	template<typename T> inline void Swap(T& a, T& b)
	{
		T tmp = a;
		a = b;
		b = tmp;
	}
}

#endif
