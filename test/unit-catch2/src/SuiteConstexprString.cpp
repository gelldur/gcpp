#include <cstdio>
#include <format>
#include <print>
#include <sstream>
#include <string_view>

#include <catch2/catch_test_macros.hpp>

#include "gcpp/string/ConstexprString.hpp"

using namespace gcpp::string;

namespace
{

template <ConstexprString S>
consteval std::size_t lengthOfNTTP()
{
	return S.length();
}

} // namespace

TEST_CASE("ConstexprString construction and access", "[ConstexprString]")
{
	static constexpr ConstexprString text{"abc"};
	STATIC_REQUIRE(std::is_same_v<decltype(text), const ConstexprString<3>>);
	STATIC_REQUIRE(text.length() == 3);
	STATIC_REQUIRE(text[0] == 'a');
	STATIC_REQUIRE(text[2] == 'c');
	STATIC_REQUIRE(text.view() == "abc");
	STATIC_REQUIRE(text.c_str()[3] == '\0');
	STATIC_REQUIRE(make("xy") == "xy");
	STATIC_REQUIRE(ConstexprString{""}.length() == 0);
	STATIC_REQUIRE(lengthOfNTTP<"hello">() == 5);
	STATIC_REQUIRE(IsConstexprString<const ConstexprString<3>&>);
	STATIC_REQUIRE_FALSE(IsConstexprString<const char*>);
}

TEST_CASE("ConstexprString concatenation", "[ConstexprString]")
{
	STATIC_REQUIRE(ConstexprString{"ab"} + "cd" == "abcd");
	STATIC_REQUIRE("ab" + ConstexprString{"cd"} == "abcd");
	STATIC_REQUIRE(ConstexprString{"ab"} + ConstexprString{"cd"} + "" + "ef" == "abcdef");
	STATIC_REQUIRE((ConstexprString{"a"} + "bc").length() == 3);
	STATIC_REQUIRE(STATIC_VIEW<ConstexprString{"x"} + "y" + "z"> == "xyz");
}

TEST_CASE("ConstexprString comparison", "[ConstexprString]")
{
	STATIC_REQUIRE(ConstexprString{"abc"} == ConstexprString{"abc"});
	STATIC_REQUIRE(ConstexprString{"abc"} != ConstexprString{"abd"});
	STATIC_REQUIRE(ConstexprString{"abc"} != ConstexprString{"abcd"});
	STATIC_REQUIRE("abc" == ConstexprString{"abc"});
	STATIC_REQUIRE(ConstexprString{"abc"} != "ab");
}

TEST_CASE("ConstexprString subString", "[ConstexprString]")
{
	constexpr auto text = ConstexprString{"abcdef"};
	STATIC_REQUIRE(subString(text, 0) == "abcdef");
	STATIC_REQUIRE(subString(text, 2) == "cdef");
	STATIC_REQUIRE(subString(text, 1, 3) == "bcd");
	STATIC_REQUIRE(subString(text, 4, 100) == "ef");
	STATIC_REQUIRE(subString(text, 6).empty());
	STATIC_REQUIRE(subString(text, 2).capacity() == 6); // capacity kept, only length shrinks
	STATIC_REQUIRE(subString(subString(text, 1), 1, 2) == "cd");
}

TEST_CASE("ConstexprString fromInteger", "[ConstexprString]")
{
	STATIC_REQUIRE(fromInteger(0) == "0");
	STATIC_REQUIRE(fromInteger(1234) == "1234");
	STATIC_REQUIRE(fromInteger(-42) == "-42");
	STATIC_REQUIRE(fromInteger(std::numeric_limits<long long>::min()) == "-9223372036854775808");
	STATIC_REQUIRE(fromInteger(18446744073709551615ULL) == "18446744073709551615");
}

TEST_CASE("ConstexprString runtime access", "[ConstexprString]")
{
	std::ostringstream stream;
	stream << STATIC_VIEW<ConstexprString{"run"} + "time">;
	CHECK(stream.str() == "runtime");
	CHECK(std::string{STATIC_C_STR<ConstexprString{"c"} + "str">} == "cstr");
	CHECK(std::string{STATIC_VIEW<ConstexprString{"ab"} + "c" + fromInteger(1)>} == "abc1");
}

TEST_CASE("ConstexprString concatenation of shrunk values", "[ConstexprString]")
{
	// Length shrinks but capacity stays - concatenation must use length, not capacity
	constexpr auto left = subString(ConstexprString{"xxABxx"}, 2, 2);
	STATIC_REQUIRE(left + "|" + fromInteger(7) == "AB|7");
	STATIC_REQUIRE((left + "|" + fromInteger(7)).capacity() == 6 + 1 + INTEGER_CAPACITY);
	STATIC_REQUIRE(STATIC_VIEW<left + "|" + fromInteger(7)> == "AB|7");
	// The pinned object is shrunk to exact size - no padding goes into the binary
	STATIC_REQUIRE(sizeof(STATIC_CHARS<left + "|" + fromInteger(7)>) == 4 + 1);
}

namespace
{

static constexpr auto NAME = ConstexprString{"rr"} + "::" + "core";

std::size_t takesView(const std::string_view text)
{
	return text.size();
}

/// Captures what std::println writes to a FILE*
template <typename... Args>
std::string printlnToString(std::format_string<Args...> format, Args&&... args)
{
	std::FILE* file = std::tmpfile();
	REQUIRE(file != nullptr);
	std::println(file, format, std::forward<Args>(args)...);
	std::rewind(file);
	std::string result(256, '\0');
	result.resize(std::fread(result.data(), 1, result.size(), file));
	std::fclose(file);
	return result;
}

} // namespace

TEST_CASE("ConstexprString implicit conversion to std::string_view", "[ConstexprString]")
{
	STATIC_REQUIRE(std::is_convertible_v<ConstexprString<3>, std::string_view>);
	STATIC_REQUIRE(std::string_view{NAME} == "rr::core");
	STATIC_REQUIRE(NAME == std::string_view{"rr::core"});
	STATIC_REQUIRE(std::string_view{"rr::core"} == NAME);
	STATIC_REQUIRE(NAME != std::string_view{"rr"});

	// static storage => conversion allowed and the view never dangles
	const std::string_view view = NAME;
	CHECK(view == "rr::core");
	CHECK(view.data() == NAME.chars);
	CHECK(takesView(NAME) == 8);
	CHECK(takesView(STATIC_VIEW<NAME + "!">) == 9);
	// Rejected at compile time, see compile_fail/: temporaries (would dangle), local constexpr
	// (would dangle), static values with unused capacity (padding would be emitted).
}

TEST_CASE("ConstexprString pointer-free queries work on any value", "[ConstexprString]")
{
	// subString keeps capacity 12 for length 5 - view() would refuse it, the queries do not
	constexpr auto padded = subString(ConstexprString{"rr::core::ab"}, 4, 5);
	STATIC_REQUIRE(padded.capacity() == 12);
	STATIC_REQUIRE(padded.startsWith("core"));
	STATIC_REQUIRE(padded.endsWith("e:"));
	STATIC_REQUIRE(padded.contains("re:"));
	STATIC_REQUIRE_FALSE(padded.contains("rr"));
	STATIC_REQUIRE(padded.find(":") == 4);
	STATIC_REQUIRE(padded.find("x") == std::string_view::npos);
	STATIC_REQUIRE(padded.rfind("e") == 3);
	STATIC_REQUIRE(fromInteger(-12).startsWith("-1"));
	STATIC_REQUIRE((ConstexprString{"a"} + "b").endsWith("ab"));
}

TEST_CASE("ConstexprString works with std::format and std::println", "[ConstexprString]")
{
	// ConstexprString itself is not formattable (compile error, see compile_fail/), the pinned
	// text or view() of an exactly sized static constant is
	CHECK(std::format("{}", NAME.view()) == "rr::core");
	CHECK(std::format("[{:>6}]", STATIC_VIEW<ConstexprString{"ab"}>) == "[    ab]");
	CHECK(std::format("[{:*<5.1}]", std::string_view{NAME}) == "[r****]");
	// shrunk value: only the text is pinned and printed, never the zero padding
	CHECK(std::format("{}|", STATIC_VIEW<subString(ConstexprString{"--ab--"}, 2, 2)>) == "ab|");
	CHECK(std::format("{}", STATIC_VIEW<fromInteger(-7)>) == "-7");

	CHECK(printlnToString("{}", NAME.view()) == "rr::core\n");
	CHECK(printlnToString("{} {}", NAME.view(), STATIC_VIEW<NAME + "!">) == "rr::core rr::core!\n");
	// ConstexprString with static storage (or STATIC_VIEW) can even be the compile-time checked
	// format string. GCC with UBSan refuses it: -fsanitize=null implies
	// -fno-delete-null-pointer-checks and GCC then can't prove `&staticObject[i] != nullptr`
	// inside std::format_string. Define GCPP_GCC_UBSAN_BUILD in such builds to skip this check.
#ifndef GCPP_GCC_UBSAN_BUILD
	static constexpr auto FORMAT = NAME + " {}";
	CHECK(printlnToString(FORMAT, 42) == "rr::core 42\n");
	CHECK(printlnToString(STATIC_VIEW<NAME + " {}!">, 7) == "rr::core 7!\n");
#endif
}

#if GCPP_HAS_DEFINE_STATIC
TEST_CASE("ConstexprString staticView (C++26 define_static_string)", "[ConstexprString]")
{
	STATIC_REQUIRE((ConstexprString{"a"} + "b").staticView() == "ab");
	STATIC_REQUIRE(subString(ConstexprString{"--ab--"}, 2, 2).staticView() == "ab");
	STATIC_REQUIRE(std::string_view{fromInteger(7).staticCStr()} == "7");

	// Works on temporaries, the result never dangles
	const std::string_view view = (NAME + "!").staticView();
	const char* cString = (NAME + "?").staticCStr();
	CHECK(view == "rr::core!");
	CHECK(std::string_view{cString} == "rr::core?");
	// Same value => same static object
	CHECK((ConstexprString{"x"} + "y").staticView().data() ==
		  (ConstexprString{"xy"}).staticView().data());
	CHECK(std::format("{}", (NAME + "!").staticView()) == "rr::core!");
}
#endif

//////////////////////////////
//////////////////////////////
//////////////////////////////
//////////////////////////////

std::string_view otherUnitSharedView(); // OtherUnit.cpp

namespace
{

/**
 * The strongest proof is that this compiles at all: a `constexpr` pointer (and string_view) may
 * only point to an object with static storage duration. Anything that could dangle - a local,
 * a temporary, heap memory - is rejected by the compiler.
 */
constexpr std::string_view NAMESPACE_SCOPE_VIEW = STATIC_VIEW<ConstexprString{"global"} + "_view">;
constexpr std::array<std::string_view, 2> VIEW_TABLE{
	STATIC_VIEW<ConstexprString{"first"} + "_entry">,
	STATIC_VIEW<ConstexprString{"second"} + "_entry">,
};

/// Returned view outlives this function's stack frame
std::string_view makeViewInFunction()
{
	const auto local = STATIC_VIEW<ConstexprString{"made"} + "_in_" + "function">;
	return local;
}

const char* makeCStrInFunction()
{
	return STATIC_C_STR<ConstexprString{"c"} + "_string">;
}

/// Overwrites the stack region previously used by other calls
[[gnu::noinline]] std::size_t clobberStack()
{
	volatile char garbage[16 * 1024];
	std::memset(const_cast<char*>(garbage), 'X', sizeof(garbage));
	return garbage[100];
}

struct Holder
{
	std::string_view name;
};

std::unique_ptr<Holder> makeHolder()
{
	// Scope of this function ends, view stored in a heap object that outlives it
	return std::make_unique<Holder>(Holder{STATIC_VIEW<ConstexprString{"held"} + "_name">});
}

} // namespace

TEST_CASE("string_view at namespace scope is a compile-time constant", "[StringViewSafety]")
{
	STATIC_REQUIRE(NAMESPACE_SCOPE_VIEW == "global_view");
	STATIC_REQUIRE(VIEW_TABLE[1] == "second_entry");
	CHECK(NAMESPACE_SCOPE_VIEW == "global_view");
	CHECK(VIEW_TABLE[0] == "first_entry");
}

TEST_CASE("view returned from a function does not dangle", "[StringViewSafety]")
{
	const std::string_view view = makeViewInFunction();
	const char* cString = makeCStrInFunction();
	CHECK(clobberStack() == 'X');
	CHECK(view == "made_in_function");
	CHECK(std::string_view{cString} == "c_string");
	CHECK(cString[std::strlen(cString)] == '\0');
}

TEST_CASE("view stored in objects outliving the creating scope", "[StringViewSafety]")
{
	std::vector<std::string_view> views;
	{
		const auto holder = makeHolder();
		views.push_back(holder->name);
		for(int i = 0; i < 3; ++i)
		{
			views.push_back(STATIC_VIEW<ConstexprString{"loop"} + "_value">);
		}
	} // holder destroyed here, the pointee is not owned by it
	CHECK(clobberStack() == 'X');
	REQUIRE(views.size() == 4);
	CHECK(views[0] == "held_name");
	CHECK(views[3] == "loop_value");
}

TEST_CASE("same value is one object for the whole program", "[StringViewSafety]")
{
	// Same value from different places => identical address, also across translation units
	const std::string_view here = STATIC_VIEW<ConstexprString{"shared"} + "_" + "value">;
	const std::string_view builtDifferently = STATIC_VIEW<ConstexprString{"shared_"} + "value">;
	const std::string_view otherUnit = otherUnitSharedView();
	CHECK(here == "shared_value");
	CHECK(here.data() == builtDifferently.data());
	CHECK(here.data() == otherUnit.data());
	// Loop iterations / repeated calls never create new copies
	CHECK(makeViewInFunction().data() == makeViewInFunction().data());
}

TEST_CASE("pinned storage is exact and null terminated", "[StringViewSafety]")
{
	constexpr auto text = gcpp::string::subString(ConstexprString{"--trim--"}, 2, 4);
	STATIC_REQUIRE(text.capacity() == 8);
	STATIC_REQUIRE(STATIC_VIEW<text>.size() == 4);
	STATIC_REQUIRE(sizeof(gcpp::string::STATIC_CHARS<text>) == 5);
	CHECK(STATIC_C_STR<text>[4] == '\0');
	CHECK(std::string{STATIC_C_STR<text>} == "trim");
}
