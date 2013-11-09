//======================================================================
//-----------------------------------------------------------------------
/**
 * @file		iutest_switch_throw_value.hpp
 * @brief		IUTEST_*_THROW_VALUE アサーション切り替え ファイル
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
#ifndef INCG_IRIS_iutest_switch_throw_value_HPP_5D6B75EE_F445_4730_8FEA_FB6249BCBD9C_
#define INCG_IRIS_iutest_switch_throw_value_HPP_5D6B75EE_F445_4730_8FEA_FB6249BCBD9C_

#if !defined(IUTEST_USE_GTEST)

//======================================================================
// define

#else

//======================================================================
// undef
#ifdef INCG_IRIS_iutest_HPP_

#undef IUTEST_ASSERT_THROW_VALUE_EQ
#undef IUTEST_ASSERT_THROW_VALUE_NE
#undef IUTEST_ASSERT_THROW_VALUE_STREQ
#undef IUTEST_ASSERT_THROW_VALUE_STRCASEEQ

#undef IUTEST_EXPECT_THROW_VALUE_EQ
#undef IUTEST_EXPECT_THROW_VALUE_NE
#undef IUTEST_EXPECT_THROW_VALUE_STREQ
#undef IUTEST_EXPECT_THROW_VALUE_STRCASEEQ

#undef IUTEST_INFORM_THROW_VALUE_EQ
#undef IUTEST_INFORM_THROW_VALUE_NE
#undef IUTEST_INFORM_THROW_VALUE_STREQ
#undef IUTEST_INFORM_THROW_VALUE_STRCAEEQ


#undef IUTEST_TEST_THROW_VALUE_
#undef IUTEST_TEST_THROW_VALUE_EQ_
#undef IUTEST_TEST_THROW_VALUE_NE_
#undef IUTEST_TEST_THROW_VALUE_STREQ_
#undef IUTEST_TEST_THROW_VALUE_STRCASEEQ_

#endif

//======================================================================
// define
#ifndef IUTEST_ASSERT_THROW_PRED_FORMAT2
#  define IUTEST_ASSERT_THROW_PRED_FORMAT2(pred_formatter, statement, expected_exception, expected_exception_value)		\
	IUTEST_THROW_PRED_FORMAT2_(pred_formatter, statement, expected_exception, expected_exception_value, IUTEST_ASSERT_FAILURE)
#endif

#ifndef IUTEST_EXPECT_THROW_PRED_FORMAT2
#  define IUTEST_EXPECT_THROW_PRED_FORMAT2(pred_formatter, statement, expected_exception, expected_exception_value)		\
	IUTEST_THROW_PRED_FORMAT2_(pred_formatter, statement, expected_exception, expected_exception_value, IUTEST_EXPECT_FAILURE)
#endif

#ifndef IUTEST_INFORM_THROW_PRED_FORMAT2
#  define IUTEST_INFORM_THROW_PRED_FORMAT2(pred_formatter, statement, expected_exception, expected_exception_value)		\
	IUTEST_THROW_PRED_FORMAT2_(pred_formatter, statement, expected_exception, expected_exception_value, IUTEST_INFORM_FAILURE)
#endif

// ASSERT
#define IUTEST_ASSERT_THROW_VALUE_EQ(statement, expected_exception, expected_value)	IUTEST_TEST_THROW_VALUE_EQ_(statement, expected_exception, expected_value, IUTEST_ASSERT_FAILURE)
#define IUTEST_ASSERT_THROW_VALUE_NE(statement, expected_exception, expected_value)	IUTEST_TEST_THROW_VALUE_NE_(statement, expected_exception, expected_value, IUTEST_ASSERT_FAILURE)
#define IUTEST_ASSERT_THROW_VALUE_STREQ(statement, expected_exception, expected_value)	IUTEST_TEST_THROW_VALUE_STREQ_(statement, expected_exception, expected_value, IUTEST_ASSERT_FAILURE)
#define IUTEST_ASSERT_THROW_VALUE_STRCASEEQ(statement, expected_exception, expected_value)	IUTEST_TEST_THROW_VALUE_STRCASEEQ_(statement, expected_exception, expected_value, IUTEST_ASSERT_FAILURE)

// EXPECT
#define IUTEST_EXPECT_THROW_VALUE_EQ(statement, expected_exception, expected_value)	IUTEST_TEST_THROW_VALUE_EQ_(statement, expected_exception, expected_value, IUTEST_EXPECT_FAILURE)
#define IUTEST_EXPECT_THROW_VALUE_NE(statement, expected_exception, expected_value)	IUTEST_TEST_THROW_VALUE_NE_(statement, expected_exception, expected_value, IUTEST_EXPECT_FAILURE)
#define IUTEST_EXPECT_THROW_VALUE_STREQ(statement, expected_exception, expected_value)	IUTEST_TEST_THROW_VALUE_STREQ_(statement, expected_exception, expected_value, IUTEST_EXPECT_FAILURE)
#define IUTEST_EXPECT_THROW_VALUE_STRCASEEQ(statement, expected_exception, expected_value)	IUTEST_TEST_THROW_VALUE_STRCASEEQ_(statement, expected_exception, expected_value, IUTEST_EXPECT_FAILURE)

// INFORM
#define IUTEST_INFORM_THROW(statement, expected_exception)	INFORM_THROW((void)(statement), expected_exception)
#define IUTEST_INFORM_THROW_VALUE_EQ(statement, expected_exception, expected_value)	IUTEST_TEST_THROW_VALUE_EQ_(statement, expected_exception, expected_value, IUTEST_INFORM_FAILURE)
#define IUTEST_INFORM_THROW_VALUE_NE(statement, expected_exception, expected_value)	IUTEST_TEST_THROW_VALUE_NE_(statement, expected_exception, expected_value, IUTEST_INFORM_FAILURE)
#define IUTEST_INFORM_THROW_VALUE_STREQ(statement, expected_exception, expected_value)	IUTEST_TEST_THROW_VALUE_STREQ_(statement, expected_exception, expected_value, IUTEST_INFORM_FAILURE)
#define IUTEST_INFORM_THROW_VALUE_STRCASEEQ(statement, expected_exception, expected_value)	IUTEST_TEST_THROW_VALUE_STRCASEEQ_(statement, expected_exception, expected_value, IUTEST_INFORM_FAILURE)


#define IUTEST_TEST_THROW_VALUE_(statement, expected_exception, expected_exception_value, on_failure, pred_formatter)	\
	IUTEST_AMBIGUOUS_ELSE_BLOCKER_													\
	if( ::testing::AssertionResult ar = ::testing::AssertionSuccess() ) {			\
		try {																		\
			IUTEST_SUPPRESS_UNREACHABLE_CODE_WARNING((void)(statement));			\
			ar << "\nExpected: " #statement " throws an exception of type "			\
					#expected_exception ".\n  Actual: it throws nothing.";			\
			goto IUTEST_PP_CAT(iutest_label_throw_value, __LINE__);					\
		} catch( expected_exception const& e) {										\
			if( ::testing::AssertionResult ar2 = pred_formatter("e", #expected_exception_value, e, expected_exception_value) ) {			\
			} else {																\
				ar << "\nExpected: " #statement " throws an exception of value\n" << ar2.message();	\
				goto IUTEST_PP_CAT(iutest_label_throw_value, __LINE__);				\
			}																		\
		} catch( ... ) {															\
			ar << "\nExpected: " #statement " throws an exception of type "			\
				#expected_exception ".\n  Actual: it throws a different type.";		\
			goto IUTEST_PP_CAT(iutest_label_throw_value, __LINE__);					\
		}																			\
	} else																			\
		IUTEST_PP_CAT(iutest_label_throw_value, __LINE__):							\
		on_failure(ar.message())

#define IUTEST_TEST_THROW_VALUE_EQ_(statement, expected_exception, expected_exception_value, on_failure)	\
	IUTEST_TEST_THROW_VALUE_(statement, expected_exception, expected_exception_value, on_failure, ::testing::internal::EqHelper<GTEST_IS_NULL_LITERAL_(expected_exception_value)>::Compare)
#define IUTEST_TEST_THROW_VALUE_NE_(statement, expected_exception, expected_exception_value, on_failure)	\
	IUTEST_TEST_THROW_VALUE_(statement, expected_exception, expected_exception_value, on_failure, ::testing::internal::NeHelper<GTEST_IS_NULL_LITERAL_(expected_exception_value)>::Compare)
#define IUTEST_TEST_THROW_VALUE_STREQ_(statement, expected_exception, expected_exception_value, on_failure)	\
	IUTEST_TEST_THROW_VALUE_(statement, expected_exception, expected_exception_value, on_failure, ::testing::internal::CmpHelperSTREQ)
#define IUTEST_TEST_THROW_VALUE_STRCASEEQ_(statement, expected_exception, expected_exception_value, on_failure)	\
	IUTEST_TEST_THROW_VALUE_(statement, expected_exception, expected_exception_value, on_failure, ::testing::internal::CmpHelperSTRCASEEQ)

#endif

#endif
