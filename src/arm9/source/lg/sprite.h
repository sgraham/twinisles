#ifndef LG_INCLUDED_lg_sprite_H
#define LG_INCLUDED_lg_sprite_H

#include <nds.h>
#include "lg/v2.h"

namespace luvafair
{
	class SpriteHardware
	{
		public:
			SpriteHardware();
			void Update();
			void Reset();

			static void Init();
			static void Purge();

		private:
			SpriteEntry mSpriteEntries[128];
			SpriteRotation* mSpriteRotations;
			int mCurTile;
			int mCurRotation;

			int AllocatePalette(const u8* paletteData);
			int AllocateRotation();
			int FindUnallocatedId() const;

			uint16 Get0(int id) const { return mSpriteEntries[id].attribute[0]; }
			uint16 Get1(int id) const { return mSpriteEntries[id].attribute[1]; }
			uint16 Get2(int id) const { return mSpriteEntries[id].attribute[2]; }

			void Set0(int id, uint16 val) { mSpriteEntries[id].attribute[0] = val; }
			void Set1(int id, uint16 val) { mSpriteEntries[id].attribute[1] = val; }
			void Set2(int id, uint16 val) { mSpriteEntries[id].attribute[2] = val; }

			friend class SpriteData;
			friend class SpriteInstance;
			friend class SpriteMatrix;
	};

	extern SpriteHardware* gSpriteHardware;

	class SpriteData
	{
		public:
			SpriteData(int w, int h, const u8* palette, int numFrames = 1);
			void AddFrame(const u8* frameData);
			void UpdateFrame(int frame, const u8* data);
			void SetEndOfAnimation();

			int GetWidth() const { return mWidth; }
			int GetHeight() const { return mHeight; }

		private:
			int mWidth;
			int mHeight;
			int mBaseTile;
			int mPaletteId;
			int mNumAddedFrames;
			enum { MAX_NUM_ANIMS = 4 }; // todo; lame-o
			int mAnimEndFrames[MAX_NUM_ANIMS];
			int mNumAnims;

			friend class SpriteInstance;
	};

	class SpriteMatrix
	{
		int mId;
		public:
			SpriteMatrix();
			void SetRotation(int degrees);
			int GetId() const { return mId; }
	};

	class SpriteInstance
	{
		public:
			SpriteInstance(const SpriteData* sprite);
			~SpriteInstance();

			void SetAnimation(int anim);
			void SetFrame(int frame);
			void SetPosition(int x, int y);
			void SetPosition(const V2& pos);
			V2 GetPosition() const;
			void SetMatrix(const SpriteMatrix* matrix);
			void SetFlipX(bool flipped);
			void SetFlipY(bool flipped);
			const SpriteData* GetData() const { return mSpriteData; }

		private:
			const SpriteData* mSpriteData;
			int mAnimation;
			int mId;
	};
}

#endif
