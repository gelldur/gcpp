//
// Created by Dawid Drozd aka Gelldur on 14.02.18.
//
#pragma once

#include <algorithm>
#include <ostream>
#include <vector>

namespace gcpp::container
{

template <typename ValueAgregate>
class Sack
{
	using container = std::vector<ValueAgregate>;

public:
	using const_iterator = typename container::const_iterator;
	using iterator = typename container::iterator;
	using Type = typename ValueAgregate::Type;

	Sack() = default;

	Sack(const Sack& other)
		: _data{other._data}
	{}

	Sack(Sack&& other) noexcept
		: _data{std::move(other._data)}
	{}

	Sack(std::initializer_list<ValueAgregate> initList)
		: _data{std::move(initList)}
	{}

	Sack(container initList)
		: _data{std::move(initList)}
	{}

	Sack& operator=(const Sack& other)
	{
		_data.clear();
		_data = other._data;
		return *this;
	}

	Sack& operator=(Sack&& other) noexcept
	{
		_data = std::move(other._data);
		return *this;
	}

	void add(const ValueAgregate& value)
	{
		auto found = find(value.getType());
		if(found == end())
		{
			_data.push_back(value);
		}
		else
		{
			*found += value;
		}
	}

	void add(ValueAgregate&& value)
	{
		auto found = find(value.getType());
		if(found == end())
		{
			_data.push_back(std::move(value));
		}
		else
		{
			*found += value;
		}
	}

	void set(const ValueAgregate& value)
	{
		auto found = find(value.getType());
		if(found == end())
		{
			_data.push_back(value);
		}
		else
		{
			*found = value;
		}
	}

	void set(ValueAgregate&& value)
	{
		auto found = find(value.getType());
		if(found == end())
		{
			_data.push_back(std::move(value));
		}
		else
		{
			*found = value;
		}
	}

	ValueAgregate get(Type type) const
	{
		auto found = find(type);
		if(found == end())
		{
			return ValueAgregate{type};
		}
		return *found;
	}

	const_iterator begin() const
	{
		return _data.cbegin();
	}

	const_iterator end() const
	{
		return _data.cend();
	}

	Sack& operator+=(const Sack& other)
	{
		for(const auto& element : other)
		{
			add(element);
		}
		return *this;
	}

	Sack& operator+=(const ValueAgregate& value)
	{
		add(value);
		return *this;
	}

	Sack& operator-=(const Sack& other)
	{
		for(const auto& element : other)
		{
			add(-element);
		}
		return *this;
	}

	Sack& operator-=(const ValueAgregate& value)
	{
		add(-value);
		return *this;
	}

	Sack operator-(const Sack& other) const
	{
		Sack sack{*this}; // copy
		for(const auto& element : other)
		{
			sack.add(-element);
		}
		return sack;
	}

	Sack operator+(const Sack& other) const
	{
		Sack sack{*this}; // copy
		for(const auto& element : other)
		{
			sack.add(element);
		}
		return sack;
	}

	bool operator==(const Sack& rhs) const
	{
		return _data == rhs._data;
	}

	bool operator!=(const Sack& rhs) const
	{
		return !(rhs == *this);
	}

	std::size_t size() const
	{
		return _data.size();
	}

	bool empty() const noexcept
	{
		return _data.empty();
	}

	void clear()
	{
		_data.clear();
	}

	iterator find(const Type& searchType)
	{
		return std::find_if(_data.begin(), _data.end(), [searchType](const ValueAgregate& value) {
			return value.getType() == searchType;
		});
	}

	const_iterator find(const Type& searchType) const
	{
		return std::find_if(_data.cbegin(), _data.cend(), [searchType](const ValueAgregate& value) {
			return value.getType() == searchType;
		});
	}

	template <class Predicate>
	void erase_if(Predicate&& predicate)
	{
		auto removeFrom =
			std::remove_if(_data.begin(), _data.end(), predicate); // TODO std::forward ?
		if(removeFrom != _data.end())
		{
			_data.erase(removeFrom, _data.end());
		}
	}

	void erase(const Type& type)
	{
		for(auto it = _data.begin(); it != _data.end(); ++it)
		{
			if(it->getType() == type)
			{
				_data.erase(it);
				return;
			}
		}
	}

	friend std::ostream& operator<<(std::ostream& stream, const Sack& sack)
	{
		stream << "Sack{ ";
		auto copy = sack._data;
		std::sort(
			copy.begin(), copy.end(), [](const ValueAgregate& left, const ValueAgregate& right) {
				return left.getType() < right.getType();
			});
		for(const auto& element : copy)
		{
			stream << element << ", ";
		}
		stream << "}";
		return stream;
	}

private:
	std::vector<ValueAgregate> _data;
};

} // namespace gcpp::container
