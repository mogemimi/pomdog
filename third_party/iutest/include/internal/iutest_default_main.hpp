//======================================================================
//-----------------------------------------------------------------------
/**
 * @file		iutest_default_main.hpp
 * @brief		iris unit test default main
 *
 * @author		t.shirayanagi
 * @par			copyright
 * Copyright (C) 2014, Takazumi Shirayanagi\n
 * This software is released under the new BSD License,
 * see LICENSE
*/
//-----------------------------------------------------------------------
//======================================================================
#ifndef INCG_IRIS_IUTEST_DEFAULT_MAIN_HPP_954CE73E_3002_4339_AE91_030BDE594F8F_
#define INCG_IRIS_IUTEST_DEFAULT_MAIN_HPP_954CE73E_3002_4339_AE91_030BDE594F8F_

//======================================================================
// function

/**
 * @brief	default main
*/
#ifdef UNICODE
int wmain(int argc, wchar_t** argv)
#else
int main(int argc, char** argv)
#endif
{
#if !defined(IUTEST_OS_WINDOWS_MOBILE)
	setlocale(LC_CTYPE, "");
#endif

	IUTEST_INIT(&argc, argv);
	return IUTEST_RUN_ALL_TESTS();
}

#endif // INCG_IRIS_IUTEST_DEFAULT_MAIN_HPP_954CE73E_3002_4339_AE91_030BDE594F8F_
