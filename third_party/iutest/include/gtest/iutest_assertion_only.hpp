//======================================================================
//-----------------------------------------------------------------------
/**
 * @file		iutest_assertion_only.hpp
 * @brief		assertion のみ利用するためのファイル
 *
 * @author		t.sirayanagi
 * @version		1.0
 *
 * @par			copyright
 * Copyright (C) 2012-2014, Takazumi Shirayanagi\n
 * This software is released under the new BSD License,
 * see LICENSE
*/
//-----------------------------------------------------------------------
//======================================================================
#ifndef INCG_IRIS_IUTEST_ASSERTION_ONLY_HPP_9E5FA1C8_EFB5_40A9_A3AD_971584ECAE56_
#define INCG_IRIS_IUTEST_ASSERTION_ONLY_HPP_9E5FA1C8_EFB5_40A9_A3AD_971584ECAE56_

#if !defined(IUTEST_USE_GTEST)

#else	// !defined(IUTEST_USE_GTEST)

#include "iutest_gtest_ver.hpp"
#if GTEST_OS_WINDOWS
#include <windows.h>
#endif

namespace testing
{
#if		GTEST_MINORVER == 0x07
	typedef ::std::string				GTestAssertionResultString;
	typedef ::std::string				GTestString;
#elif	GTEST_MINORVER == 0x06
	typedef ::std::string				GTestAssertionResultString;
	typedef ::testing::internal::String	GTestString;
#elif	GTEST_MINORVER == 0x05
	typedef ::testing::internal::String	GTestAssertionResultString;
	typedef ::testing::internal::String	GTestString;
#else
#  error sorry, google test version 1.4.0 or less is unsupported.
#endif

	AssertionResult::AssertionResult(const AssertionResult& other)
		: success_(other.success_)
		, message_(other.message_.get() != NULL ?
		new GTestAssertionResultString(*other.message_) : static_cast< GTestAssertionResultString*>(NULL))
	{
	}

	AssertionResult AssertionResult::operator ! () const
	{
		return AssertionResult(success_);
	}

	AssertionResult AssertionFailure()
	{
		return AssertionResult(false);
	}

	AssertionResult AssertionSuccess()
	{
		return AssertionResult(true);
	}

	namespace internal
	{

		AssertHelper::AssertHelper(TestPartResult::Type type,
			const char* file,
			int line,
			const char* message)
			: data_(new AssertHelperData(type, file, line, message)) {}
		AssertHelper::~AssertHelper()
		{
			delete data_;
		}

		void AssertHelper::operator=(const Message& message) const
		{
			Message msg;
#if defined(_MSC_VER)
			msg << data_->file << "(" << data_->line << "): error : " << data_->message << message.GetString() << "\n";
#else
			msg << data_->file << ":" << data_->line << ": error : " << data_->message << message.GetString() << "\n";
#endif

			printf(msg.GetString().c_str());
#if GTEST_OS_WINDOWS
			OutputDebugStringA(msg.GetString().c_str());
#endif
		}

		bool IsTrue(bool condition) { return condition; }
		bool AlwaysTrue() { return true; }

		AssertionResult EqFailure(const char* expected_expression,
			const char* actual_expression,
			const GTestString& expected_value,
			const GTestString& actual_value,
			bool ignoring_case)
		{
			Message msg;
			msg << "Value of: " << actual_expression;
			if( !String::CStringEquals(actual_value.c_str(), actual_expression) ) {
				msg << "\n  Actual: " << actual_value;
			}

			msg << "\nExpected: " << expected_expression;
			if (ignoring_case) {
				msg << " (ignoring case)";
			}
			if( !String::CStringEquals(expected_value.c_str(), expected_expression) ) {
				msg << "\nWhich is: " << expected_value;
			}

			return AssertionFailure() << msg;
		}

#if GTEST_MINORVER <= 0x06
		int String::Compare(const String& rhs) const
		{
			const char* const lhs_c_str = c_str();
			const char* const rhs_c_str = rhs.c_str();

			if (lhs_c_str == NULL) {
				return rhs_c_str == NULL ? 0 : -1;  // NULL < anything except NULL
			} else if (rhs_c_str == NULL) {
				return 1;
			}

			const size_t shorter_str_len =
				length() <= rhs.length() ? length() : rhs.length();
			for (size_t i = 0; i != shorter_str_len; i++) {
				if (lhs_c_str[i] < rhs_c_str[i]) {
					return -1;
				} else if (lhs_c_str[i] > rhs_c_str[i]) {
					return 1;
				}
			}
			return (length() < rhs.length()) ? -1 :
				(length() > rhs.length()) ? 1 : 0;
		}
#endif

		bool String::CStringEquals(const char* lhs, const char* rhs)
		{
			if( lhs == NULL || rhs == NULL ) return lhs == rhs;
			return strcmp(lhs, rhs) == 0;
		}
		bool String::CaseInsensitiveCStringEquals(const char* lhs,
			const char* rhs)
		{
			if( lhs == NULL || rhs == NULL ) return lhs == rhs;
			return internal::posix::StrCaseCmp(lhs, rhs) == 0;
		}
		namespace impl
		{
			GTestString ShowCStringQuoted(const char* c_str)
			{
				::std::string s = "\"";
				s += c_str;
				s += "\"";
				return GTestString(s);
			}
		}
#ifdef GTEST_CREF_WORKAROUND_
		String String::ShowCStringQuoted(const char* c_str)
		{
			return impl::ShowCStringQuoted(c_str);
		}
#endif

		GTestString GetBoolAssertionFailureMessage(const AssertionResult& assertion_result,
			const char* expression_text,
			const char* actual_predicate_value,
			const char* expected_predicate_value) {
				const char* actual_message = assertion_result.message();
				Message msg;
				msg << "Value of: " << expression_text
					<< "\n  Actual: " << actual_predicate_value;
				if (actual_message[0] != '\0')
					msg << " (" << actual_message << ")";
				msg << "\nExpected: " << expected_predicate_value;
				return msg.GetString();
		}

		GTestString StringStreamToString(::std::stringstream* ss) {
			const ::std::string& str = ss->str();
			const char* const start = str.c_str();
			const char* const end = start + str.length();

			// We need to use a helper stringstream to do this transformation
			// because String doesn't support push_back().
			::std::stringstream helper;
			for (const char* ch = start; ch != end; ++ch) {
				if (*ch == '\0') {
					helper << "\\0";  // Replaces NUL with "\\0";
				} else {
					helper.put(*ch);
				}
			}

			return GTestString(helper.str().c_str());
		}
		GTestString StrStreamToString(::std::stringstream* ss) {
			return StringStreamToString(ss);
		}

		AssertionResult CmpHelperSTREQ(const char* expected_expression,
			const char* actual_expression,
			const char* expected,
			const char* actual) {
				if (String::CStringEquals(expected, actual)) {
					return AssertionSuccess();
				}

				return EqFailure(expected_expression,
					actual_expression,
					impl::ShowCStringQuoted(expected),
					impl::ShowCStringQuoted(actual),
					false);
		}

		AssertionResult CmpHelperSTRCASEEQ(const char* expected_expression,
			const char* actual_expression,
			const char* expected,
			const char* actual)
		{
			if (String::CaseInsensitiveCStringEquals(expected, actual)) {
				return AssertionSuccess();
			}

			return EqFailure(expected_expression,
				actual_expression,
				impl::ShowCStringQuoted(expected),
				impl::ShowCStringQuoted(actual),
				true);
		}

		AssertionResult CmpHelperSTRNE(const char* s1_expression,
			const char* s2_expression,
			const char* s1,
			const char* s2)
		{
			if (!String::CStringEquals(s1, s2)) {
				return AssertionSuccess();
			} else {
				return AssertionFailure() << "Expected: (" << s1_expression << ") != ("
					<< s2_expression << "), actual: \""
					<< s1 << "\" vs \"" << s2 << "\"";
			}
		}

		AssertionResult CmpHelperSTRCASENE(const char* s1_expression,
			const char* s2_expression,
			const char* s1,
			const char* s2) {
				if (!String::CaseInsensitiveCStringEquals(s1, s2)) {
					return AssertionSuccess();
				} else {
					return AssertionFailure()
						<< "Expected: (" << s1_expression << ") != ("
						<< s2_expression << ") (ignoring case), actual: \""
						<< s1 << "\" vs \"" << s2 << "\"";
				}
		}
	}

#if GTEST_MINORVER >= 0x07
	Message::Message() : ss_(new ::std::stringstream) {
	  *ss_ << std::setprecision(std::numeric_limits<double>::digits10 + 2);
	}

	std::string Message::GetString() const {
		return internal::StringStreamToString(ss_.get());
	}
#endif

}

#endif	// !defined(IUTEST_USE_GTEST)

#endif	// INCG_IRIS_IUTEST_ASSERTION_ONLY_HPP_9E5FA1C8_EFB5_40A9_A3AD_971584ECAE56_
