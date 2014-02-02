//======================================================================
//-----------------------------------------------------------------------
/**
 * @file		iutest_charcode.hpp
 * @brief		iris unit test 文字コード対応 ファイル
 *
 * @author		t.sirayanagi
 * @version		1.0
 *
 * @par			copyright
 * Copyright (C) 2011-2014, Takazumi Shirayanagi\n
 * This software is released under the new BSD License,
 * see LICENSE
*/
//-----------------------------------------------------------------------
//======================================================================
#ifndef INCG_IRIS_IUTEST_CHARCODE_HPP_D444FB3E_3AFA_46D0_AD69_33FAAF5615E3_
#define INCG_IRIS_IUTEST_CHARCODE_HPP_D444FB3E_3AFA_46D0_AD69_33FAAF5615E3_

//======================================================================
// include
#include "iutest_port.hpp"

#if IUTEST_HAS_CXX_HDR_CODECVT
#  include <codecvt>
#endif

IUTEST_PRAGMA_CRT_SECURE_WARN_DISABLE_BEGIN()

namespace iutest {
namespace detail
{

//======================================================================
// declare
/**
 * @brief	ワイド文字列からUTF8へ変換
 * @param [in]	str	= 入力
 * @param [in]	num = 入力バッファサイズ
 * @return	UTF8 文字列
*/
::std::string WideStringToUTF8(const wchar_t* str, int num=-1);

/**
* @brief	ワイド文字列からマルチバイトへ変換
* @param [in]	str	= 入力
* @param [in]	num = 入力バッファサイズ
* @return	マルチバイト文字列
*/
::std::string WideStringToMultiByteString(const wchar_t* str, int num=-1);

/**
* @brief	マルチバイト文字からUTF8へ変換
* @param [in]	str	= 入力
* @param [in]	num = 入力バッファサイズ
* @return	UTF8 文字列
*/
::std::string MultiByteStringToUTF8(const char* src, int num=-1);

/**
* @brief	ワイド文字列から ::std::string へ変換
* @param [in]	wide_c_str	= 入力
* @return	string
*/
::std::string ShowWideCString(const wchar_t* wide_c_str);

#if IUTEST_HAS_CXX_HDR_CODECVT
/**
* @brief	UTF16文字列からUTF8へ変換
* @param [in]	str	= 入力
* @param [in]	num = 入力バッファサイズ
* @return	UTF8 文字列
*/
#if IUTEST_HAS_CHAR16_T
::std::string UTF16ToUTF8(const char16_t* str, int num=-1);
#else
::std::string UTF16ToUTF8(const wchar_t* str, int num=-1);
#endif

#endif

//======================================================================
// function

inline ::std::string ShowWideCString(const wchar_t* wide_c_str)
{
	if( wide_c_str == NULL )
	{
		return "(null)";
	}
#if IUTEST_MBS_CODE == IUTEST_MBS_CODE_UTF8
	return WideStringToUTF8(wide_c_str);
#elif defined(IUTEST_OS_WINDOWS) && IUTEST_MBS_CODE == IUTEST_MBS_CODE_WINDOWS31J
	return win::WideStringToMultiByteString(wide_c_str);
#else
	return WideStringToMultiByteString(wide_c_str);
#endif
}

#if IUTEST_HAS_CHAR16_T
inline ::std::string ShowWideCString(const char16_t* wide_c_str)
{
	if(wide_c_str == NULL)
	{
		return "(null)";
	}
#if IUTEST_HAS_CXX_HDR_CODECVT
#if IUTEST_MBS_CODE == IUTEST_MBS_CODE_UTF8
	return UTF16ToUTF8(wide_c_str);
#else
	return ShowWideCString(reinterpret_cast<const wchar_t*>(wide_c_str));
	//return UTF16ToMultiByteString(wide_c_str);
#endif
#else
	return ShowWideCString(reinterpret_cast<const wchar_t*>(wide_c_str));
#endif
}
#endif

//======================================================================
// struct
namespace mbs_ptr_impl
{
	template<typename T>
	struct to_mbs_ptr
	{
		const char* ptr(const char* arg) { return arg; }
	};
	struct wcs_to_mbs_ptr
	{
		::std::string m_arg;
		const char* ptr(const wchar_t* arg) 
		{
			m_arg = ShowWideCString(arg);
			return m_arg.c_str();
		}
	};
	template<>
	struct to_mbs_ptr<wchar_t> : public wcs_to_mbs_ptr {};
	template<>
	struct to_mbs_ptr<const wchar_t> : public wcs_to_mbs_ptr {};
}	// end of namespace mbs_ptr_impl

/**
 * @brief	mbs_ptr
*/
template<typename CharType>
struct mbs_ptr : public mbs_ptr_impl::to_mbs_ptr<CharType>
{
};

IUTEST_PRAGMA_CRT_SECURE_WARN_DISABLE_END()

}	// end of namespace detail
}	// end of namespace iutest

#if !IUTEST_HAS_LIB
#  include "../impl/iutest_charcode.ipp"
#endif

#endif	// INCG_IRIS_IUTEST_CHARCODE_HPP_D444FB3E_3AFA_46D0_AD69_33FAAF5615E3_
