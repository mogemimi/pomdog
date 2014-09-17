//======================================================================
//-----------------------------------------------------------------------
/**
 * @file		iutest_util_menu.hpp
 * @brief		iris unit test テスト メニュー生成 ファイル
 *
 * @author		t.shirayanagi
 * @par			copyright
 * Copyright (C) 2013-2014, Takazumi Shirayanagi\n
 * This software is released under the new BSD License,
 * see LICENSE
*/
//-----------------------------------------------------------------------
//======================================================================
#ifndef INCG_IRIS_IUTEST_MENU_TESTS_HPP_52925DE1_A4AE_4CCB_B524_8E97AA73E03D_
#define INCG_IRIS_IUTEST_MENU_TESTS_HPP_52925DE1_A4AE_4CCB_B524_8E97AA73E03D_

//======================================================================
// include
#include "iutest_util_tests.hpp"

#if defined(IUTEST_OS_WINDOWS)
#include <windows.h>
#include <map>

namespace iuutil
{

//======================================================================
// class
/**
 * @brief	メニュークラス
*/
class TestMenu
{
	typedef ::std::map<WORD, const ::iutest::TestInfo*> TestInfoMap;
	typedef ::std::map<WORD, const ::iutest::TestCase*> TestCaseMap;
	WORD m_nID;
	const WORD m_nIDTop;
	TestInfoMap m_TestInfoList;
	TestCaseMap m_TestCaseList;
public:
	TestMenu(WORD nIDTop) : m_nIDTop(nIDTop), m_nID(nIDTop) {}
public:
	bool Create(HMENU hMenu)
	{
		if( hMenu == NULL )
		{
			return false;
		}

		// テストを列挙
		HMENU hRoot = AppendPopup(hMenu, "TestList");
		if( hRoot == NULL )
		{
			return false;
		}

		Append(hRoot, "以下をすべて実行", m_nID);
		++m_nID;

		::iutest::UnitTest* pUnitTest = ::iutest::UnitTest::GetInstance();
		const int testcase_count = pUnitTest->total_test_case_count();
		for( int i=0; i < testcase_count; ++i )
		{
			const ::iutest::TestCase* pTestCase = pUnitTest->GetTestCase(i);
			const int test_count = pTestCase->total_test_count();
			HMENU hTestCase = AppendPopup(hRoot, pTestCase->name());
			Append(hTestCase, "以下をすべて実行", m_nID);
#if IUTEST_HAS_STD_EMPLACE
			m_TestCaseList.emplace(m_nID, pTestCase);
#else
			m_TestCaseList.insert( ::std::pair<WORD, const ::iutest::TestCase*>(m_nID, pTestCase) );
#endif
			++m_nID;
			for( int j=0; j < test_count; ++j )
			{
				const ::iutest::TestInfo* pTestInfo = pTestCase->GetTestInfo(j);
				Append(hTestCase, pTestInfo->name(), m_nID);
#if IUTEST_HAS_STD_EMPLACE
				m_TestInfoList.emplace(m_nID, pTestInfo);
#else
				m_TestInfoList.insert(::std::pair<WORD, const ::iutest::TestInfo*>(m_nID, pTestInfo));
#endif
				++m_nID;
			}
		}
		return true;
	}

	bool OnCommand(WORD wID)
	{
		if( wID == m_nIDTop )
		{
			::iutest::IUTEST_FLAG(filter) = "*";
			return IUTEST_RUN_ALL_TESTS() == 0 ? true : false;
		}

		{
			TestInfoMap::iterator it = m_TestInfoList.find(wID);
			if( it != m_TestInfoList.end() )
			{
				::iutest::IUTEST_FLAG(filter) = TestFullName(it->second);
				return IUTEST_RUN_ALL_TESTS() == 0 ? true : false;
			}
		}
		{
			TestCaseMap::iterator it = m_TestCaseList.find(wID);
			if( it != m_TestCaseList.end() )
			{
				::std::string filter = it->second->name();
				filter += ".*";
				::iutest::IUTEST_FLAG(filter) = filter;
				return IUTEST_RUN_ALL_TESTS() == 0 ? true : false;
			}
		}
		return true;
	}

private:
	static bool Append(HMENU hMenu, const char* lpszName, WORD nID)
	{
		char str[256];
		MENUITEMINFOA mii = {0};
		mii.cbSize		= sizeof(mii);
		mii.fMask		= MIIM_ID | MIIM_TYPE;
		mii.fType		= MFT_STRING;
		mii.dwTypeData	= str;
		mii.wID			= nID;
		strcpy_s(str, lpszName);
		const int num = ::GetMenuItemCount(hMenu);
		if( !::InsertMenuItemA(hMenu, num, TRUE, &mii) )
		{
			return false;
		}
		return true;
	}
	static HMENU AppendPopup(HMENU hMenu, const char* lpszName)
	{
		HMENU hSubMenu = CreateMenu();
		char str[256];
		MENUITEMINFOA mii = {0};
		mii.cbSize		= sizeof(mii);
		mii.fMask		= MIIM_ID | MIIM_TYPE | MIIM_SUBMENU;
		mii.fType		= MFT_STRING;
		mii.dwTypeData	= str;
		mii.hSubMenu	= hSubMenu;
		strcpy_s(str, lpszName);
		const int num = ::GetMenuItemCount(hMenu);
		if( !::InsertMenuItemA(hMenu, num, TRUE, &mii) )
		{
			return NULL;
		}
		return hSubMenu;
	}
};

}	// end of namespace iuutil

#endif

#endif // INCG_IRIS_IUTEST_MENU_TESTS_HPP_52925DE1_A4AE_4CCB_B524_8E97AA73E03D_
