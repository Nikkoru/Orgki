#pragma once

#include "Time.hpp"
#include <compare>

namespace Orgki {
struct TimeRange {
    Time m_FirstPoint{};
    Time m_LastPoint{};

    std::strong_ordering operator<=>(const TimeRange& other) const = default;
};
}
