#pragma once

#include "Activity.hpp"
#include "Time/TimeRange.hpp"
#include "Time/TimeTerm.hpp"
#include <map>
#include <string>
#include <vector>

using TableID = unsigned int;

class Plan;
class Table {
private:
    friend class Plan;
    Plan* m_ParentPlan = nullptr;

    std::string m_TableName{};
    TimeTerm m_ActiveTimeTerm{};
    std::vector<std::pair<TimeRange, ActivityID>> m_Activities;
private:
    void AddActivity(ActivityID act, TimeRange range);
public:
    const std::string& GetTableName() const { return m_TableName; }
    size_t GetActivityCount() const { return m_Activities.size(); } 

    std::vector<Activity> GetActivities(Time time);
    std::vector<Activity> GetActivities(TimeRange range);

    std::vector<std::pair<TimeRange, ActivityID>>& GetActivities() { return m_Activities; }
    std::map<Time, ActivityID> GetFormatedActivities();
};
