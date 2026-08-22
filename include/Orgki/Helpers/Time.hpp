#pragma once

#include "libOrgki/Time/Time.hpp"
#include "libOrgki/Time/TimeRange.hpp"

#include <string>

namespace Helper {
Time StringToTime(const std::string str, bool monthFirst = false);
TimeRange StringToTimeRange(const std::string str, bool monthFirst = false);
}
