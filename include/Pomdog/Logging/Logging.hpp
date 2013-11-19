//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_LOGGING_HPP
#define POMDOG_LOGGING_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include "LogEntry.hpp"
#include "LogStream.hpp"

namespace Pomdog {

/// @addtogroup Framework
/// @{
/// @addtogroup Logging
/// @{

struct Logging
{
	static LogStream Stream(LoggingLevel verbosity = LoggingLevel::Verbose);
	static LogStream Stream(std::string const& channelName, LoggingLevel verbosity = LoggingLevel::Verbose);
};

/// @}
/// @}

}// namespace Pomdog

#endif // !defined(POMDOG_LOGGING_HPP)
