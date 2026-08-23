#include "Orgki/Helpers/Time.hpp"
#include "libOrgki/Time/TimeRange.hpp"
#include <algorithm>
#include <format>

namespace {
size_t findClosestToMiddle(const std::string& str, char target) {
    if (str.empty()) return str.npos;

    auto len = str.length();
    auto mid = len / 2;

    if (str[mid] == target) return mid;

    auto maxDist = std::max(mid, len - 1 - mid);
    for (size_t step = 1; step <= maxDist; step++) {
        if (mid - step >= 0 && str[mid - step] == target) {
            return mid - step;
        }

        if (mid + step < len && str[mid + step] == target) {
            return mid + step;
        }
    }

    return str.npos;
}
}

Time Helper::StringToTime(const std::string str, bool monthFirst) {
    std::string yearStr{};
    std::string monthStr{};
    std::string dayStr{};

    std::string hourStr{};
    std::string minStr{};
    std::string secStr{};

    if (str.contains('/') || str.contains('-')) { // check if contains a date
        std::string toParse{};
        if (str.contains('+'))
            toParse = str.substr(0, str.find('+'));
        else
            toParse = str;

        while (!toParse.empty()) {
            auto number = toParse.substr(0, toParse.find('/'));
            if (number.size() > 4)
                number = toParse.substr(0, toParse.find('-'));

            if (number.size() == 4 && yearStr.empty()) {
                yearStr = number;
            }
            else {
                if (monthFirst && monthStr.empty()) {
                    monthStr = number;
                }
                else if (dayStr.empty()) {
                    dayStr = number;
                }
                else if (monthStr.empty()) {
                    monthStr = number;
                }
                else {
                    break;
                }
            }

            toParse.erase(0, toParse.find('/') > 4 ? toParse.find('-') + 1 : toParse.find('/') + 1);
        }
    }


    if ((str.contains(':'))) {
        std::string toParse{};
        if (str.contains('+')) // date+time format
            toParse = str.substr(str.find('+') + 1);
        else // only time format
            toParse = str;

        while (!toParse.empty()) {
            if (hourStr.empty())
                hourStr = toParse.substr(0, toParse.find(':'));
            else if (minStr.empty())
                minStr = toParse.substr(0, toParse.find(':'));
            else if (secStr.empty())
                secStr = toParse.substr(0, toParse.find(':'));
            else
                break;

            if (!toParse.contains(':'))
                break;

            toParse.erase(0, toParse.find(':') + 1);
        }
    }

    unsigned int year{};
    unsigned int month{};
    unsigned int day{};
    unsigned int hour{};
    unsigned int min{};
    unsigned int sec{};
    
    if (!yearStr.empty())
		year = std::stoi(yearStr);
    if (!monthStr.empty())
		month = std::stoi(monthStr);
    if (!dayStr.empty())
		day = std::stoi(dayStr);

    if (!hourStr.empty())
		hour = std::stoi(hourStr);
    if (!minStr.empty())
		min = std::stoi(minStr);
    if (!secStr.empty())
		sec = std::stoi(secStr);

    return {
        .m_Year = static_cast<unsigned int>(year),
        .m_Month = static_cast<Month>(month),
        .m_DayNumber = static_cast<unsigned int>(day),
        .m_Hour = static_cast<unsigned int>(hour),
        .m_Minute = static_cast<unsigned int>(min),
        .m_Seconds = static_cast<unsigned int>(sec),
    };
}

std::string Helper::TimeToString(const Time time) {
    return std::format("{}/{}/{}+{}:{}:{}",
        time.m_DayNumber,
        static_cast<unsigned int>(time.m_Month),
        time.m_Year,
        time.m_Hour,
        time.m_Minute,
        time.m_Seconds
    );
}

TimeRange Helper::StringToTimeRange(const std::string str, bool monthFirst) {
    if (!str.contains('-')) return {};

    auto t1Str = str.substr(0, findClosestToMiddle(str, '-'));
    auto t2Str = str.substr(findClosestToMiddle(str, '-') + 1);

    auto t1 = StringToTime(t1Str, monthFirst);
    auto t2 = StringToTime(t2Str, monthFirst);

    return {
        .m_FirstPoint = t1,
        .m_LastPoint = t2
    };
}

std::string Helper::TimeRangeToString(const TimeRange timeRange) {
    return std::format("{} - {}", TimeToString(timeRange.m_FirstPoint), TimeToString(timeRange.m_FirstPoint));
}
