//
// Created by Dawid Drozd aka Gelldur on 03.12.17.
//
#pragma once

#include <geometry/Point.h>

namespace math
{
namespace Bezier
{

template<typename Type>
Dexode::Point<Type> cubic(const Type& t
						  , const Dexode::Point<Type>& A
						  , const Dexode::Point<Type>& B
						  , const Dexode::Point<Type>& C
						  , const Dexode::Point<Type>& D)
{
	Dexode::Point<Type> result = A * (1 - t) * (1 - t) * (1 - t);//A(1-t)^3
	result += 3 * B * t * (1 - t) * (1 - t);// 3Bt(1-t)^2
	result += 3 * C * t * t * (1 - t);// 3Ct^2(1-t)
	result += D * t * t * t;// Dt^3
	return result;
}

}
}


