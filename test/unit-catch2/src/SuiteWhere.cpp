//
// Created by gelldur on 30.03.19.
//
#include <iostream>
#include <print>
#include <sstream>
#include <string>
#include <string_view>

#include <catch2/catch_test_macros.hpp>

#include "gcpp/where.hpp"

using namespace gcpp::string;
using namespace std::string_literals;
using gcpp::string::ConstexprString;
using gcpp::string::STATIC_VIEW;
namespace where = gcpp::where;

namespace
{

constexpr auto TEST_FILE_TAG = STATIC_VIEW<gcpp::where::projectFilePath()>;
constexpr std::string_view TEST_FILE_TAG2 = STATIC_VIEW<gcpp::where::projectFilePath()>;

template <ConstexprString S>
consteval std::size_t lengthOfNTTP()
{
	return S.length();
}

// In some cases we want to pass a 'tag' which may be build from ConstexprString
void exampleTagFunction(const std::string_view tag)
{
	std::println("exampleTagFunction={} as string='{}'", tag, std::string{tag});
}

void exampleUseCaseWhere(int argument)
{
	std::cout << "__FILE_NAME__=" << __FILE_NAME__ << std::endl;
	/**
	 * To avoid leaking build paths in binary we can use: -fmacro-prefix-map=${CMAKE_SOURCE_DIR}/=  is a compiler flag that rewrites __FILE__
	 *
	 * For cmake:
	 *	# Global application for Release builds
	 *	if(CMAKE_BUILD_TYPE STREQUAL "Release")
	 *		add_compile_options(
	 *			-fmacro-prefix-map=${CMAKE_SOURCE_DIR}=SRC
	 *			-fmacro-prefix-map=${CMAKE_BINARY_DIR}=BIN
	 *		)
	 *	endif()
	 *
	 *	# Or for a specific target
	 *	target_compile_options(my_target PRIVATE
	 *		-fmacro-prefix-map=${CMAKE_SOURCE_DIR}=SRC
	 *	)
	 * */
	std::cout << "__FILE__=" << __FILE__ << std::endl;
	std::cout << "__PRETTY_FUNCTION__=" << __PRETTY_FUNCTION__ << std::endl;

	// Example output
	// Called: void (anonymous namespace)::exampleUseCaseWhere(int):18
	std::cout << "GCPP_FUNCTION_NAME=" << GCPP_FUNCTION_NAME << std::endl;

	std::cout << "gcpp::where::filePath()=" << STATIC_VIEW<gcpp::where::filePath()> << std::endl;
	std::cout << "gcpp::where::fileName()=" << STATIC_VIEW<gcpp::where::fileName()> << std::endl;
	std::cout << "gcpp::where::fileNameWithExtension()="
			  << STATIC_VIEW<gcpp::where::fileNameWithExtension()> << std::endl;
	std::cout << "gcpp::where::functionName()="
			  << STATIC_C_STR<gcpp::where::functionName()> << std::endl;
	std::cout << "gcpp::where::projectFilePath()="
			  << STATIC_VIEW<gcpp::where::projectFilePath()> << std::endl;
	std::cout << "gcpp::where::exceptionTag()=" << toView<gcpp::where::exceptionTag()>()
			  << std::endl;

	std::cout << std::flush;

	std::println();
	std::println();
	exampleTagFunction(STATIC_VIEW<gcpp::where::filePath()>);

	constexpr std::string_view constLocalVar = STATIC_VIEW<gcpp::where::filePath()>;
	std::string localVar{STATIC_VIEW<gcpp::where::filePath()>};
	// std::string localVar2 = STATIC_VIEW<gcpp::where::filePath()>;

	std::println("TEST_FILE_TAG={}", TEST_FILE_TAG);

	// std::string where = WHERE; // copy c string
	// std::cout << where << std::endl;
	//
	// constexpr auto where2 = WHERE_STR;
	// std::string_view where_view{where2.data(), where2.length()};
	// std::cout << where_view << std::endl;
}

} // namespace

TEST_CASE("Where usage example", "[where][ConstexprString]")
{
	exampleUseCaseWhere(10);
}

TEST_CASE("Should work at compile time", "[ConstexprString]")
{
	constexpr auto foo = gcpp::string::make("foo");
	constexpr auto bar = gcpp::string::make("bar");

	constexpr auto foobar = foo + bar;

	static_assert(foobar == "foobar");
	static_assert("foobar" == foobar);

	static_assert((foo + "bar") == "foobar");
	static_assert("foobar" == ("foo" + bar));

	static_assert((foo + bar)[3] == 'b');
	static_assert((foo + bar).length() == 6);
	assert(foo == "foo"); // Do not work with REQUIRE macro
	assert(bar == "bar");
	assert(foobar != "bar");
	assert(foobar != "foo");
	assert(foobar == "foobar");
}

namespace
{

template <typename T>
consteval auto functionNameInTemplate(T)
{
	return where::functionName();
}

/// Example of a user defined tag built from the building blocks
consteval auto logTag(const std::source_location location = std::source_location::current())
{
	return "<" + where::fileName(location) + "@" + where::line(location) + ">";
}

} // namespace

TEST_CASE("erasePath", "[Where]")
{
	STATIC_REQUIRE(gcpp::erasePath(ConstexprString{"/src/core/Order.cpp"}) == "Order.cpp");
	STATIC_REQUIRE(gcpp::erasePath(ConstexprString{"C:\\src\\Order.cpp"}) == "Order.cpp");
	STATIC_REQUIRE(gcpp::erasePath(ConstexprString{"Order.cpp"}) == "Order.cpp");
	STATIC_REQUIRE(gcpp::erasePath(ConstexprString{"/src/dir/"}).empty());
	STATIC_REQUIRE(gcpp::erasePath(ConstexprString{""}).empty());
}

TEST_CASE("eraseExtension", "[Where]")
{
	STATIC_REQUIRE(gcpp::eraseExtension(ConstexprString{"Order.cpp"}) == "Order");
	STATIC_REQUIRE(gcpp::eraseExtension(ConstexprString{"/a/b/Order.hpp"}) == "/a/b/Order");
	STATIC_REQUIRE(gcpp::eraseExtension(ConstexprString{"archive.tar.gz"}) == "archive.tar");
	STATIC_REQUIRE(gcpp::eraseExtension(ConstexprString{".hidden"}) == ".hidden");
	STATIC_REQUIRE(gcpp::eraseExtension(ConstexprString{"/a/.hidden"}) == "/a/.hidden");
	STATIC_REQUIRE(gcpp::eraseExtension(ConstexprString{"dir.v2/Makefile"}) == "dir.v2/Makefile");
	STATIC_REQUIRE(gcpp::eraseExtension(gcpp::erasePath(ConstexprString{"/x/y.z/Order.cpp"})) ==
				   "Order");
}

TEST_CASE("trimPath", "[Where]")
{
	constexpr ConstexprString path{"/home/me/rr/src/core/Order.cpp"};
	STATIC_REQUIRE(gcpp::trimPath(path, ConstexprString{"/home/me/rr"}) == "src/core/Order.cpp");
	STATIC_REQUIRE(gcpp::trimPath(path, ConstexprString{"/home/me/rr/"}) == "src/core/Order.cpp");
	// partial directory name must not match
	STATIC_REQUIRE(gcpp::trimPath(path, ConstexprString{"/home/me/r"}) == path);
	// path outside the project stays unchanged (e.g. 3rd party headers)
	STATIC_REQUIRE(gcpp::trimPath(path, ConstexprString{"/opt/qt"}) == path);
	STATIC_REQUIRE(gcpp::trimPath(path, ConstexprString{""}) == path);
	// whole path equal to root
	STATIC_REQUIRE(gcpp::trimPath(ConstexprString{"/a/b"}, ConstexprString{"/a/b"}) == "/a/b");
	// composes with the rest
	STATIC_REQUIRE(gcpp::erasePath(gcpp::trimPath(path, ConstexprString{"/home/me/rr"})) ==
				   "Order.cpp");
}

TEST_CASE("where building blocks report the caller", "[Where]")
{
	STATIC_REQUIRE(where::fileName() == "SuiteWhere");
	STATIC_REQUIRE(where::fileNameWithExtension() == "SuiteWhere.cpp");
	STATIC_REQUIRE(where::filePath().endsWith("SuiteWhere.cpp"));
	STATIC_REQUIRE(where::line() == gcpp::string::fromInteger(__LINE__));
	STATIC_REQUIRE(where::line() == GCPP_LINE);
	STATIC_REQUIRE(where::functionName() == GCPP_FUNCTION_NAME);
	STATIC_REQUIRE(where::filePath() == GCPP_FILE_PATH);
	STATIC_REQUIRE(where::fileName() == GCPP_FILE_NAME);
	STATIC_REQUIRE(where::fileNameWithExtension() == GCPP_FILE_NAME_WITH_EXTENSION);
#ifdef WHERE_PROJECT_ROOT
	STATIC_REQUIRE(where::projectFilePath() == "test/unit-catch2/src/SuiteWhere.cpp");
#else
	STATIC_REQUIRE(where::projectFilePath() == where::filePath());
#endif
}

TEST_CASE("where::functionName inside a template", "[Where]")
{
	STATIC_REQUIRE(functionNameInTemplate(1).endsWith("= int]"));
	STATIC_REQUIRE(functionNameInTemplate(1.0).endsWith("= double]"));
}

TEST_CASE("where::exceptionTag replaces WHERE_EXCEPTION_TAG", "[Where]")
{
	STATIC_REQUIRE(where::exceptionTag() ==
				   "[" + where::functionName() + ":" + where::line() + "] ");

	// clang-format off
	const std::string message = std::string{STATIC_VIEW<where::exceptionTag()>} + "Order rejected"; const auto expectedLine = __LINE__;
	// clang-format on
	const std::string expectedPrefix =
		"[" + std::string{GCPP_FUNCTION_NAME} + ":" + std::to_string(expectedLine);
	CHECK(message.starts_with(expectedPrefix));
	CHECK(message.ends_with("] Order rejected"));
}

TEST_CASE("custom tag from building blocks", "[Where]")
{
	STATIC_REQUIRE(logTag() == "<SuiteWhere@" + where::line() + ">");
	CHECK(std::string_view{gcpp::string::STATIC_C_STR<logTag()>}.starts_with("<SuiteWhere@"));
}

TEST_CASE("toView and toString helpers", "[Where]")
{
	using gcpp::string::toString;
	using gcpp::string::toView;

	STATIC_REQUIRE(toView<ConstexprString{"a"} + "b">() == "ab");
	STATIC_REQUIRE(toView<where::fileName()>() == "SuiteWhere");
	// same pinned object as STATIC_VIEW
	STATIC_REQUIRE(toView<where::fileName()>().data() == STATIC_VIEW<where::fileName()>.data());

	const std::string text = toString<"[" + where::fileNameWithExtension() + "] ">() + "rejected";
	CHECK(text == "[SuiteWhere.cpp] rejected");

	// clang-format off
	const std::string tag = toString<where::exceptionTag()>(); const auto expectedLine = __LINE__;
	// clang-format on
	CHECK(tag.ends_with(":" + std::to_string(expectedLine) + "] "));
}

/// Lives in another translation unit on purpose - see "same value is one object" test
std::string_view otherUnitSharedView()
{
	return STATIC_VIEW<ConstexprString{"shared"} + "_" + "value">;
}
