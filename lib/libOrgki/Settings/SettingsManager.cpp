#include "Settings/SettingsManager.hpp"
#include "Settings/SettingsData.hpp"
#include <fstream>
#include <nlohmann/json.hpp>

namespace fs = std::filesystem;
using json = nlohmann::json;

namespace Orgki {
void SettingsManager::LoadSettings(const std::filesystem::path& path) {
    auto f = std::ifstream{ path };
    if (!f.is_open()) return;

    UnloadSettings();
    if (f.peek() != std::ifstream::traits_type::eof())
        m_JsonObject = json::parse(f);
    else
        m_JsonObject = json{};

    f.close();
    m_ActivePath = path;
    m_Loaded = true;
}

void SettingsManager::ReloadSettings() {
    if (m_ActivePath.empty()) return;

    LoadSettings(m_ActivePath);
}

void SettingsManager::UnloadSettings() {
    if (!SettingsLoaded()) return;

    m_JsonObject.clear();
    m_Loaded = false;
}

bool SettingsManager::SettingsLoaded() {
    // return !m_JsonObject.empty();
    return m_Loaded;
}

bool SettingsManager::Contains(const std::string& key) {
    if (!SettingsLoaded()) return false;
    
    return m_JsonObject.contains(key);
}

void SettingsManager::AddSetting(const std::string& key, json value) {
    if (!SettingsLoaded()) return;
    if (Contains(key)) return;

    if (!value.empty() && value.is_object())
        m_JsonObject[key] = value;
    else
        m_JsonObject[key] = json::object();
}

void SettingsManager::RemoveSetting(const std::string& key) {
    if (!Contains(key)) return;
}

SettingsData SettingsManager::GetSetting(const std::string& key) {
    return SettingsData{ key, m_JsonObject.at(key) };
}

void SettingsManager::SaveToDisk(const fs::path& path) {
    fs::path pathToSave = path;

    if (pathToSave.empty()) 
        pathToSave = m_ActivePath;

    if (!fs::exists(pathToSave)) return;

    auto o = std::ofstream{ pathToSave, std::ios::out | std::ios::trunc };
    if (!o.is_open()) return;

    o << std::setw(4) << m_JsonObject << std::endl;
    o.close();
}

void SettingsManager::DiscardChanges() {

}
}
