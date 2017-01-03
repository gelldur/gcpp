//
// Created by Dawid Drozd aka Gelldur on 10/11/16.
//

#pragma once

//Source code from: http://stackoverflow.com/a/20170989/1052261

#include <type_traits>
#include <typeinfo>

#ifndef _MSC_VER
#   include <cxxabi.h>

#endif
#include <memory>
#include <string>
#include <cstdlib>

template<class T>
std::string
type_name()
{
	typedef typename std::remove_reference<T>::type TR;
	std::unique_ptr<char, void (*)(void*)> own
			(
#ifndef _MSC_VER
			abi::__cxa_demangle(typeid(TR).name(), nullptr, nullptr, nullptr),
#else
			nullptr,
#endif
			std::free
	);
	std::string r = own != nullptr ? own.get() : typeid(TR).name();
	if (std::is_const<TR>::value)
	{
		r += " const";
	}
	if (std::is_volatile<TR>::value)
	{
		r += " volatile";
	}
	if (std::is_lvalue_reference<T>::value)
	{
		r += "&";
	}
	else if (std::is_rvalue_reference<T>::value)
	{
		r += "&&";
	}
	return r;
}
