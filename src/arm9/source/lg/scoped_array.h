#ifndef LG_INCLUDED_core_scoped_array_H
#define LG_INCLUDED_core_scoped_array_H

#include "lg/lg.h"
#include <cstddef>

namespace luvafair
{
	template <class T> class ScopedArray
	{
		private:
			T* mPtr;
			ScopedArray(ScopedArray const &);
			ScopedArray& operator=(ScopedArray const &);

		public:
			explicit ScopedArray(T * p = 0) : mPtr(p) {}

			~ScopedArray()
			{
		        typedef char typeMustBeComplete[sizeof(T)];
				delete [] mPtr;
			}

			void Reset(T* p = 0)
			{
				assert(p == 0 || p != mPtr);
				ScopedArray<T>(p).Swap(*this);
			}

			T& operator[](std::ptrdiff_t i) const
			{
				assert(mPtr != 0);
				assert(i >= 0);
				return mPtr[i];
			}

			T* Get() const
			{
				return mPtr;
			}

			bool IsNull() const { return mPtr == 0; }

			operator bool () const
			{
				return mPtr != 0;
			}
			bool operator! () const
			{
				return mPtr == 0;
			}

			void Swap(ScopedArray& b)
			{
				T * tmp = b.mPtr;
				b.mPtr = mPtr;
				mPtr = tmp;
			}

	};
}

#endif

