#include "Plan.hpp"
#include "Activity.hpp"
#include "Table.hpp"
#include <print>
#include <stdexcept>

namespace Orgki {
Plan::Plan(std::string name, std::string desc) : m_PlanName(name), m_PlanDescription(desc) {}

std::string Plan::GetName() const {
    return m_PlanName;
}

std::string Plan::GetDescription() const {
    return m_PlanDescription;
}

void Plan::AddTable(Table& table, TableID id) {
    if (m_Tables.contains(id)) {
        std::println("ID already is taken");
        return;
    }

    m_Tables[id] = table;
}

void Plan::AddTable(const std::string& name) {
    Table table{};
    table.m_TableName = name;
    table.m_ParentPlan = this;
    auto id = m_Tables.size();

    AddTable(table, id);
}

Table& Plan::GetTable(TableID id) {
    if (!m_Tables.contains(id))
        throw std::runtime_error("id was not found");

    return m_Tables.at(id);
}

void Plan::SetTable(TableID id, Table& newTable) {
    if (m_Tables.size() <= id) {
        throw std::runtime_error("tableID dosen't exists");
    }
    m_Tables.at(id) = newTable;
}

void Plan::CreateActivity(Activity& act) {
    m_Activities[m_Activities.size()] = act;
}

void Plan::CreateActivity(const std::string& name, const std::string& desc, bool isDynamic) {
    Activity act;
    act.m_ActivityName = name;
    act.m_ActivityDescription = desc;
    act.m_Dynamic = isDynamic;

    CreateActivity(act);
}

Activity& Plan::GetActivity(const std::string& name) {
    for (auto& [id, act] : m_Activities) {
        if (act.m_ActivityName == name) {
            return act;
        }
    }

    throw std::runtime_error("duh");
}

Activity& Plan::GetActivity(ActivityID id) {
    if (m_Activities.size() <= id) {
        throw std::runtime_error("activityID dosen't exists");
    }

    return m_Activities.at(id);
}

void Plan::SetActivity(ActivityID id, Activity& newAct) {
    if (m_Activities.size() <= id) {
        throw std::runtime_error("activityID dosen't exists");
    }
    
    m_Activities.at(id) = newAct;
}

void Plan::AddActivityToTable(TableID tableID, ActivityID actID, TimeRange range) {
    GetTable(tableID).AddActivity(actID, range);
}
}
