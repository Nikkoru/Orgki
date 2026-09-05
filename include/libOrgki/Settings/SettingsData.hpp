#pragma once

#include <string>
#include <nlohmann/json.hpp>

namespace Orgki {
class SettingsData {
private:
    std::string m_Name;
    nlohmann::json& m_JsonData;
public:
    SettingsData(std::string name, nlohmann::json& json) : m_Name(name), m_JsonData(json) {}

    bool HasValue(const std::string& key);
    template<typename TVal>
    TVal GetValue(const std::string& key);
    void AddValue(const std::string& key, nlohmann::json value = nullptr);
    void SetValue(const std::string& key, nlohmann::json value);
    void RemoveValue(const std::string& key);
};
}
