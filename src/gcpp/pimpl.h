#pragma once

#include <memory>

template <class T>
using Pimpl = std::unique_ptr<T>;
