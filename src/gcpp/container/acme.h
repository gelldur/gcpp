//
// Created by Dawid Drozd aka Gelldur on 04.12.17.
//
#pragma once

#include <algorithm>

namespace gcpp::container
{

template <typename ContainerT, typename PredicateT>
void erase_if(ContainerT& items, const PredicateT& predicate)
{
	for(auto it = items.begin(); it != items.end();)
	{
		if(predicate(*it))
		{
			it = items.erase(it);
		}
		else
		{
			++it;
		}
	}
}

template <typename ContainerT, typename PredicateT>
void remove_if(ContainerT& items, const PredicateT& predicate) // TODO
{
	auto removeFrom = std::remove_if(std::begin(items), std::end(items), predicate);
	if(removeFrom != std::end(items))
	{
		items.erase(removeFrom, std::end(items));
	}
}

template <typename ContainerT, typename Iterator>
bool erase_fast(ContainerT& items, const Iterator& iterator)
{
	if(items.empty() || iterator == std::end(items))
	{
		return false;
	}
	std::swap(items.back(), *iterator);
	items.pop_back();
	return true;
}

template <typename InputIterator, typename ValueType>
InputIterator closest(InputIterator first, InputIterator last, ValueType value)
{
	return std::min_element(first, last, [&](ValueType x, ValueType y) {
		return std::abs(x - value) < std::abs(y - value);
	});
}

template <typename T>
T& append(T& destination, const T& elements)
{
	destination.insert(std::end(destination), std::begin(elements), std::end(elements));
	return destination;
}

} // namespace gcpp::container
