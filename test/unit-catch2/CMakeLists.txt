cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

# http://www.levelofindirection.com/journal/2010/12/28/unit-testing-in-c-and-objective-c-just-got-easier.html
# Thanks for CATCH!

# Dependencies
enable_testing()

include(FetchContent)
option(CATCH_INSTALL_DOCS "" OFF)
set(CATCH_CONFIG_DEFAULT_REPORTER "compact" CACHE STRING "" FORCE)
set(CATCH_CONFIG_ENABLE_ALL_STRINGMAKERS 1 CACHE BOOL "" FORCE)

fetchcontent_declare(Catch2
	GIT_SHALLOW TRUE
	GIT_REPOSITORY https://github.com/catchorg/Catch2.git
	GIT_TAG v3.5.0)

fetchcontent_makeavailable(Catch2)

# Target definition
add_executable(UnitTest-gcpp-catch2
	src/main.cpp
	src/SuiteFromString.cpp
	src/SuiteNiceTime.cpp
	src/SuitePercent.cpp
	src/SuiteSack.cpp
	src/SuiteToString.cpp
	src/SuiteWhere.cpp
		src/SuiteConstexprString.cpp
		src/SuiteTypeName.cpp
)

set(DEBUG_FLAGS
	-O0 -fno-inline
	-DDEBUG
	#-D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC
	)

target_compile_options(UnitTest-gcpp-catch2 PUBLIC "$<$<CONFIG:DEBUG>:${DEBUG_FLAGS}>")
target_link_libraries(UnitTest-gcpp-catch2 PUBLIC gcpp Catch2::Catch2 Catch2::Catch2WithMain)

add_test(NAME gcpp-catch2.UnitTests COMMAND UnitTest-gcpp-catch2)
