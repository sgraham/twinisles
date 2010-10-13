#include "lg/lg.h"
#include "lg/texture.h"
#include <climits>

namespace luvafair
{

// todo; init/purge
static uint32* sNextBlock = (uint32*)0x06800000;
//static uint32  sNextPBlock = 0;

static void GetDimensionsFromTexFormat(u16 texFormat, u16& width, u16& height, u16& bpp)
{
	u16 wlog2m3 = (texFormat >> (20 - 16)) & 7;
	u16 hlog2m3 = (texFormat >> (23 - 16)) & 7;
	u16 fmt = (texFormat >> (26 - 16)) & 7;

	assert(fmt == 6 && "haven't encoded formats other than rgb8_a5 yet");
	bpp = 8;

	width = 1 << (wlog2m3 + 3);
	height = 1 << (hlog2m3 + 3);
}

static void GetDataPointers(u16 texFormat, const u8* data, const void*& palette, const void*& texture)
{
	int fmt = (texFormat >> (26 - 16)) & 7;
	data += 2; // texFormat u16
	switch (fmt)
	{
		case 1: palette = data;			// rgb32_a3
				texture = data + 32 * sizeof(u16);
				break;

		case 2: palette = data;			// rgb4
				texture = data + 4 * sizeof(u16);
				break;

		case 3: palette = data;			// rgb16
				texture = data + 16 * sizeof(u16);
				break;

		case 4: palette = data;			// rgb256
				texture = data + 256 * sizeof(u16);
				break;

		case 6: palette = data;			// rgb8_a5
				texture = data + 8 * sizeof(u16);
				break;

		case 7: palette = 0;			// rgba
				texture = data;

		default:
				palette = 0;
				texture = 0;
				assert(false && "unhandled texture format");
	}
}


static inline uint32 alignVal(uint32 val, uint32 to)
{
  return (val & (to-1))? (val & ~(to-1)) + to : val;
}

/*
static int GetNextPaletteSlot(u16 count, uint8 format)
{
  // ensure the result aligns on a palette block for this format
  uint32 result = alignVal(nextPBlock, 1<<(4-(format==GL_RGB4)));
  
  // convert count to bytes and align to next (smallest format) palette block
  count = alignVal( count<<1, 1<<3 ); 

  // ensure that end is within palette video mem
  if( result+count > 0x10000 )   // VRAM_F - VRAM_E
    return -1;

  nextPBlock = result+count;
  return (int)result;
} 
*/

uint16* VramGetBank(uint16 *addr)
{
	if(addr >= VRAM_A && addr < VRAM_B)
		return VRAM_A;
	else if(addr >= VRAM_B && addr < VRAM_C)
		return VRAM_B;
	else if(addr >= VRAM_C && addr < VRAM_D)
		return VRAM_C;
	else if(addr >= VRAM_D && addr < VRAM_E)
		return VRAM_D;
	else if(addr >= VRAM_E && addr < VRAM_F)
		return VRAM_E;
	else if(addr >= VRAM_F && addr < VRAM_G)
		return VRAM_F;
	else if(addr >= VRAM_G && addr < VRAM_H)
		return VRAM_H;
	else if(addr >= VRAM_H && addr < VRAM_I)
		return VRAM_H;
	else return VRAM_I;
}


int VramIsTextureBank(uint16 *addr)
{
   uint16* vram = VramGetBank(addr);

   if(vram == VRAM_A)
   {
      if((VRAM_A_CR & 3) == ((VRAM_A_TEXTURE) & 3))
         return 1;
      else return 0;
   }
   else if(vram == VRAM_B)
   {
      if((VRAM_B_CR & 3) == ((VRAM_B_TEXTURE) & 3))
         return 1;
      else return 0;
   }
   else if(vram == VRAM_C)
   {
      if((VRAM_C_CR & 3) == ((VRAM_C_TEXTURE) & 3))
         return 1;
      else return 0;
   }
   else if(vram == VRAM_D)
   {
      if((VRAM_D_CR & 3) == ((VRAM_D_TEXTURE) & 3))
         return 1;
      else return 0;
   }
   else
      return 0;
   
} 
uint32* GetNextTextureSlot(int size)
{
   uint32* result = sNextBlock;
   sNextBlock += size >> 2;

   //uh-oh...out of texture memory in this bank...find next one assigned to textures
   while(!VramIsTextureBank((uint16*)sNextBlock - 1) && sNextBlock <= (uint32*)VRAM_E)
   {
      sNextBlock = (uint32*)VramGetBank((uint16*)result) + (0x20000 >> 2); //next bank
      result = sNextBlock;        
      sNextBlock += size >> 2;
   }

   if(sNextBlock > (uint32*)VRAM_E)
      return 0;

   else return result;   

} 

Texture::Texture(const u8* data) : mData(data)
{
	u16 width, height, bpp;
	const void* texture;
	const void* palette;
	u16 texFormat = *(const u16*)mData;
	GetDimensionsFromTexFormat(texFormat, width, height, bpp);
	GetDataPointers(texFormat, mData, palette, texture);

	uint32 size = width * height * bpp / CHAR_BIT;
	uint32* addr = GetNextTextureSlot(size);

	uint32 vramTemp = vramSetMainBanks(VRAM_A_LCD, VRAM_B_LCD, VRAM_C_LCD, VRAM_D_LCD);

    swiCopy((void*)texture, addr, size / 4 | COPY_MODE_WORD);
	mAddress = (((uint32)addr) / 8) & 0xffff;

	vramRestoreMainBanks(vramTemp);
}

}
