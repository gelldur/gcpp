#pragma once

#include <locale>
#include <string>
#include <string_view>

namespace gcpp::i18n
{

// You are responsible to implement this method for own engine search, or use one of that I already
// created ;)
std::string tr(std::string_view key, const std::locale& locale = std::locale());

} // namespace gcpp::i18n
