//======================================================================
//-----------------------------------------------------------------------
/**
 * @file		iutest_time.ipp
 * @brief		iris unit test time utility
 *
 * @author		t.shirayanagi
 * @par			copyright
 * Copyright (C) 2011-2014, Takazumi Shirayanagi\n
 * This software is released under the new BSD License,
 * see LICENSE
*/
//-----------------------------------------------------------------------
//======================================================================
#ifndef INCG_IRIS_IUTEST_TIME_IPP_C6B9E65B_4059_4175_A6DB_397DBF338AA8_
#define INCG_IRIS_IUTEST_TIME_IPP_C6B9E65B_4059_4175_A6DB_397DBF338AA8_

//======================================================================
// include
#include "../internal/iutest_time.hpp"

namespace iutest {
namespace detail
{

//======================================================================
// function
IUTEST_IPP_INLINE ::std::string FormatTimeInMillisecAsSecond(TimeInMillisec msec)
{
	detail::iuStringStream::type ss;
#if defined(_MSC_VER) && _MSC_VER < 1300
	ss << static_cast<unsigned int>(msec)/1000.0;
#else
	ss << msec/1000.0;
#endif
	return ss.str();
}

IUTEST_IPP_INLINE ::std::string FormatTimeInMillisecAsIso8601(TimeInMillisec msec)
{
#if IUTEST_HAS_CTIME
	time_t sec = static_cast<time_t>(msec / 1000);
IUTEST_PRAGMA_CRT_SECURE_WARN_DISABLE_BEGIN()
	const struct tm* const t = localtime(&sec);
IUTEST_PRAGMA_CRT_SECURE_WARN_DISABLE_END()
	if( t == NULL )
	{
		return "";
	}

	detail::iuStringStream::type ss;
	ss << (t->tm_year+1900);
	return ss.str() + "-"
		+ FormatIntWidth2(t->tm_mon+1) + "-"
		+ FormatIntWidth2(t->tm_mday) + "T"
		+ FormatIntWidth2(t->tm_hour) + ":"
		+ FormatIntWidth2(t->tm_min) + ":"
		+ FormatIntWidth2(t->tm_sec);
#else
	return FormatTimeInMillisecAsSecond(msec);
#endif
}

IUTEST_IPP_INLINE time_t GetTime(void)
{
#if IUTEST_HAS_CTIME
	return time(NULL);
#else
	return 0;
#endif
}

IUTEST_IPP_INLINE TimeInMillisec GetTimeInMillis(void)
{
#if   defined(IUTEST_GetMillisec)
	return IUTEST_GetMillisec();

#elif IUTEST_HAS_CXX11_HDR_CHRONO
	return ::std::chrono::duration_cast< ::std::chrono::milliseconds>(::std::chrono::high_resolution_clock::now().time_since_epoch()).count();

#elif IUTEST_HAS_GETTIMEOFDAY
	timeval tv;
	gettimeofday(&tv, NULL);
	return static_cast<TimeInMillisec>(tv.tv_sec) * 1000 + static_cast<TimeInMillisec>(tv.tv_usec) / 1000;

#elif defined(IUTEST_OS_WINDOWS)

#if defined(IUTEST_OS_WINDOWS_MOBILE)
	return static_cast<TimeInMillisec>(GetTickCount());
#else

	_timeb tb;

IUTEST_PRAGMA_CRT_SECURE_WARN_DISABLE_BEGIN()
	_ftime(&tb);
IUTEST_PRAGMA_CRT_SECURE_WARN_DISABLE_END()

	return static_cast<TimeInMillisec>(tb.time * 1000 + tb.millitm);

#endif

#elif IUTEST_HAS_CLOCK
	return clock() * 1000 / CLOCKS_PER_SEC;

#else

#define IUTEST_NO_GETTIMEINMILLIS
	return GetTime()*1000;
#endif
}

IUTEST_IPP_INLINE unsigned int GetIndefiniteValue(void)
{
#if !defined(IUTEST_NO_GETTIMEINMILLIS)
	return static_cast<unsigned int>(GetTimeInMillis());
#else
	// なるべく同じにならないようにする
	static unsigned int s = 20120206;
	s = s*1664525 + 1013904223;
	return s;
#endif
}

}	// end of namespace detail
}	// end of namespace iutest

#endif // INCG_IRIS_IUTEST_TIME_IPP_C6B9E65B_4059_4175_A6DB_397DBF338AA8_
