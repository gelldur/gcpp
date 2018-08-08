#include "i18n.h"

#include <boost/locale.hpp>

/**
 * Example cmake:
 * #Needed for i18n::tr
 * if(NOT TARGET Boost::locale)
 * 	find_package(Boost COMPONENTS locale REQUIRED)
 * endif()
 *
 * target_link_libraries(CommonCpp PRIVATE Boost::locale)
 */

namespace i18n
{

std::string tr(std::string_view key, const std::locale& locale)
{
	return boost::locale::gettext(key.data(), locale);
}

} // namespace i18n
