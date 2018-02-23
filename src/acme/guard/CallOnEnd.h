//
// Created by Dawid Drozd aka Gelldur on 10/1/17.
//

#pragma once

#include <functional>

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

class CallOnEnd
{
public:
	CallOnEnd(std::function<void()> onDie)
			: _onDie(std::move(onDie))
	{
	}

	~CallOnEnd()
	{
		if (_onDie)
		{
			_onDie();
		}
	}

private:
	std::function<void()> _onDie;
};


