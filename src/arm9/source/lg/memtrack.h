#ifndef LG_INCLUDED_lg_memtrack_H
#define LG_INCLUDED_lg_memtrack_H

#include <cstdio>

namespace lg
{
    struct MemStamp
    {
		char const * const file;
		int line;
		MemStamp(char const *file, int line) : file(file), line(line) {}
    };

	struct TrackCP;
	struct TrackCheckpoint
	{
		TrackCheckpoint();
		~TrackCheckpoint();

		void Clear();
		bool Contains(int i) const;

		TrackCP* head;
	};

	void* TrackMalloc(size_t size);
	void TrackFree(void* p);
	void TrackStamp(void* p, const MemStamp& ms);
	void TrackDump(TrackCheckpoint* cp = 0);

    template <class T> inline T *operator*(const MemStamp &stamp, T *p)
    {
        TrackStamp(p, stamp);
        return p;
    }

	void TrackSaveCheckpoint(TrackCheckpoint* checkpoint);
}

#define LG_MEMTRACK_NEW lg::MemStamp(__FILE__, __LINE__) * new
#define new LG_MEMTRACK_NEW

#endif
