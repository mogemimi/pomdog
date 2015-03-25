// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#include <Pomdog/Event/Signal.hpp>
#include <gtest/iutest_switch.hpp>
#include <utility>

using Pomdog::Signal;

TEST(Singal, Void)
{
	{
		Signal<void()> signal;
		std::string text;
		signal.Connect([&]{ text = "Done"; });
		signal();
		EXPECT_EQ("Done", text);
	}
	{
		Signal<void()> signal;
		std::string text;
		signal.Connect([&]{ text += "Doki"; });
		signal();
		signal();
		signal();
		signal();
		EXPECT_EQ("DokiDokiDokiDoki", text);
	}
	{
		Signal<void()> signal;
		std::string text;
		signal.Connect([&]{ text += "Chuck "; });
		signal.Connect([&]{ text += "Norris"; });
		signal();
		EXPECT_EQ("Chuck Norris", text);
	}
}

TEST(Singal, FundamentalTypes)
{
	{
		Signal<void(bool)> signal;
		bool result;
		signal.Connect([&](bool in){ result = in; });
		signal(true);
		EXPECT_EQ(true, result);
		signal(false);
		EXPECT_EQ(false, result);
		signal(result);
		EXPECT_EQ(false, result);
	}
	{
		Signal<void(int)> signal;
		int result;
		signal.Connect([&](int in){ result = in; });
		signal(42);
		EXPECT_EQ(42, result);
		signal(72);
		EXPECT_EQ(72, result);
		signal(result);
		EXPECT_EQ(72, result);
	}
	{
		Signal<void(float)> signal;
		float result;
		signal.Connect([&](float in){ result = in; });
		signal(42.0f);
		EXPECT_EQ(result, 42.0f);
		signal(72.0f);
		EXPECT_EQ(result, 72.0f);
		signal(result);
		EXPECT_EQ(result, 72.0f);
	}
	{
		Signal<void(char)> signal;
		char result;
		signal.Connect([&](char in){ result = in; });
		signal('a');
		EXPECT_EQ(result, 'a');
		signal('\n');
		EXPECT_EQ(result, '\n');
		signal(result);
		EXPECT_EQ(result, '\n');
	}
}

TEST(Singal, String)
{
	{
		Signal<void(std::string const&)> signal;
		std::string result;
		signal.Connect([&](std::string const& in){ result = in; });
		signal("Norris");
		EXPECT_EQ("Norris", result);
		signal(std::string{"Chuck"});
		EXPECT_EQ("Chuck", result);
		signal(result);
		EXPECT_EQ("Chuck", result);
	}
	{
		Signal<void(std::string const&)> signal;
		std::string result;
		signal.Connect([&](std::string in){ result = in; });
		signal("Norris");
		EXPECT_EQ("Norris", result);
		signal(std::string{"Chuck"});
		EXPECT_EQ("Chuck", result);
		signal(result);
		EXPECT_EQ("Chuck", result);
	}
	{
		Signal<void(std::string)> signal;
		std::string result;
		signal.Connect([&](std::string in){ result = in; });
		signal("Norris");
		EXPECT_EQ("Norris", result);
		signal(std::string{"Chuck"});
		EXPECT_EQ("Chuck", result);
		signal(result);
		EXPECT_EQ("Chuck", result);
	}
	{
		Signal<void(std::string)> signal;
		std::string result;
		signal.Connect([&](std::string const& in){ result = in; });
		signal("Norris");
		EXPECT_EQ("Norris", result);
		signal(std::string{"Chuck"});
		EXPECT_EQ("Chuck", result);
		signal(result);
		EXPECT_EQ("Chuck", result);
	}
	{
		Signal<void(std::string &)> signal;
		std::string result;
		signal.Connect([](std::string & in){ in += "Chuck "; });
		signal.Connect([](std::string & in){ in += "Norris"; });
		signal(result);
		EXPECT_EQ("Chuck Norris", result);
	}
}

TEST(Singal, CustomClass)
{
	struct Chuck {
		int value;

		Chuck(): value(0) {};
		Chuck(int v): value(v) {};

		Chuck(Chuck const&) = delete;
		Chuck(Chuck &&) = delete;
		Chuck & operator=(Chuck const&) = delete;
		Chuck & operator=(Chuck &&) = delete;
	};

	{
		Signal<void(Chuck const&)> signal;
		int result = 0;
		signal.Connect([&](Chuck const& in){ result += in.value; });
		signal.Connect([&](Chuck const& in){ result += in.value; });
		Chuck const chuck{42};
		signal(chuck);
		EXPECT_EQ(42 + 42, result);
	}
	{
		Signal<void(Chuck const&, Chuck const&)> signal;
		int result = 0;
		signal.Connect([&](Chuck const& in1, Chuck const& in2){ result += (in1.value + in2.value); });
		signal.Connect([&](Chuck const& in1, Chuck const& in2){ result += (in1.value + in2.value); });
		Chuck const chuck{3};
		signal(chuck, chuck);
		signal(chuck, chuck);
		signal(chuck, chuck);
		signal(chuck, chuck);
		EXPECT_EQ(((3 + 3) * 2) * 4, result);
	}
}
