//======================================================================
//-----------------------------------------------------------------------
/**
 * @file		iutest_expression_assertion.hpp
 * @brief		iris unit test 式アサーション 定義 ファイル
 *
 * @author		t.shirayanagi
 * @par			copyright
 * Copyright (C) 2014, Takazumi Shirayanagi\n
 * This software is released under the new BSD License,
 * see LICENSE
*/
//-----------------------------------------------------------------------
//======================================================================
#ifndef INCG_IRIS_IUTEST_EXPRESSION_ASSERTION_HPP_B9783316_33CF_4CA7_81D0_0BF44B048A4A_
#define INCG_IRIS_IUTEST_EXPRESSION_ASSERTION_HPP_B9783316_33CF_4CA7_81D0_0BF44B048A4A_

//======================================================================
// define
/**
 * @brief	式展開用 operand マクロ
 * @note	IUTEST_ASSERT(f() != 42 IUTEST_OPERAND(||) f() == 32 )
*/
#define IUTEST_OPERAND(op)		op IIUT_EXPRESSION_DECOMPOSE()

/**
 * @brief	式展開用マクロ
 * @note	IUTEST_ASSERT_TRUE(IUTEST_EXPRESSION(f() != 42) || IUTEST_EXPRESSION(f() == 10))
*/
#define IUTEST_EXPRESSION(expr)	(IIUT_EXPRESSION_DECOMPOSE() expr).GetResult()


/**
 * @private
 * @{
*/

#if IUTEST_HAS_ARITHMETIC_EXPRESSION_DECOMPOSE
#  define IIUT_EXPRESSION_DECOMPOSE()	::iutest::detail::ExpressionDecomposer()->*
#else
#  define IIUT_EXPRESSION_DECOMPOSE()	::iutest::detail::ExpressionDecomposer()>>
#endif

#define IUTEST_TEST_EXPRESSION_(expr, expected, on_failure)	\
	IUTEST_TEST_TRUE( ( IIUT_EXPRESSION_DECOMPOSE() expr ).GetResult(expected), #expr, on_failure )

#define IIUT_DECL_EXPRESSION_RESULT_OP(op)	\
	template<typename RHS>ExpressionResult operator op (const RHS& rhs) const {	\
		const bool b = result() op rhs ? true : false;							\
		return ExpressionResult(AssertionResult(b)								\
					<< m_result.message() << " " #op " " << rhs);				\
	}																			\
	ExpressionResult operator op (const ExpressionResult& rhs) const {			\
		const bool b = result() op rhs.result() ? true : false;					\
		return ExpressionResult(AssertionResult(b)								\
					<< m_result.message() << " " #op " " << rhs.message());		\
	}																			\
	ExpressionResult operator op (const AssertionResult& rhs) const {			\
		const bool b = result() op rhs.passed() ? true : false;					\
		return ExpressionResult(AssertionResult(b)								\
					<< m_result.message() << " " #op " " << rhs.message());		\
	}

#define IIUT_DECL_EXPRESSION_OP(op)	\
	template<typename RHS>ExpressionResult operator op (const RHS& rhs) const {			\
		const bool b = (m_lhs op rhs) ? true : false;									\
		return ExpressionResult(AssertionResult(b) << m_message << " " #op " " << rhs);	\
	}

#if IUTEST_HAS_ARITHMETIC_EXPRESSION_DECOMPOSE

#if IUTEST_HAS_DECLTYPE

#define IIUT_DECL_EXPRESSION_OP_LHS(op)	\
	template<typename RHS>auto operator op (const RHS& rhs) const	\
	-> ExpressionLHS< decltype( expression_op_helper::operand_result( ::std::declval<T>() op rhs) )> {	\
		return OperandResult(m_lhs op rhs) << " " #op " " << rhs;	\
	}

#else

#define IIUT_DECL_EXPRESSION_OP_LHS(op)	\
	template<typename RHS>ExpressionLHS<RHS> operator op (const RHS& rhs) const {	\
	return OperandResult(m_lhs op rhs) << " " #op " " << rhs;	\
	}

#endif

#endif

/**
 * @}
*/

namespace iutest {
namespace detail
{

//======================================================================
// class

/**
 * @brief	expression result
*/
class ExpressionResult
{
public:
	ExpressionResult(const AssertionResult& ar)
		: m_result(ar) {}

public:
	IIUT_DECL_EXPRESSION_RESULT_OP(||)
	IIUT_DECL_EXPRESSION_RESULT_OP(&&)

public:
	AssertionResult GetResult(bool expected) const 
	{
		return AssertionResult(result() == expected) << "expansion: " << m_result.message();
	}
	AssertionResult GetResult(void) const
	{
		return AssertionResult(result()) << m_result.message();
	}
private:
	bool result(void) const { return m_result.passed(); }
	const char* message(void) const { return m_result.message(); }
private:
	AssertionResult m_result;
};

namespace expression_op_helper
{
	template<typename T>
	T operand_result(const T&);
}

/**
 * @brief	expression lhs
 * @tparam	T=type
*/
template<typename T>
class ExpressionLHS
{
	typedef ExpressionLHS<T> _Myt;
public:
	ExpressionLHS(T lhs) : m_lhs(lhs)
	{
		AppendMessage(lhs);
	}
	ExpressionLHS(T lhs, const ::std::string& msg) : m_lhs(lhs), m_message(msg) {}

public:
	IIUT_DECL_EXPRESSION_OP(==)
	IIUT_DECL_EXPRESSION_OP(!=)
	IIUT_DECL_EXPRESSION_OP(<)
	IIUT_DECL_EXPRESSION_OP(<=)
	IIUT_DECL_EXPRESSION_OP(>)
	IIUT_DECL_EXPRESSION_OP(>=)
	IIUT_DECL_EXPRESSION_OP(&&)
	IIUT_DECL_EXPRESSION_OP(||)

#if IUTEST_HAS_ARITHMETIC_EXPRESSION_DECOMPOSE
	IIUT_DECL_EXPRESSION_OP_LHS(+)
	IIUT_DECL_EXPRESSION_OP_LHS(-)
	IIUT_DECL_EXPRESSION_OP_LHS(*)
	IIUT_DECL_EXPRESSION_OP_LHS(/)
	IIUT_DECL_EXPRESSION_OP_LHS(%)
#endif

#if IUTEST_HAS_BITWISE_EXPRESSION_DECOMPOSE
	IIUT_DECL_EXPRESSION_OP_LHS(&)
	IIUT_DECL_EXPRESSION_OP_LHS(|)
	IIUT_DECL_EXPRESSION_OP_LHS(^)
	IIUT_DECL_EXPRESSION_OP_LHS(<<)
	IIUT_DECL_EXPRESSION_OP_LHS(>>)
#endif

private:
	template<typename U>
	ExpressionLHS<U> OperandResult(const U& lhs) const
	{
		return ExpressionLHS<U>(lhs, m_message);
	}

public:
	/**
	 * @brief	メッセージ追加
	*/
	template<typename U>
	_Myt& operator << (const U& value)
	{
		AppendMessage(value);
		return *this;
	}

public:
	/** @private */
	AssertionResult GetResult(bool expected) const
	{
		const bool b = m_lhs ? true : false;
		return AssertionResult(b == expected) << "expansion: " << m_message;
	}
	AssertionResult GetResult(void) const
	{
		const bool b = m_lhs ? true : false;
		return AssertionResult(b) << m_message;
	}

private:
	template<typename U>
	void AppendMessage(const U& value)
	{
		Message msg;
		msg << value;
		m_message += msg.GetString();
	}

private:
	IUTEST_PP_DISALLOW_ASSIGN(ExpressionLHS);
private:
	T m_lhs;
	::std::string m_message;
};

#undef IIUT_DECL_EXPRESSION_RESULT_OP
#undef IIUT_DECL_EXPRESSION_OP
#ifdef IIUT_DECL_EXPRESSION_OP_LHS
#  undef IIUT_DECL_EXPRESSION_OP_LHS
#endif

/**
 * @brief	expression decomposer
*/
class ExpressionDecomposer
{
public:
#if IUTEST_HAS_ARITHMETIC_EXPRESSION_DECOMPOSE
	template<typename T>
	ExpressionLHS<const T&> operator ->*(const T& expr)
	{
		return ExpressionLHS<const T&>(expr);
	}
#else
	template<typename T>
	ExpressionLHS<const T&> operator >>(const T& expr)
	{
		return ExpressionLHS<const T&>(expr);
	}
#endif
};

}	// end of namespace detail
}	// end of namespace iutest

#endif // INCG_IRIS_IUTEST_EXPRESSION_ASSERTION_HPP_B9783316_33CF_4CA7_81D0_0BF44B048A4A_
