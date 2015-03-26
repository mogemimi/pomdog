// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_STRINGFORMAT_FF9D5A44_E2AB_4484_815C_4BEC31D0796E_HPP
#define POMDOG_STRINGFORMAT_FF9D5A44_E2AB_4484_815C_4BEC31D0796E_HPP

#include "Pomdog/Basic/Export.hpp"
#include <string>

namespace Pomdog {

std::string POMDOG_EXPORT StringFormat(char const* format, ...)
#if defined(__has_attribute)
#if __has_attribute(format)
__attribute__((__format__(printf, 1, 2)));
#endif
#elif __GNUC__ >= 4
__attribute__((__format__(printf, 1, 2)));
#else
;
#endif

}// namespace Pomdog

#endif // !defined(POMDOG_STRINGFORMAT_FF9D5A44_E2AB_4484_815C_4BEC31D0796E_HPP)
