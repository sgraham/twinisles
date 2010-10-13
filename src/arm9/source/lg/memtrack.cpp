#include "lg/lg.h"
#include "lg/memtrack.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <nds.h>

#undef new
#include <new>

namespace lg
{
	struct Header
	{
		char signature[4];
		Header* prev;
		Header* next;
		char file[104];
		int line;
		int count;
		size_t size;
	};

	struct TrackCP
	{
		TrackCP* next;
		int allocNumber;
	};

	TrackCheckpoint::TrackCheckpoint() : head(0) {}

	TrackCheckpoint::~TrackCheckpoint()
	{
		Clear();
	}

	void TrackCheckpoint::Clear()
	{
		while (head)
		{
			TrackCP* next = head->next;
			free(head);
			head = next;
		}
	}

	bool TrackCheckpoint::Contains(int i) const
	{
		TrackCP* cur = head;
		while (cur)
		{
			if (i == cur->allocNumber) return true;
			cur = cur->next;
		}

		return false;
	}

	static Header* gListOfBlocks = 0;
	static int gAllocCount = 0;
	
	static Header* GetHeader(void *p)
	{
		Header* h = (Header*)(((unsigned char*)p) - sizeof(Header));
		if (h->signature[0] != 'L'
			|| h->signature[1] != 'G'
			|| h->signature[2] != 'M'
			|| h->signature[3] != 'B')
		{
			consoleDemoInit();
			consoleClear();
			BG_PALETTE_SUB[0] = RGB15(0,0,0);
			BG_PALETTE_SUB[255] = RGB15(31,31,31);
			iprintf("Header wasn't LGMB!\n");
			iprintf("0x%08x {%d}: %d bytes, %s(%d)\n", (unsigned int)(((unsigned char*)h) + sizeof(Header)), h->count, h->size, h->file, h->line);
			for (;;) {}
		}
		return h;
	}

	void* TrackMalloc(size_t size)
	{
		size_t origSize = size;
		size += sizeof(Header);
		void* ret = malloc(size);
		Header* h = (Header*)ret;
		h->signature[0] = 'L';
		h->signature[1] = 'G';
		h->signature[2] = 'M';
		h->signature[3] = 'B';
		h->prev = 0;
		h->next = gListOfBlocks;
		if (gListOfBlocks) gListOfBlocks->prev = h;
		gListOfBlocks = h;
		h->count = gAllocCount++;
		h->size = origSize;
		unsigned char* data = (unsigned char*)ret + sizeof(Header);
		return data;
	}

	void TrackFree(void* p)
	{
		if (p == 0) return;

		Header* h = GetHeader(p);

		if (h->prev)
		{
			h->prev->next = h->next;
		}
		else
		{
			gListOfBlocks = h->next;
		}

		if (h->next)
		{
			h->next->prev = h->prev;
		}

		h->signature[2] = 'D';
		h->signature[3] = 'M';

		free(h);
	}

	void TrackStamp(void* p, const MemStamp& stamp)
	{
		Header* h = GetHeader(p);
		strncpy(h->file, stamp.file, sizeof(h->file) - 1);
		h->file[sizeof(h->file) - 1] = 0;
		h->line = stamp.line;
	}

	void TrackDump(TrackCheckpoint* cp)
	{
#if 1 //LG_PLATFORM_NDS
		bool changedMode = false;

		int offset = 0;
		for (;;)
		{
			Header* h = gListOfBlocks;
			int count = 0;
			while (h != 0)
			{
				if (!cp || !cp->Contains(h->count))
				{
					if (count++ >= offset)
					{
						if (!changedMode)
						{
							consoleDemoInit();
							consoleClear();
							BG_PALETTE_SUB[0] = RGB15(0,0,0);
							BG_PALETTE_SUB[255] = RGB15(31,31,31);
							changedMode = true;
						}
						iprintf("0x%08x {%d}: %d bytes, %s(%d)\n", (unsigned int)(((unsigned char*)h) + sizeof(Header)), h->count, h->size, h->file, h->line);
					}
				}
				h = h->next;
			}

			if (changedMode)
			{
				for (;;)
				{
					scanKeys();
					if (keysDown() & KEY_UP)
					{
						offset--;
						if (offset < 0) offset = 0;
						break;
					}
					else if (keysDown() & KEY_DOWN)
					{
						offset++;
						break;
					}
				}

				consoleClear();
			}
			else
			{
				break;
			}
		}

#else
		Header* h = gListOfBlocks;
		bool printedHeader = false;
		while (h != 0)
		{
			if (!cp || !cp->Contains(h->count))
			{
				if (!printedHeader)
				{
					printf("Leaks detected:\n");
					printedHeader = true;
				}
				printf("0x%08x {%d}: %d bytes, %s(%d)\n", (unsigned int)(((unsigned char*)h) + sizeof(Header)), h->count, h->size, h->file, h->line);
			}
			h = h->next;
		}
#endif
	}

	void TrackSaveCheckpoint(TrackCheckpoint* cp)
	{
		cp->Clear();

		Header* h = gListOfBlocks;
		while (h != 0)
		{
			TrackCP* c = (TrackCP*)malloc(sizeof(TrackCP));
			c->next = cp->head;
			c->allocNumber = h->count;
			cp->head = c;

			h = h->next;
		}
	}
}

void *operator new(size_t size)
{
    void *p = lg::TrackMalloc(size);
	assert(p);
    return p;
}

void operator delete(void *p)
{
    lg::TrackFree(p);
}

void *operator new[](size_t size)
{
    void *p = lg::TrackMalloc(size);
	assert(p);
    return p;
}

void operator delete[](void *p)
{
    lg::TrackFree(p);
}

