//======================================================================
//-----------------------------------------------------------------------
/**
 * @file		iutest_vc_unittest.hpp
 * @brief		iris unit test, visual studio 11 C++ Unit Test Framework 対応 ファイル
 *
 * @author		t.shirayanagi
 * @par			copyright
 * Copyright (C) 2012-2014, Takazumi Shirayanagi\n
 * This software is released under the new BSD License,
 * see LICENSE
*/
//-----------------------------------------------------------------------
//======================================================================
#ifndef INCG_IRIS_IUTEST_VC_UNITTEST_HPP_5CA7C30C_2D2A_40A1_A9DE_83EAB4527787_
#define INCG_IRIS_IUTEST_VC_UNITTEST_HPP_5CA7C30C_2D2A_40A1_A9DE_83EAB4527787_


//======================================================================
// include
#include "CppUnitTest.h"
#include "../../../include/iutest_prod.hpp"
#include "../../../include/util/iutest_util_tests.hpp"

//======================================================================
// define

#ifdef IUTEST
#  undef IUTEST
#endif
#define IUTEST(testcase_, testname_)	IUTEST_VCUNIT(testcase_, testname_, iutest::Test	\
											, iutest::internal::GetTestTypeId()				\
											, testcase_##testname_##_class, testcase_##_##testname_)

#ifdef IUTEST_F
#  undef IUTEST_F
#endif
#define IUTEST_F(testcase_, testname_)	IUTEST_VCUNIT(testcase_, testname_, testcase_	\
											, iutest::internal::GetTypeId<testcase_>()	\
											, testcase_##testname_##_class, testcase_##_##testname_)

#define IUTEST_VCUNIT_CHECK_DISABLED(testcase_, testname_)				\
	if( const char* p = #testcase_)			\
		if(strstr(p, "DISABLED_") == p)		\
			return;							\
	if( const char* p = #testname_ )		\
		if(strstr(p, "DISABLED_") == p)		\
			return

#define IUTEST_VCUNIT(testcase_, testname_, parent_, type_id_, className, methodName)		\
	IUTEST_TEST_(testcase_, testname_, parent_, type_id_) {}								\
	IUTEST_VCUNIT_I(testcase_, testname_, className, methodName)

#define IUTEST_VCUNIT_I(testcase_, testname_, className, methodName)		\
	IUTEST_VC_TEST_CLASS(className), public IUTEST_TEST_CLASS_NAME_(testcase_, testname_) {	\
	public: TEST_METHOD(methodName) {										\
		IUTEST_VCUNIT_CHECK_DISABLED(testcase_, testname_);					\
		OnTestStart(#testcase_, #testname_); SetUp();						\
		Body(); TearDown(); OnTestEnd(#testcase_, #testname_); }			\
	TEST_CLASS_INITIALIZE(iuSetUp) { IUTEST_TEST_CLASS_NAME_(testcase_, testname_)::SetUpTestCase(); }		\
	TEST_CLASS_CLEANUP(iuTearDown) { IUTEST_TEST_CLASS_NAME_(testcase_, testname_)::TearDownTestCase(); }	\
	virtual void Body(void);				\
	};										\
	void className::Body()

#ifndef IUTEST_USE_GTEST

#ifdef IUTEST_P
#  undef IUTEST_P
#endif

IUTEST_MAKE_PEEP(::iutest::detail::iuFactoryBase* ::iutest::TestInfo::*, ::iutest::TestInfo, m_factory);

#define IUTEST_P(testcase_, testname_)														\
	class IUTEST_TEST_CLASS_NAME_(testcase_, testname_) : public testcase_ {				\
	public: IUTEST_TEST_CLASS_NAME_(testcase_, testname_)(void) {}							\
	protected: virtual void Body(void) {}													\
		IUTEST_PP_DISALLOW_COPY_AND_ASSIGN(IUTEST_TEST_CLASS_NAME_(testcase_, testname_));	\
	};																						\
	IUTEST_P_VCUNIT_I(testcase_, testname_, testcase_##testname_##_class, testcase_##_##testname_)

#define IUTEST_P_VCUNIT_I(testcase_, testname_, className, methodName)						\
	IUTEST_VC_TEST_CLASS(className), public IUTEST_TEST_CLASS_NAME_(testcase_, testname_) { \
	public: TEST_METHOD(methodName) {														\
		IUTEST_VCUNIT_CHECK_DISABLED(testcase_, testname_);									\
		const ::iutest::TestCase* testcase = ::iuutil::FindParamTestCase(#testcase_);		\
		::Microsoft::VisualStudio::CppUnitTestFramework::Assert::IsNotNull(testcase);		\
		while( testcase != NULL ) {															\
			const char* testcase_name = testcase->name();									\
			::std::string name(#testname_);	name += "/";									\
			int testinfo_count = testcase->total_test_count();								\
			for( int i=0; i < testinfo_count; ++i ) {										\
				const ::iutest::TestInfo* testinfo = testcase->GetTestInfo(i);				\
				::Microsoft::VisualStudio::CppUnitTestFramework::Assert::IsNotNull(testinfo);	\
				const char* testinfo_name = testinfo->name();								\
				if( strstr(testinfo_name, name.c_str()) == testinfo_name ) {				\
					::iutest::detail::iuParamTestFactory<className>* factory =				\
						static_cast< ::iutest::detail::iuParamTestFactory<className>*>(		\
							IUTEST_PEEP_GET(*testinfo, ::iutest::TestInfo, m_factory));		\
					::Microsoft::VisualStudio::CppUnitTestFramework::Assert::IsNotNull(factory);	\
					SetParam(&factory->GetParam());											\
					OnTestStart(testcase_name, testinfo_name);								\
					SetUp(); Body(); TearDown();											\
					OnTestEnd(testcase_name, testinfo_name);								\
				}																			\
			}																				\
			testcase = ::iuutil::FindParamTestCase(#testcase_, testcase);					\
		}																					\
	}																						\
	TEST_CLASS_INITIALIZE(iuSetUp) { IUTEST_TEST_CLASS_NAME_(testcase_, testname_)::SetUpTestCase(); }		\
	TEST_CLASS_CLEANUP(iuTearDown) { IUTEST_TEST_CLASS_NAME_(testcase_, testname_)::TearDownTestCase(); }	\
	virtual void Body(void);																\
	private: static int	AddRegister(void) {													\
			static ::iutest::detail::ParamTestInstance< className > testinfo(#testname_);	\
			::iutest::UnitTest::GetInstance()->parameterized_test_registry().				\
			GetTestCasePatternHolder< testcase_ >(#testcase_, IUTEST_GET_PACKAGENAME_())	\
				->AddTestPattern(&testinfo); return 0;										\
		}																					\
		static int dummy_;																	\
	};																						\
	int className::dummy_ IUTEST_ATTRIBUTE_UNUSED_ = className::AddRegister();				\
	void className::Body()


#if 1
#ifdef IUTEST_TYPED_TEST
#  undef IUTEST_TYPED_TEST
#endif

#define IUTEST_TYPED_TEST(testcase_, testname_)											\
	IUTEST_TYPED_TEST_VCUNIT_I(testcase_, testname_, testcase_##testname_##_class, testcase_##_##testname_);	\
	IIUT_TYPED_TEST_(testcase_, testname_)

#define IUTEST_TYPED_TEST_VCUNIT_I(testcase_, testname_, className, methodName)			\
	IUTEST_VC_TEST_CLASS(className) {													\
	public: TEST_METHOD(methodName) {													\
		IUTEST_VCUNIT_CHECK_DISABLED(testcase_, testname_);								\
		const ::iutest::TestCase* testcase = ::iuutil::FindTypedTestCase(#testcase_);	\
		::Microsoft::VisualStudio::CppUnitTestFramework::Assert::IsNotNull(testcase);	\
		while( testcase != NULL ) {														\
			const char* testcase_name = testcase->name();								\
			const ::iutest::TestInfo* testinfo = ::iuutil::FindTestInfo(testcase, #testname_);	\
			::Microsoft::VisualStudio::CppUnitTestFramework::Assert::IsNotNull(testinfo);	\
			::iutest::detail::iuFactoryBase* factory =									\
				IUTEST_PEEP_GET(*testinfo, ::iutest::TestInfo, m_factory);				\
			::Microsoft::VisualStudio::CppUnitTestFramework::Assert::IsNotNull(factory);	\
			::iutest::detail::auto_ptr< ::iutest::Test > p = factory->Create();			\
			::iuutil::VisualStudio::Test* tester = static_cast< ::iuutil::VisualStudio::Test*>(p.get());	\
			OnTestStart(testcase_name, #testname_);										\
			tester->SetUp(); tester->Body(); tester->TearDown();						\
			OnTestEnd(testcase_name, #testname_);										\
			testcase = ::iuutil::FindTypedTestCase(#testcase_, testcase);				\
		}																				\
	}																					\
	}

#endif

#if 1
#ifdef IUTEST_TYPED_TEST_P
#  undef IUTEST_TYPED_TEST_P
#endif

#define IUTEST_TYPED_TEST_P(testcase_, testname_)		\
	IUTEST_TYPED_TEST_P_VCUNIT_I(testcase_, testname_, testcase_##testname_##_class, testcase_##_##testname_);	\
	IIUT_TYPED_TEST_P_(testcase_, testname_)

#define IUTEST_TYPED_TEST_P_VCUNIT_I(testcase_, testname_, className, methodName)			\
	IUTEST_VC_TEST_CLASS(className) {														\
	public: TEST_METHOD(methodName) {														\
		const ::iutest::TestCase* testcase = ::iuutil::FindParamTypedTestCase(#testcase_);	\
		::Microsoft::VisualStudio::CppUnitTestFramework::Assert::IsNotNull(testcase);		\
		while( testcase != NULL ) {															\
		::Microsoft::VisualStudio::CppUnitTestFramework::Logger::WriteMessage(#testcase_);	\
			const char* testcase_name = testcase->name();									\
			const ::iutest::TestInfo* testinfo = ::iuutil::FindTestInfo(testcase, #testname_);	\
			::Microsoft::VisualStudio::CppUnitTestFramework::Assert::IsNotNull(testinfo);	\
			::iutest::detail::iuFactoryBase* factory =										\
				IUTEST_PEEP_GET(*testinfo, ::iutest::TestInfo, m_factory);					\
			::Microsoft::VisualStudio::CppUnitTestFramework::Assert::IsNotNull(factory);	\
			::iutest::detail::auto_ptr< ::iutest::Test > p = factory->Create();				\
			::iuutil::VisualStudio::Test* tester = static_cast< ::iuutil::VisualStudio::Test*>(p.get());	\
			OnTestStart(testcase_name, #testname_);										\
			tester->SetUp(); tester->Body(); tester->TearDown();						\
			OnTestEnd(testcase_name, #testname_);										\
			testcase = ::iuutil::FindParamTypedTestCase(#testcase_, testcase);			\
		}																				\
	}																					\
	}

#endif

#endif

#define IUTEST_VC_TEST_CLASS(className)	\
	ONLY_USED_AT_NAMESPACE_SCOPE class className : public ::iuutil::VisualStudio::TestClass<className>

//======================================================================
// class
namespace iuutil {
namespace VisualStudio
{

typedef ::testing::TestEventListener* (::testing::TestEventListeners::* pfnRepeater)(void);

#define testing	iutest
IUTEST_MAKE_PEEP(pfnRepeater, ::testing::TestEventListeners, repeater);
#undef testing

#ifndef IUTEST_USE_GTEST

class Test : public ::iutest::Test
{
public:
	using ::iutest::Test::SetUp;
	using ::iutest::Test::Body;
	using ::iutest::Test::TearDown;
};

#endif

template<typename T>
class TestClass : public ::Microsoft::VisualStudio::CppUnitTestFramework::TestClass<T>
{
	typedef ::Microsoft::VisualStudio::CppUnitTestFramework::Logger Logger;
public:
	void OnTestStart(const char* testcase_name, const char* testinfo_name)
	{
		const ::iutest::TestInfo* testinfo = iuutil::FindTestInfo(testcase_name, testinfo_name);
		if( testinfo == NULL )
		{
			return;
		}
		::iutest::TestEventListeners& listeners = ::iutest::UnitTest::GetInstance()->listeners();
		::iutest::TestEventListener* repeator =
#define testing	iutest
			IUTEST_PEEP_GET(listeners, ::testing::TestEventListeners, repeater)();
#undef testing
		repeator->OnTestStart(*testinfo);
	}
	void OnTestEnd(const char* testcase_name, const char* testinfo_name)
	{
		const ::iutest::TestInfo* testinfo = iuutil::FindTestInfo(testcase_name, testinfo_name);
		if( testinfo == NULL )
		{
			return;
		}
		::iutest::TestEventListeners& listeners = ::iutest::UnitTest::GetInstance()->listeners();
		::iutest::TestEventListener* repeator =
#define testing	iutest
			IUTEST_PEEP_GET(listeners, ::testing::TestEventListeners, repeater)();
#undef testing
		repeator->OnTestEnd(*testinfo);
	}
};

/**
 * @brief	Logger
*/
class VCCppUnitTestLogger : public ::iutest::detail::iuLogger
{
	typedef ::Microsoft::VisualStudio::CppUnitTestFramework::Logger Logger;
public:
	virtual void voutput(const char* fmt, va_list va)
	{
		int length = _vscprintf(fmt, va);
		if( length <= 0 )
		{
			return;
		}
		length += 1;
		char* buf = new char [length];
		vsprintf_s(buf, length, fmt, va);
		m_log += buf;
		delete [] buf;

		int pos = m_log.find('\n');
		while(pos >= 0) {
			Logger::WriteMessage(m_log.substr(0, pos).c_str());
			m_log = m_log.substr(pos+1);
			pos = m_log.find('\n');
		}
	}
private:
	::std::string m_log;
};

/**
 * @brief	Hook 用リポーター
*/
class VCCppUnitTestPartResultReporter : public ::iutest::EmptyTestEventListener
{
public:
	VCCppUnitTestPartResultReporter(void)
	{
		::iutest::TestEventListeners& listeners = ::iutest::UnitTest::GetInstance()->listeners();
		listeners.Append(this);
	}
	virtual ~VCCppUnitTestPartResultReporter(void)
	{
		::iutest::TestEventListeners& listeners = ::iutest::UnitTest::GetInstance()->listeners();
		listeners.Release(this);
	}
	virtual void OnTestPartResult(const ::iutest::TestPartResult& result)
	{
		// VC にも送る
		if( result.failed() )
		{
			size_t size=0;
			wchar_t buf1[4096];	// TODO: Fixed buffer...
			wchar_t buf2[MAX_PATH];
			::mbstowcs_s(&size, buf1, sizeof(buf1)/sizeof(buf1[0]), result.message(), _TRUNCATE);
			::mbstowcs_s(&size, buf2, sizeof(buf2)/sizeof(buf2[0]), result.file_name(), _TRUNCATE);
			::Microsoft::VisualStudio::CppUnitTestFramework::Assert::Fail(buf1
				, &Microsoft::VisualStudio::CppUnitTestFramework::__LineInfo(buf2, "", result.line_number()) );
		}
	}
};


inline void SetUpCppUnitTest(void)
{
#if 0
	{
		int argc=0;
		char** argv = NULL;
		IUTEST_INIT(&argc, argv);
	}
#endif
	{
		static VCCppUnitTestPartResultReporter fake;
#ifndef IUTEST_USE_GTEST
		static VCCppUnitTestLogger logger;
		::iutest::detail::iuConsole::SetLogger(&logger);
		::iutest::UnitTestSource::GetInstance().Initialize();
#endif
	}
}

}	// end of namespace VisualStudio
}	// end of namespace iuutil


#endif
