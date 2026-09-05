#pragma once

#include "libOrgki/Time/Time.hpp"
#include "libOrgki/Time/TimeRange.hpp"

#include <string>

namespace Helper {
Orgki::Time StringToTime(const std::string str, bool monthFirst = false);
std::string TimeToString(const Orgki::Time time);
Orgki::TimeRange StringToTimeRange(const std::string str, bool monthFirst = false);
std::string TimeRangeToString(const Orgki::TimeRange time);
}
