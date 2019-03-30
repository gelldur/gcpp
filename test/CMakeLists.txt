cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

# http://www.levelofindirection.com/journal/2010/12/28/unit-testing-in-c-and-objective-c-just-got-easier.html
# Thanks for CATCH!

project(CommonCppTest)

set(CMAKE_CXX_STANDARD 17)

# Dependencies
enable_testing()

find_package(Catch2 2.6 REQUIRED)

##################################################
set(COMMON_CPP_SRC ${CMAKE_CURRENT_SOURCE_DIR}/../src)

add_library(CommonCpp
	${COMMON_CPP_SRC}/acme/beautify.cpp
	)

target_include_directories(CommonCpp PUBLIC
	${COMMON_CPP_SRC}/
	)
##################################################

# Target definition
add_executable(CommonCppTest
	src/acme/where_test.cpp
	)

target_compile_options(CommonCppTest PUBLIC
	-Wall -pedantic
	-Wno-unused-private-field
	-Wnon-virtual-dtor
	-Wno-gnu
	-Werror
	)

set(DEBUG_FLAGS
	-O0 -fno-inline
	-DDEBUG
	#-D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC
	)

target_compile_options(CommonCppTest PUBLIC "$<$<CONFIG:DEBUG>:${DEBUG_FLAGS}>")
target_link_libraries(CommonCppTest PUBLIC CommonCpp Catch2::Catch2)

add_test(NAME CommonCpp.UnitTests COMMAND CommonCppTest)
