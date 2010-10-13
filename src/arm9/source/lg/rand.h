#ifndef LG_INCLUDED_lg_rand_H
#define LG_INCLUDED_lg_rand_H

#include "lg/f32.h"

namespace luvafair
{

	class Rand
	{
#define LG_RAND_STATE_VECTOR_LENGTH (624)                 
#define LG_RAND_PERIOD (397)                 
#define LG_RAND_MAGIC (0x9908B0DFU)         
#define LG_RAND_HI_BIT(u) ((u) & 0x80000000U)
#define LG_RAND_LO_BIT(u) ((u) & 0x00000001U)
#define LG_RAND_LO_BITS(u) ((u) & 0x7FFFFFFFU)
#define LG_RAND_MIX_BITS(u, v) (LG_RAND_HI_BIT(u)|LG_RAND_LO_BITS(v))
		unsigned int *mRandNext;          
		int mRandLeft;
		unsigned int mRandState[ LG_RAND_STATE_VECTOR_LENGTH + 1 ];  
		unsigned int ReloadMersenneTwister();
		
	public:
		Rand() : mRandLeft(-1) {}
		unsigned int Get();
		void Seed(unsigned int seed);
		luvafair::F32 Getf();
	};

}

#endif
