#ifndef LG_INCLUDED_lg_texture_H
#define LG_INCLUDED_lg_texture_H

namespace luvafair
{
	// represents a resident hardware texture.
	// format:
	// - first u16 is top 16bits of GFX_TEX_FORMAT
	// - if appropriate, palette follows
	// - image data follows after that
	class Texture
	{
		uint16 mAddress;
		const u8* mData;

		public:
			Texture(const u8* data);
	};
}

#endif
