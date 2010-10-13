#ifndef LG_INCLUDED_lg_string_H
#define LG_INCLUDED_lg_string_H

#include "lg/lg.h"

namespace luvafair
{
	class String
	{
		public:
			String(const char *text = 0);
			String(const char *text, unsigned int length);
			String(const String& string);
			~String();

			int Length() const { return mLength; }
			const char *c_str() const { return mText; }

			String& operator=(const String& rhs);
			String& operator+=(const String& rhs);
			String& operator+=(char c);
			String operator+(const String& rhs) const;
			String operator+(char c) const;
			bool Equals(const String& rhs) const;
			const char& operator[](int index) const { assert(index >= 0 && index <= mLength); return mText[index]; } // allow == length to get \0 at end

			void Swap(String& rhs);

		private:
			int mLength;	
			char *mText;
	};


	String ToString(int t);
	String ToString(unsigned int t);
	String ToString(float t);
	String ToString(const String& t);
	String ToString(const char* t);
	String TicksAsTime(int t);
	String Prec(float t, const char* spec);

	String InternalFormat(const char* fmt, int num, ...);

	inline String Format(const char* fmt) { return String(fmt); }
	template <class T0> String Format(const char* fmt, const T0& t0) { return InternalFormat(fmt, 1, ToString(t0).c_str()); }
	template <class T0, class T1> String Format(const char* fmt, const T0& t0, const T1& t1) { return InternalFormat(fmt, 2, ToString(t0).c_str(), ToString(t1).c_str()); }
	template <class T0, class T1, class T2> String Format(const char* fmt, const T0& t0, const T1& t1, const T2& t2)  { return InternalFormat(fmt, 3, ToString(t0).c_str(), ToString(t1).c_str(), ToString(t2).c_str()); }
	template <class T0, class T1, class T2, class T3> String Format(const char* fmt, const T0& t0, const T1& t1, const T2& t2, const T3& t3) { return InternalFormat(fmt, 4, ToString(t0).c_str(), ToString(t1).c_str(), ToString(t2).c_str(), ToString(t3).c_str()); }
	template <class T0, class T1, class T2, class T3, class T4> String Format(const char* fmt, const T0& t0, const T1& t1, const T2& t2, const T3& t3, const T4& t4) { return InternalFormat(fmt, 5, ToString(t0).c_str(), ToString(t1).c_str(), ToString(t2).c_str(), ToString(t3).c_str(), ToString(t4).c_str()); }
	template <class T0, class T1, class T2, class T3, class T4, class T5> String Format(const char* fmt, const T0& t0, const T1& t1, const T2& t2, const T3& t3, const T4& t4, const T5& t5) { return InternalFormat(fmt, 6, ToString(t0).c_str(), ToString(t1).c_str(), ToString(t2).c_str(), ToString(t3).c_str(), ToString(t4).c_str(), ToString(t5).c_str()); }
	template <class T0, class T1, class T2, class T3, class T4, class T5, class T6> String Format(const char* fmt, const T0& t0, const T1& t1, const T2& t2, const T3& t3, const T4& t4, const T5& t5, const T6& t6) { return InternalFormat(fmt, 7, ToString(t0).c_str(), ToString(t1).c_str(), ToString(t2).c_str(), ToString(t3).c_str(), ToString(t4).c_str(), ToString(t5).c_str(), ToString(t6).c_str()); }
	template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7> String Format(const char* fmt, const T0& t0, const T1& t1, const T2& t2, const T3& t3, const T4& t4, const T5& t5, const T6& t6, const T7& t7) { return InternalFormat(fmt, 8, ToString(t0).c_str(), ToString(t1).c_str(), ToString(t2).c_str(), ToString(t3).c_str(), ToString(t4).c_str(), ToString(t5).c_str(), ToString(t6).c_str(), ToString(t7).c_str()); }
	template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8> String Format(const char* fmt, const T0& t0, const T1& t1, const T2& t2, const T3& t3, const T4& t4, const T5& t5, const T6& t6, const T7& t7, const T8& t8) { return InternalFormat(fmt, 9, ToString(t0).c_str(), ToString(t1).c_str(), ToString(t2).c_str(), ToString(t3).c_str(), ToString(t4).c_str(), ToString(t5).c_str(), ToString(t6).c_str(), ToString(t7).c_str(), ToString(t8).c_str()); }
	template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9> String Format(const char* fmt, const T0& t0, const T1& t1, const T2& t2, const T3& t3, const T4& t4, const T5& t5, const T6& t6, const T7& t7, const T8& t8, const T9& t9) { return InternalFormat(fmt, 10, ToString(t0).c_str(), ToString(t1).c_str(), ToString(t2).c_str(), ToString(t3).c_str(), ToString(t4).c_str(), ToString(t5).c_str(), ToString(t6).c_str(), ToString(t7).c_str(), ToString(t8).c_str(), ToString(t9).c_str()); }
}

#endif
