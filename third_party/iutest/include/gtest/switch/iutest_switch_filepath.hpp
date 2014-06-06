//======================================================================
//-----------------------------------------------------------------------
/**
 * @file		iutest_switch_filepath.hpp
 * @brief		FilePath 切り替え ファイル
 *
 * @author		t.shirayanagi
 * @par			copyright
 * Copyright (C) 2013-2014, Takazumi Shirayanagi\n
 * This software is released under the new BSD License,
 * see LICENSE
*/
//-----------------------------------------------------------------------
//======================================================================
#ifndef INCG_IRIS_IUTEST_SWITCH_FILEPATH_HPP_D92E9CDC_9C63_4008_BE35_6E32177B28D2_
#define INCG_IRIS_IUTEST_SWITCH_FILEPATH_HPP_D92E9CDC_9C63_4008_BE35_6E32177B28D2_

#if !defined(IUTEST_USE_GTEST)

#else // !defined(IUTEST_USE_GTEST)

namespace testing {
namespace internal
{

inline bool operator == (const FilePath& lhs
						 , const FilePath& rhs)
{
	return posix::StrCaseCmp(lhs.c_str(), rhs.c_str()) == 0;
}

inline bool operator == (const FilePath& lhs
						 , const char* rhs)
{
	return posix::StrCaseCmp(lhs.c_str(), rhs) == 0;
}

inline bool operator == (const char* lhs
						 , const FilePath& rhs)
{
	return posix::StrCaseCmp(lhs, rhs.c_str()) == 0;
}

inline ::std::ostream& operator << (::std::ostream& os, const FilePath& path)
{
	return os << path.c_str();
}

}	// end of namespace internal
}	// end of namespace testing

#endif // !defined(IUTEST_USE_GTEST)

#endif // INCG_IRIS_IUTEST_SWITCH_FILEPATH_HPP_D92E9CDC_9C63_4008_BE35_6E32177B28D2_
