//
// Created by Dawid Drozd aka Gelldur on 8/3/16.
//

#pragma once

template<class T, class V>
class ChangeOnEnd
{
public:
	ChangeOnEnd(T& value, V changeToValue)
			: _value(value)
			, _change(std::move(changeToValue))
	{
	}

	~ChangeOnEnd()
	{
		_value = std::move(_change);
	}

private:
	T& _value;
	V _change;
};


