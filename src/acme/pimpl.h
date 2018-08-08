#pragma once

template <class T>
using Pimpl = const std::unique_ptr<T>;

