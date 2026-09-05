#include "Settings/SettingsData.hpp"
#include <string>

using json = nlohmann::json;

namespace Orgki {
bool SettingsData::HasValue(const std::string& key) {
    if (m_JsonData.empty()) return false;

    return m_JsonData.contains(key);
}

template<typename TVal>
TVal SettingsData::GetValue(const std::string& key) {
    if (!HasValue(key)) return {};
    
    return m_JsonData.at(key).get<TVal>();
}

void SettingsData::AddValue(const std::string& key, json value) {
    if (HasValue(key)) return; 

    m_JsonData[key] = value;
}

void SettingsData::SetValue(const std::string& key, json value) {
    if (!HasValue(key)) return;

    m_JsonData[key] = value;
}

void SettingsData::RemoveValue(const std::string& key) {
    if (!HasValue(key)) return;

    m_JsonData.erase(key);
}
}
