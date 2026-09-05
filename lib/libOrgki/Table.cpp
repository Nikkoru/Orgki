#include "Table.hpp"

#include "Plan.hpp"
#include <algorithm>
#include <print>

namespace Orgki {
void Table::AddActivity(ActivityID act, TimeRange range) {
    if (m_ParentPlan && m_ParentPlan->ActivityExists(act)) {
        m_Activities.emplace_back(std::make_pair(range, act));
        std::sort(m_Activities.begin(), m_Activities.end());
    }
    else std::println("failed to add activity");
}
}
