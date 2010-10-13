#include "lg/rand.h"

namespace luvafair
{

// this code is taken from the SGADE gameboy library, thanks to Jaap

unsigned int Rand::ReloadMersenneTwister()
{
	// Variables;
    register unsigned int *p0=mRandState, *p2=mRandState+2, *pM=mRandState+LG_RAND_PERIOD, s0, s1;
    register int j;

	// Code;
    if( mRandLeft < -1 ) Seed( 4357U );

    mRandLeft = LG_RAND_STATE_VECTOR_LENGTH - 1;
	mRandNext = mRandState + 1;

    for( s0 = mRandState[0], s1 = mRandState[1], j = LG_RAND_STATE_VECTOR_LENGTH - LG_RAND_PERIOD + 1; --j; s0 = s1, s1 = *p2++ )
	{
        *p0++ = *pM++ ^ (LG_RAND_MIX_BITS(s0, s1) >> 1) ^ (LG_RAND_LO_BIT(s1) ? LG_RAND_MAGIC : 0U);
	}

    for( pM = mRandState, j = LG_RAND_PERIOD; --j; s0 = s1, s1 = *p2++ )
	{
        *p0++ = *pM++ ^ (LG_RAND_MIX_BITS(s0, s1) >> 1) ^ (LG_RAND_LO_BIT(s1) ? LG_RAND_MAGIC : 0U);
	}

    s1=mRandState[0], *p0 = *pM ^ (LG_RAND_MIX_BITS(s0, s1) >> 1) ^ (LG_RAND_LO_BIT(s1) ? LG_RAND_MAGIC : 0U);
    s1 ^= (s1 >> 11);
    s1 ^= (s1 <<  7) & 0x9D2C5680U;
    s1 ^= (s1 << 15) & 0xEFC60000U;
    
	return(s1 ^ (s1 >> 18));
}


unsigned int Rand::Get()
{
	unsigned int y;

    if( --mRandLeft < 0 ) return ReloadMersenneTwister();

    y  = *mRandNext++;
    y ^= (y >> 11);
    y ^= (y <<  7) & 0x9D2C5680U;
    y ^= (y << 15) & 0xEFC60000U;
    
	return(y ^ (y >> 18));
}

luvafair::F32 Rand::Getf()
{
#define LG_MAX_RANDF 8192
	F32 ret((int)(Get() % LG_MAX_RANDF));
	ret /= LG_MAX_RANDF;
	return ret;
}

void Rand::Seed(unsigned int a_Seed)
{
	register unsigned int x = (a_Seed | 1U) & 0xFFFFFFFFU, *s = mRandState;
    register int j;

    for( mRandLeft = 0, *s++ = x, j = LG_RAND_STATE_VECTOR_LENGTH; 
		 --j; 
		 *s++ = (x*=69069U) & 0xFFFFFFFFU );
}

}
