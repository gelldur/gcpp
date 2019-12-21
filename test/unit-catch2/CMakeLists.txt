cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

# http://www.levelofindirection.com/journal/2010/12/28/unit-testing-in-c-and-objective-c-just-got-easier.html
# Thanks for CATCH!

set(CMAKE_CXX_STANDARD 17)

# Dependencies
enable_testing()

find_package(Catch2 2.6 REQUIRED)

# Target definition
add_executable(UnitTest-gcpp-catch2
	src/main.cpp
	src/SuiteNiceTime.cpp
	src/SuiteSack.cpp
	src/SuiteWhere.cpp
	)

set(DEBUG_FLAGS
	-O0 -fno-inline
	-DDEBUG
	#-D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC
	)

target_compile_options(UnitTest-gcpp-catch2 PUBLIC "$<$<CONFIG:DEBUG>:${DEBUG_FLAGS}>")
target_link_libraries(UnitTest-gcpp-catch2 PUBLIC gcpp Catch2::Catch2)

add_test(NAME gcpp-catch2.UnitTests COMMAND UnitTest-gcpp-catch2)
