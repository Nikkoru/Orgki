#pragma once

#include "DayWeek.hpp"
#include "Month.hpp"
#include <compare>

namespace Orgki {
struct Time {
    unsigned int m_Year{};
    Month m_Month{};
    unsigned int m_DayNumber{};
    Day m_Day{};
    unsigned int m_Hour{};
    unsigned int m_Minute{};
    unsigned int m_Seconds{};

    std::strong_ordering operator<=>(const Time& other) const = default;
};
}
