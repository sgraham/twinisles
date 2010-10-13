#ifndef LG_INCLUDED_scoped_ptr_H
#define LG_INCLUDED_scoped_ptr_H

#include "lg/lg.h"

namespace luvafair
{
	template <class T>
	class ScopedPtr
	{
		public:

		ScopedPtr(T* ptr = 0) : mPtr(ptr) {}

		~ScopedPtr() 
		{ 
			typedef char typeMustBeComplete[ sizeof(T) ];   
			delete mPtr;
		}

		T* operator -> () const { assert(mPtr && "null pointer exception"); return  mPtr; }
		T& operator *  () const { assert(mPtr && "null pointer exception"); return *mPtr; }

		void Reset(T* ptr = 0)
		{
			typedef char typeMustBeComplete[ sizeof(T) ];   
			delete mPtr;
			mPtr = ptr;
		}

		T* Get() const { return mPtr; }

		bool IsNull() const { return mPtr == 0; }

		private:
			ScopedPtr(const ScopedPtr&);
			ScopedPtr& operator=(const ScopedPtr&);
			T* mPtr;
	};

}

#endif
