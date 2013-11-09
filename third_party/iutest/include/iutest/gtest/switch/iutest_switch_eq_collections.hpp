//======================================================================
//-----------------------------------------------------------------------
/**
 * @file		iutest_switch_eq_collections.hpp
 * @brief		IUTEST_*_EQ_COLLECTIONS 切り替え定義 ファイル
 *
 * @author		t.sirayanagi
 * @version		1.0
 *
 * @par			copyright
 * Copyright (C) 2012-2013, Takazumi Shirayanagi\n
 * This software is released under the new BSD License,
 * see LICENSE
*/
//-----------------------------------------------------------------------
//======================================================================
#ifndef INCG_IRIS_iutest_switch_eq_collections_HPP_F9FF4E85_4E30_4ed8_AE80_8401D2189879_
#define INCG_IRIS_iutest_switch_eq_collections_HPP_F9FF4E85_4E30_4ed8_AE80_8401D2189879_

#if !defined(IUTEST_USE_GTEST)

//======================================================================
// define

#else

//======================================================================
// define
#ifndef IUTEST_ASSERT_EQ_COLLECTIONS
#  define	IUTEST_ASSERT_EQ_COLLECTIONS(b1, e1, b2, e2)	IUTEST_TEST_EQ_COLLECTIONS(b1, e1, b2, e2, IUTEST_ASSERT_FAILURE)
#endif
#ifndef IUTEST_EXPECT_EQ_COLLECTIONS
#  define	IUTEST_EXPECT_EQ_COLLECTIONS(b1, e1, b2, e2)	IUTEST_TEST_EQ_COLLECTIONS(b1, e1, b2, e2, IUTEST_EXPECT_FAILURE)
#endif
#ifndef IUTEST_INFORM_EQ_COLLECTIONS
#  define	IUTEST_INFORM_EQ_COLLECTIONS(b1, e1, b2, e2)	IUTEST_TEST_EQ_COLLECTIONS(b1, e1, b2, e2, IUTEST_INFORM_FAILURE)
#endif

#ifndef IUTEST_TEST_EQ_COLLECTIONS
#  define IUTEST_TEST_EQ_COLLECTIONS(b1, e1, b2, e2, on_failure) IUTEST_PRED_FORMAT4_( ::testing::internal::CmpHelperEqCollections, b1, e1, b2, e2, on_failure)
#endif

//======================================================================
// function
namespace testing
{

namespace internal
{

template<typename T1, typename T2>
AssertionResult IUTEST_ATTRIBUTE_UNUSED_ CmpHelperEqCollections(const char* expr1b, const char* expr1e, const char* expr2b, const char* expr2e
	, T1 b1, T1 e1, T2 b2, T2 e2)
{
	int elem=0;
	bool result = true;
	Message ar;
	for( elem=0; b1 != e1 && b2 != e2; ++b1, ++b2, ++elem )
	{
		if( !EqHelper<false>::Compare("", "", *b1, *b2) )
		{
			result = false;
			ar << "\nMismatch in a position " << elem << ": "
				<< FormatForComparisonFailureMessage(*b1, *b2)
				<< " vs " << FormatForComparisonFailureMessage(*b2, *b1);
		}
	}
	if( b1 != e1 )
	{
		int elem1 = elem;
		for( ; b1 != e1; ++b1, ++elem1 )
			;
		result = false;
		ar << "\nMismatch element : " << elem1 << " vs " << elem;
	}
	if( b2 != e2 )
	{
		int elem2 = elem;
		for( ; b2 != e2; ++b2, ++elem2 )
			;
		result = false;
		ar << "\nMismatch element : " << elem << " vs " << elem2;
	}
	if( !result )
	{
		return AssertionFailure() << "error: Expected: { " << expr1b << ", " << expr1e << " } == { "
			<< expr2b << ", " << expr2e << " }\n  Actual:" << ar;
	}
	return AssertionSuccess();
}

}	// end of namespace internal
}	// end of namespace testing

#endif

#endif
