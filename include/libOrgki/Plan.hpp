#pragma once

#include <string>
#include <map>

#include "Table.hpp"
#include "Activity.hpp"

namespace Orgki {
class Plan {
private:
    std::map<TableID, Table> m_Tables;
    std::map<ActivityID, Activity> m_Activities;
    std::string m_PlanName;
    std::string m_PlanDescription;
public:
    void AddTable(Table& table, TableID id);
    void AddTable(const std::string& tableName);
    Table& GetTable(TableID id);
    void SetTable(TableID id, Table& newTable);
    bool TableExists(TableID id) { return m_Tables.contains(id); }
    size_t GetTableCount() { return m_Tables.size(); }
    std::map<TableID, Table>& GetTables() { return m_Tables; };

    void CreateActivity(Activity& act);
    void CreateActivity(const std::string& name, const std::string& desc = "", bool isDynamic = false);
    Activity& GetActivity(const std::string& name);
    Activity& GetActivity(ActivityID id);
    void SetActivity(ActivityID id, Activity& newAct);
    bool ActivityExists(ActivityID id) { return m_Activities.contains(id); }
    size_t GetActivityCount() { return m_Activities.size(); }
    std::map<ActivityID, Activity>& GetActivities() { return m_Activities; };

    void AddActivityToTable(TableID tableID, ActivityID actID, TimeRange range);
};
}
