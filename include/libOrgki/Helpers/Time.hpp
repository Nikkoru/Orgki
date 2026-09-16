#pragma once

#include "../Time/Time.hpp"
#include "../Time/TimeRange.hpp"

#include <string>

namespace Orgki::Helper {
Time StringToTime(const std::string str, bool monthFirst = false);
std::string TimeToString(const Orgki::Time time);
TimeRange StringToTimeRange(const std::string str, bool monthFirst = false);
std::string TimeRangeToString(const Orgki::TimeRange time);
}
