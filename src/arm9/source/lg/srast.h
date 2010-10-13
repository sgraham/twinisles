//
// "S"imple "Rast"erizer.
// scott.srast@h4ck3r.net.
//
// Rasterizes points, lines, and polygons into a user buffer. Solid colour only.
// It's not fast. At all.

// Undoubtedly there are others that are better in some/many ways around,
// but I wanted something small, simple, and self-contained. So here it is.
//

#ifndef INCLUDED_SRast_H
#define INCLUDED_SRast_H

template <class T> class SRastBuffer
{
	T* mData;
	int mWidth;
	int mPitch;
	int mHeight;

public:
	SRastBuffer(T* data, int width, int pitch, int height)
		: mData(data)
		, mWidth(width)
		, mPitch(pitch)
		, mHeight(height)
	{
	}

	static inline size_t ExpectedBufferSize(int pitch, int height) { return sizeof(T) * pitch * height; }

	void Clear(T colour)
	{
		for (int y = 0; y < mHeight; y++) for (int x = 0; x < mWidth; ++x) DrawPixel(x, y, colour);
	}

	void DrawPixel(int x, int y, T colour)
	{
		if (x < 0 || y < 0 || x >= mWidth || y >= mHeight) return;
		mData[y * mWidth + x] = colour;
	}

    void DrawLine(int x0, int y0, int x1, int y1, T colour)
	{
		int dy = y1 - y0;
		int dx = x1 - x0;
		int stepx, stepy;

		if (dy < 0) { dy = -dy;  stepy = -1; } else { stepy = 1; }
		if (dx < 0) { dx = -dx;  stepx = -1; } else { stepx = 1; }
		dy <<= 1;
		dx <<= 1;

		DrawPixel(x0, y0, colour);
		if (dx > dy)
		{
			int fraction = dy - (dx >> 1);
			while (x0 != x1)
			{
				if (fraction >= 0)
				{
					y0 += stepy;
					fraction -= dx;
				}
				x0 += stepx;
				fraction += dy;
				DrawPixel(x0, y0, colour);
			}
		}
		else
		{
			int fraction = dx - (dy >> 1);
			while (y0 != y1)
			{
				if (fraction >= 0)
				{
					x0 += stepx;
					fraction -= dy;
				}
				y0 += stepy;
				fraction += dx;
				DrawPixel(x0, y0, colour);
			}
		}
	}

private:
	enum 
	{
		PolygonFixedShift = 18
	};
	enum
	{
		// Can be changed arbitrarily, only used to avoid heap allocation in DrawPolygon routine (fixed stack size)
		MaxNumVerticesInPolygon = 20
	};

	struct PolygonEdge
	{
		int top;
		int bottom;
		int x, dx; // fixed;
		int w; // fixed;
		PolygonEdge* prev;
		PolygonEdge* next;

		PolygonEdge(const int* i1, const int* i2)
		{
		   if (i2[1] < i1[1])
		   {
		      const int *it;

		      it = i1;
		      i1 = i2;
		      i2 = it;
		   }

		   top = i1[1];
		   bottom = i2[1] - 1;
		   dx = ((i2[0] - i1[0]) << PolygonFixedShift) / (i2[1] - i1[1]);
		   x = (i1[0] << PolygonFixedShift) + (1<<(PolygonFixedShift-1)) - 1;
		   prev = 0;
		   next = 0;

		   if (dx < 0)
			   x += min(dx+(1<<PolygonFixedShift), 0);

		   w = max(abs(dx)-(1<<PolygonFixedShift), 0);
		}

		PolygonEdge() : top(-1), bottom(-1), prev(0), next(0) {}

	private:
		template <class U> U min(const U& a, const U& b) { return (a < b) ? a : b; }
		template <class U> U max(const U& a, const U& b) { return (a > b) ? a : b; }
		template <class U> U abs(const U& a) { return (a < 0) ? -a : a; }
	};

	inline PolygonEdge *addEdge(PolygonEdge *list, PolygonEdge *edge, bool sortByX)
	{
		PolygonEdge *pos = list;
		PolygonEdge *prev = 0;

		if (sortByX)
		{
			while ((pos) && ((pos->x + (pos->w + pos->dx) / 2) < (edge->x + (edge->w + edge->dx) / 2)))
			{
				prev = pos;
				pos = pos->next;
			}
		}
		else
		{
			while ((pos) && (pos->top < edge->top))
			{
				prev = pos;
				pos = pos->next;
			}
		}

		edge->next = pos;
		edge->prev = prev;

		if (pos) pos->prev = edge;

		if (prev)
		{
			prev->next = edge;
			return list;
		}
		else
		{
			return edge;
		}
	}

	inline PolygonEdge *removeEdge(PolygonEdge *list, PolygonEdge *edge)
	{
		if (edge->next) edge->next->prev = edge->prev;

		if (edge->prev)
		{
			edge->prev->next = edge->next;
			return list;
		}
		else
		{
			return edge->next;
		}
	}


public:
	void DrawPolygon(int vertices, const int *points, T colour)
	{
		int c;
		int top = mHeight;
		int bottom = 0;
		const int *i1, *i2;
		PolygonEdge *edge;
		PolygonEdge *nextEdge;
		PolygonEdge *activeEdges = 0;
		PolygonEdge *inactiveEdges = 0;

		if (vertices >= MaxNumVerticesInPolygon)
		{
			return;
		}
		PolygonEdge tempBuf[MaxNumVerticesInPolygon];

		edge = tempBuf;
		i1 = points;
		i2 = points + (vertices-1) * 2;

		// make a list of edges, adding them all to the inactive list
		// keep track of the top/bottom as we go
		for (c = 0; c < vertices; ++c)
		{
			if (i1[1] != i2[1])
			{
				*edge = PolygonEdge(i1, i2);

				if (edge->bottom >= edge->top)
				{
					if (edge->top < top) top = edge->top;
					if (edge->bottom > bottom) bottom = edge->bottom;
					inactiveEdges = addEdge(inactiveEdges, edge, false);
					edge++;
				}
			}
			i2 = i1;
			i1 += 2;
		}

		if (bottom >= mHeight) bottom = mHeight - 1;

		// for each scanline in the polygon...
		for (c = top; c <= bottom; ++c)
		{
			// check for newly active edges
			edge = inactiveEdges;
			while ((edge) && (edge->top == c))
			{
				nextEdge = edge->next;
				inactiveEdges = removeEdge(inactiveEdges, edge);
				activeEdges = addEdge(activeEdges, edge, true);
				edge = nextEdge;
			}

			// draw horizontal line segments
			edge = activeEdges;
			while ((edge) && (edge->next))
			{
				DrawLine(edge->x >> PolygonFixedShift, c, (edge->next->x + edge->next->w) >> PolygonFixedShift, c, colour); // todo;
				edge = edge->next->next;
			}

			// update edges, sorting and removing dead ones
			edge = activeEdges;
			while (edge)
			{
				nextEdge = edge->next;
				if (c >= edge->bottom)
				{
					activeEdges = removeEdge(activeEdges, edge);
				}
				else
				{
					edge->x += edge->dx;
					while ((edge->prev) && (edge->x + edge->w/2 < edge->prev->x + edge->prev->w/2))
					{
						if (edge->next) edge->next->prev = edge->prev;
						edge->prev->next = edge->next;
						edge->next = edge->prev;
						edge->prev = edge->prev->prev;
						edge->next->prev = edge;
						if (edge->prev)
						{
							edge->prev->next = edge;
						}
						else
						{
							activeEdges = edge;
						}
					}
				}
				edge = nextEdge;
			}
		}
	}


};

#endif
