#include "lg/lg.h"
#include "lg/string.h"
#include "lg/swap.h"
#include <cstdlib>
#include <cstring>
#include <cstdarg>

namespace luvafair
{
	String::String(const char *text)
	{
		if (text)
		{
			mLength = (int)strlen(text);
			mText = new char[mLength + 1];
			strcpy(mText, text);
		}
		else
		{
			mLength = 0;
			mText = new char[1];
			mText[0] = 0;
		}
	}

	String::String(const char *text, unsigned int length)
	{
		assert(text != 0 || length == 0);
		if (text)
		{
			mLength = length;
			mText = new char[mLength + 1];
			memcpy(mText, text, mLength);
			mText[mLength] = 0;
		}
		else
		{
			mLength = 0;
			mText = new char[1];
			mText[0] = 0;
		}
	}


	String::String(const String& string)
	{
		mLength = string.mLength;
		mText = new char[mLength + 1];
		memcpy(mText, string.mText, mLength + 1);
	}

	String::~String()
	{
		delete [] mText;
	}

	void String::Swap(String& rhs)
	{
		luvafair::Swap(mLength, rhs.mLength);
		luvafair::Swap(mText, rhs.mText);
	}

	String& String::operator=(const String& rhs)
	{
		String copy(rhs);
		Swap(copy);
		return *this;
	}

	String& String::operator+=(const String& rhs)
	{
		String copy(*this + rhs);
		Swap(copy);
		return *this;
	}

	String& String::operator+=(char c)
	{
		String copy(*this + c);
		Swap(copy);
		return *this;
	}

	String String::operator+(const String& rhs) const
	{
		String copy(*this);
		copy.mLength += rhs.mLength;
		char *newText = new char[copy.mLength + 1];
		strcpy(newText, mText);
		strcat(newText, rhs.mText);
		delete [] copy.mText;
		copy.mText = newText;
		return copy;
	}

	String String::operator+(char c) const
	{
		String copy(*this);
		copy.mLength++;
		char *newText = new char[copy.mLength + 1];
		strcpy(newText, mText);
		newText[copy.mLength - 1] = c;
		newText[copy.mLength] = 0;
		delete [] copy.mText;
		copy.mText = newText;
		return copy;
	}

	bool String::Equals(const String& rhs) const
	{
		return mLength == rhs.mLength && memcmp(mText, rhs.mText, mLength) == 0;
	}

	bool operator==(const String& lhs, const String& rhs) { return lhs.Equals(rhs); }
	bool operator!=(const String& lhs, const String& rhs) { return !(lhs.Equals(rhs)); }

	String ToString(int t)
	{
		char buf[33];
		sprintf(buf, "%d", t);
		return String(buf);
	}

	String ToString(unsigned int t)
	{
		char buf[33];
		sprintf(buf, "%d", t);
		return String(buf);
	}

	String ToString(float t)
	{
		char buf[128];
		sprintf(buf, "%f", t);
		return String(buf);
	}

	String ToString(const String& t)
	{
		return t;
	}

	String ToString(const char* t)
	{
		return String(t);
	}

	String TicksAsTime(int t)
	{
		char buf[128];
		sprintf(buf, "%d:%02d", t / 60 / 60, t / 60 % 60);
		return String(buf);
	}

	String Prec(float t, const char* spec)
	{
		char fmt[128];
		char buf[256];
		sprintf(fmt, "%%%sf", spec);
		sprintf(buf, fmt, t);
		return String(buf);
	}

	String InternalFormat(const char* fmt, int num, ...)
	{
		String ret;

		const char* args[10]; // we only support this many in the templates anyway
		assert(num <= (int)(sizeof(args)/sizeof(args[0])));

		va_list marker;
		va_start(marker, num);
		for (int i = 0; i < num; ++i)
		{
			args[i] = va_arg(marker, const char*);
		}
		va_end(marker);

		const char *s = fmt;
		for (;;)
		{
			if (*s == 0) break;
			if (*s == '%')
			{
				++s;
				assert(*s == '%' || (*s >= '0' && *s <= '9'));
				if (*s == '%')
				{
					ret += '%';
					++s;
				}
				else if (*s >= '0' && *s <= '9')
				{
					int which = *s - '0';
					++s;
					assert(which < num);
					ret += args[which];
				}
				else break;
			}
			else
			{
				ret += *s++;
			}
		}
		return ret;
	}

}
