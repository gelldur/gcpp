#
cmake_minimum_required(VERSION 3.11)
project(gcpp-dev
	VERSION 2.0.0
	LANGUAGES CXX
	)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# For development
add_library(gcpp
	src/gcpp/color.hpp
	src/gcpp/container/acme.h
	src/gcpp/container/Sack.h
	src/gcpp/exception/fail.cpp src/gcpp/exception/fail.hpp
	src/gcpp/guard/CallOnEnd.h
	src/gcpp/guard/ChangeOnEnd.h
	src/gcpp/i18n/i18n-boost.cpp
	src/gcpp/i18n/i18n.h
	src/gcpp/input/SecretCin.cpp src/gcpp/input/SecretCin.h
	src/gcpp/math/Bezier.h
	src/gcpp/math/Percent.h
	src/gcpp/math/Point.h
	src/gcpp/math/traits.h
	src/gcpp/nice/number.cpp src/gcpp/nice/number.hpp
	src/gcpp/nice/time.cpp src/gcpp/nice/time.hpp
	src/gcpp/pimpl.h
	src/gcpp/random/MersenneTwister.cpp src/gcpp/random/MersenneTwister.h
	src/gcpp/string/acme.cpp src/gcpp/string/acme.hpp
	src/gcpp/string/ConstexprString.hpp
	src/gcpp/time.cpp src/gcpp/time.hpp
	src/gcpp/type_name.h
	src/gcpp/where.hpp
	#	src/gcpp/wrapper/lib/poco/MessageStream.cpp src/gcpp/wrapper/lib/poco/MessageStream.hpp
	#	src/gcpp/wrapper/lib/poco/PocoRequest.cpp src/gcpp/wrapper/lib/poco/PocoRequest.h
	)

target_compile_definitions(gcpp PUBLIC WHERE_PROJECT_ROOT="${CMAKE_CURRENT_SOURCE_DIR}")

target_include_directories(gcpp PUBLIC src/)

target_compile_options(gcpp
	PUBLIC
	-Wall -Wpedantic -Wextra
	-Wnon-virtual-dtor
	-Werror
	-Wno-unused-parameter
	-Wno-error=deprecated-declarations
	# ----------------------------------------
	PRIVATE
	-fPIC
	)

add_subdirectory(test/)
