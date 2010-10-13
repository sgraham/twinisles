#include "lg/lg.h"
#include "lg/sprite.h"
#include <nds/arm9/trig_lut.h>
#include <cstring>
#include <cstdio>

namespace luvafair
{

inline void DmaCopy(void* dest, const void* source, int size)
{
	assert(size % 4 == 0); // actually only has to be 2 right now, but would like to be able to change to COPY_WORDS if it's faster
	DMA_SRC(3) = (uint32)source;
	DMA_DEST(3) = (uint32)dest;
	DMA_CR(3) = DMA_COPY_HALFWORDS | (size>>1);
	while(DMA_CR(3) & DMA_BUSY);
}

// -------------------------------------------------
SpriteHardware* gSpriteHardware;

void SpriteHardware::Init()
{
	assert(gSpriteHardware == 0);
	gSpriteHardware = new SpriteHardware();
}

void SpriteHardware::Purge()
{
	assert(gSpriteHardware != 0);
	delete gSpriteHardware;
}

// -------------------------------------------------
SpriteHardware::SpriteHardware()
{
	Reset();
}

void SpriteHardware::Reset()
{
	mCurTile = 0;
	mCurRotation = 0;
	mSpriteRotations = (SpriteRotation*)(void*)mSpriteEntries;
	for (int i = 0; i < 128; ++i)
	{
		mSpriteEntries[i].attribute[0] = ATTR0_DISABLED;
		mSpriteEntries[i].attribute[1] = 0;
		mSpriteEntries[i].attribute[2] = 0;
	}
	for (int i = 0; i < 32; ++i)
	{
		mSpriteRotations[i].hdx = 256;
		mSpriteRotations[i].hdy = 0;
		mSpriteRotations[i].vdx = 0;
		mSpriteRotations[i].vdy = 256;
	}
}

// -------------------------------------------------
int SpriteHardware::AllocatePalette(const u8* palette)
{
	// todo; need to do something useful here
	DmaCopy(SPRITE_PALETTE, palette, 512);
	return 0;
}

// -------------------------------------------------
int SpriteHardware::AllocateRotation()
{
	// todo; need to do something useful here
	assert(mCurRotation < 32);
	return mCurRotation++;
}

// -------------------------------------------------
int SpriteHardware::FindUnallocatedId() const
{
	for (int i = 0; i < 128; ++i)
	{
		if (Get0(i) & ATTR0_DISABLED) return i;
	}
	assert(false && "no sprite OAM entries available");
	return 0;
}

// -------------------------------------------------
void SpriteHardware::Update()
{
	DC_FlushAll();
	DmaCopy(OAM, mSpriteEntries, sizeof(mSpriteEntries));
}


// -------------------------------------------------
SpriteData::SpriteData(int w, int h, const u8* palette, int numFrames)
	: mWidth(w)
	, mHeight(h)
	, mNumAddedFrames(0)
	, mNumAnims(0)
{
	for (int i = 0; i < ARRAYSIZE(mAnimEndFrames); ++i) mAnimEndFrames[i] = 0;
	mPaletteId = gSpriteHardware->AllocatePalette(palette);

	assert(gSpriteHardware);
	mBaseTile = gSpriteHardware->mCurTile;
	gSpriteHardware->mCurTile += (w * h * numFrames) / 256;
}

// -------------------------------------------------
void SpriteData::AddFrame(const u8* data)
{
	assert(mNumAnims < ARRAYSIZE(mAnimEndFrames));
	DmaCopy(&SPRITE_GFX[(mBaseTile + (mWidth * mHeight * mNumAddedFrames) / 256) * 128], data, mWidth * mHeight);
	++mNumAddedFrames;
	++mAnimEndFrames[mNumAnims];
}

// -------------------------------------------------
void SpriteData::UpdateFrame(int frame, const u8* data)
{
	// hmm, for some reason, DC_FlushAll() + DMA isn't sufficient? Flush isn't synch?
	memcpy(&SPRITE_GFX[(mBaseTile + (mWidth * mHeight * frame) / 256) * 128], data, mWidth * mHeight);
}

// -------------------------------------------------
void SpriteData::SetEndOfAnimation()
{
	++mNumAnims;
	if (mNumAnims < ARRAYSIZE(mAnimEndFrames))
	{
		mAnimEndFrames[mNumAnims] = mAnimEndFrames[mNumAnims - 1];
	}
}

// -------------------------------------------------
SpriteMatrix::SpriteMatrix()
{
	assert(gSpriteHardware);
	mId = gSpriteHardware->AllocateRotation();
}

// -------------------------------------------------
void SpriteMatrix::SetRotation(int degrees)
{
	assert(gSpriteHardware);
	int lutangle = degrees * LUT_SIZE / 360;
	int cosine = COS[lutangle & LUT_MASK] >> 4;
	int sine = SIN[lutangle & LUT_MASK] >> 4;
	gSpriteHardware->mSpriteRotations[mId].hdx = cosine;
	gSpriteHardware->mSpriteRotations[mId].hdy = -sine;
	gSpriteHardware->mSpriteRotations[mId].vdx = sine;
	gSpriteHardware->mSpriteRotations[mId].vdy = cosine;
}

struct WidthHeightToAttr
{
	u8 w, h;
	u16 attr1;
	u16 attr0;
};
static const WidthHeightToAttr whmap[] =
{
	{ 8, 8, ATTR1_SIZE_8, ATTR0_SQUARE },
	{ 16, 16, ATTR1_SIZE_16, ATTR0_SQUARE },
	{ 32, 32, ATTR1_SIZE_32, ATTR0_SQUARE },
	{ 64, 64, ATTR1_SIZE_64, ATTR0_SQUARE },
	{ 64, 32, ATTR1_SIZE_64, ATTR0_WIDE },
};

// -------------------------------------------------
SpriteInstance::SpriteInstance(const SpriteData* spriteData) : mSpriteData(spriteData), mAnimation(0)
{
	assert(gSpriteHardware);
	mId = gSpriteHardware->FindUnallocatedId();
	gSpriteHardware->Set0(mId, ATTR0_COLOR_256 | ATTR0_NORMAL);
	uint16 attr1size = 0;
	uint16 attr0size = 0;
	u16 i;
	for (i = 0; i < sizeof(whmap)/sizeof(whmap[0]); ++i)
	{
		if (whmap[i].w == spriteData->mWidth && whmap[i].h == spriteData->mHeight)
		{
			attr1size = whmap[i].attr1;
			attr0size = whmap[i].attr0;
			break;
		}
	}
	assert(i != sizeof(whmap)/sizeof(whmap[0]));
	gSpriteHardware->Set0(mId, gSpriteHardware->Get0(mId) | attr0size);
	gSpriteHardware->Set1(mId, ATTR1_ROTDATA(0) | attr1size);
	gSpriteHardware->Set2(mId, ATTR2_PRIORITY(1) | ATTR2_PALETTE(mSpriteData->mPaletteId));
	SetFrame(0);
}

// -------------------------------------------------
SpriteInstance::~SpriteInstance()
{
	assert(gSpriteHardware);
	gSpriteHardware->Set0(mId, ATTR0_DISABLED);
}

// -------------------------------------------------
void SpriteInstance::SetAnimation(int anim)
{
	mAnimation = anim;
	SetFrame(0);
}

// -------------------------------------------------
void SpriteInstance::SetFrame(int frame)
{
	int offset = 0;
	if (mAnimation > 0) offset = GetData()->mAnimEndFrames[mAnimation - 1];
	uint16 a2 = gSpriteHardware->Get2(mId) & 0xfc00;
	uint16 tile = GetData()->mBaseTile + ((GetData()->mWidth * GetData()->mHeight) / 256) * (offset + frame);
	a2 |= tile;
	gSpriteHardware->Set2(mId, a2);
}

// -------------------------------------------------
void SpriteInstance::SetPosition(int x, int y)
{
	assert(gSpriteHardware);

	uint16 a0 = gSpriteHardware->Get0(mId) & 0xff00;
	a0 |= y & 0xff;
	gSpriteHardware->Set0(mId, a0);

	uint16 a1 = gSpriteHardware->Get1(mId) & 0xfe00;
	a1 |= x & 0x1ff;
	gSpriteHardware->Set1(mId, a1);
}

// -------------------------------------------------
void SpriteInstance::SetPosition(const V2& pos)
{
	SetPosition(pos.x.Int(), pos.y.Int());
}

// -------------------------------------------------
V2 SpriteInstance::GetPosition() const
{
	assert(gSpriteHardware);

	return V2(gSpriteHardware->Get1(mId) & 0xfe00, gSpriteHardware->Get0(mId) & 0xff);
}

// -------------------------------------------------
void SpriteInstance::SetMatrix(const SpriteMatrix* matrix)
{
	assert(gSpriteHardware);
	if (matrix)
	{
		gSpriteHardware->Set0(mId, (gSpriteHardware->Get0(mId) & ~0x3) | ATTR0_ROTSCALE);
		gSpriteHardware->Set1(mId, (gSpriteHardware->Get1(mId) & ~0x3e00) | ATTR1_ROTDATA(matrix->GetId()));
	}
	else
	{
		gSpriteHardware->Set0(mId, (gSpriteHardware->Get0(mId) & ~0x3) | ATTR0_NORMAL);
	}
}

void SpriteInstance::SetFlipX(bool flipped)
{
	SetMatrix(0);
	if (flipped)
	{
		gSpriteHardware->Set1(mId, gSpriteHardware->Get1(mId) | ATTR1_FLIP_X);
	}
	else
	{
		gSpriteHardware->Set1(mId, gSpriteHardware->Get1(mId) & ~ATTR1_FLIP_X);
	}
}

void SpriteInstance::SetFlipY(bool flipped)
{
	SetMatrix(0);
	if (flipped)
	{
		gSpriteHardware->Set1(mId, gSpriteHardware->Get1(mId) | ATTR1_FLIP_Y);
	}
	else
	{
		gSpriteHardware->Set1(mId, gSpriteHardware->Get1(mId) & ~ATTR1_FLIP_Y);
	}
}

}
