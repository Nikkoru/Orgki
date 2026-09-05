#pragma once

#include <string>

namespace Orgki {
using ActivityID = unsigned int;

class Plan;
class Activity {
private:
    friend class Plan;
    ActivityID m_ID{};
    std::string m_ActivityName{};
    std::string m_ActivityDescription{};
    bool m_Dynamic{ false };
public:
    const std::string GetActivityName() const { return m_ActivityName; }
    const std::string GetActivityDescription() const { return m_ActivityDescription; }
    bool IsDynamic() const { return m_Dynamic; }

    void SetActivityName(std::string name) { m_ActivityName = name; }
    void SetActivityDescription(std::string desc) { m_ActivityDescription = desc; }
    void SetDynamic(bool dynamic) { m_Dynamic = dynamic; }
};
}
