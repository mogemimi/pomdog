//======================================================================
//-----------------------------------------------------------------------
/**
 * @file		iutest_constant.hpp
 * @brief		iris unit test 定数 定義 ファイル
 *
 * @author		t.shirayanagi
 * @par			copyright
 * Copyright (C) 2011-2014, Takazumi Shirayanagi\n
 * This software is released under the new BSD License,
 * see LICENSE
*/
//-----------------------------------------------------------------------
//======================================================================
#ifndef INCG_IRIS_IUTEST_CONSTANT_HPP_37DDDC13_3259_42E0_A648_47B064DED4BA_
#define INCG_IRIS_IUTEST_CONSTANT_HPP_37DDDC13_3259_42E0_A648_47B064DED4BA_

//======================================================================
// include

namespace iutest {
namespace detail
{

namespace helper
{

/**
 * @brief	文字列定数
*/
template<typename DMY>
struct kStringsT
{
	static const char* const DefaultXmlReportFileName;	//!< デフォルト xml 出力ファイル名
	static const char* const UnkownFile;				//!< unkown file
	static const char* const Null;						//!< NULL
};

template<typename DMY>
const char* const kStringsT<DMY>::DefaultXmlReportFileName =  "test_detail.xml";
template<typename DMY>
const char* const kStringsT<DMY>::UnkownFile =  "unkown file";
template<typename DMY>
const char* const kStringsT<DMY>::Null =  "(null)";

#if IUTEST_HAS_LIB

#if IUTEST_HAS_EXTERN_TEMPLATE

IUTEST_PRAGMA_EXTERN_TEMPLATE_WARN_DISABLE_BEGIN()

extern template struct kStringsT<void>;

IUTEST_PRAGMA_EXTERN_TEMPLATE_WARN_DISABLE_END()

#else

template struct kStringsT<void>;

#endif
#endif

}	// end of namespace helper

/** @internal */
typedef helper::kStringsT<void> kStrings;

/**
 * @brief	定数群
*/
struct kValues
{
#if defined(IUTEST_NO_INCLASS_MEMBER_INITIALIZATION)
	enum
	{
		MaxPrintContainerCount = 32
		, PrintArrayThreshold = 16
		, PrintArrayChunksize = PrintArrayThreshold/2
	};
#else
	static const size_t MaxPrintContainerCount = 32;
	static const size_t PrintArrayThreshold = 18;
	static const size_t PrintArrayChunksize = PrintArrayThreshold/2;
#endif
};

}	// end of namespace detail
}	// end of namespace iutest

#endif // INCG_IRIS_IUTEST_CONSTANT_HPP_37DDDC13_3259_42E0_A648_47B064DED4BA_
