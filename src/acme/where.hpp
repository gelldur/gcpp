//
// Created by gelldur on 30.03.19.
//
#pragma once

#include "string/ConstexprString.hpp"

#define S1(x) #x
#define S2(x) S1(x)

// WHERE - const char* const should be used as temporary value
#define WHERE (string::make(__PRETTY_FUNCTION__) + ":" + string::make(S2(__LINE__))).get()

// It is safe to store e.g. `constexpr auto where = WHERE_STR;`
#define WHERE_STR (string::make(__PRETTY_FUNCTION__) + ":" + string::make(S2(__LINE__)))
