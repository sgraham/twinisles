#ifndef LG_INCLUDED_gui_H
#define LG_INCLUDED_gui_H

class Screen;
#include "lg/font.h"
#include "lg/v2.h"

class GuiItem
{
public:
	virtual ~GuiItem() {}
	virtual bool TapTest(const V2& p, Screen* screen) { return false; }
	virtual void Draw(u16* bg) = 0;
};

typedef void (*ButtonCallback)(GuiItem* item, Screen* screen, void* p);

class Button : public GuiItem
{
	const char* mText;
	int x, y;
	int w, h;
	ButtonCallback mCallback;
	void* mUser;

public:
	Button(const char* text, int x, int y, int w, int h, ButtonCallback func, void* user = 0)
		: mText(text), x(x), y(y), w(w), h(h), mCallback(func), mUser(user) {}

	virtual void Draw(u16* bg);
	void SetCallback(ButtonCallback func);
	void SetText(const char* text);
	virtual bool TapTest(const V2& p, Screen* screen);
};

class Label : public GuiItem
{
	const char* mText;
	int x, y;
	u16 mColour;
	luvafair::TextAlignment mTA;

public:
	Label(const char* text, int x, int y, u16 colour, luvafair::TextAlignment ta) : mText(text), x(x), y(y), mColour(colour), mTA(ta) {}
	virtual void Draw(u16* bg);
};

class WrappedLabel : public GuiItem
{
	const char* mText;
	int x, y;
	int w, h;
	u16 mColour;

public:
	WrappedLabel(const char* text, int x, int y, int w, int h, u16 colour) : mText(text), x(x), y(y), w(w), h(h), mColour(colour) {}
	virtual void Draw(u16* bg);
	void SetText(const char* text);
};

class Image : public GuiItem
{
	u16* mImage;
	int x, y;
	int w, h;
public:
	Image(u16* image, int x, int y, int w, int h)
		: mImage(image), x(x), y(y), w(w), h(h) {}

	virtual void Draw(u16* bg);
};

class Background : public GuiItem
{
	u16 mColour;
public:
	Background(u16 colour) : mColour(colour) {}
	virtual void Draw(u16* bg);
};

enum ArrowType
{
	Arrow_Left,
	Arrow_Right,
};

class Arrow : public GuiItem
{
	int x, y;
	int w, h;
	ArrowType mType;
	ButtonCallback mCallback;
	void* mUser;

	public:
		Arrow(int x, int y, int w, int h, ArrowType type, ButtonCallback func, void* user = 0)
			: x(x), y(y), w(w), h(h), mType(type), mCallback(func), mUser(user)
		{
			while (w % 2 != 0) --w;
			while (h % 3 != 0) --h;
		}

		virtual void Draw(u16* bg);
		virtual bool TapTest(const V2& p, Screen* screen);
};

class Screen
{
	u16* mBG;
	GuiItem* mItems[10];
	int mNumItems;

public:
	Screen(u16* bg) : mBG(bg), mNumItems(0) {}

	void Add(GuiItem* item);
	void Tap(const V2& p);
	void Draw();
	~Screen();
};

#endif
