#pragma once

#include <memory>

/**
 * This isn't in any namespace because it should provide nice and intentional alias.
 *
 * How to use: When using as class member
 * 1. Define dtor in cpp file. (IMPORTNAT)
 * 2. Predeclare MyExampleClazz e.g. class MyExampleClazz;
 * 3. E.g. Pimpl<MyExampleClazz> _dataSource;
 * 4. In cpp file include file with definition of MyExampleClazz
 */

template <class T>
using Pimpl = std::unique_ptr<T>;
