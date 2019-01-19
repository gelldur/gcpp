//
// Created by Dawid Drozd aka Gelldur on 8/3/16.
//

#pragma once

/**
 * Also checkout std::lock_guard
 * http://en.cppreference.com/w/cpp/thread/lock_guard
 *
 * C++ standard
 *
 * * 3.7.2/3
 * If a named automatic object has initialization or a destructor with side effects, it shall not be destroyed before the end of its block, nor shall it be eliminated as an optimization even if it appears to be unused
 *
 *
 * https://stackoverflow.com/questions/2087600/is-a-c-destructor-guaranteed-not-to-be-called-until-the-end-of-the-block
 */

template <class T, class V>
class ChangeOnEnd
{
public:
	ChangeOnEnd(T& value, V changeToValue)
		: _value(value)
		, _change(std::move(changeToValue))
	{}

	~ChangeOnEnd()
	{
		_value = std::move(_change);
	}

private:
	T& _value;
	V _change;
};
