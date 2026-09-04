#pragma once

#include <filesystem>
#include <string>
#include <nlohmann/json.hpp>

#include "SettingsData.hpp"

class SettingsManager {
private:
    std::filesystem::path m_ActivePath{};
    nlohmann::json m_JsonObject{};
    bool m_Loaded{};
public:
    void LoadSettings(const std::filesystem::path& path);
    void ReloadSettings();
    void UnloadSettings();

    bool SettingsLoaded();

    bool Contains(const std::string& key);
    
    void AddSetting(const std::string& key, nlohmann::json value = {});
    SettingsData GetSetting(const std::string& key);
    void RemoveSetting(const std::string& key);

    void SaveToDisk(const std::filesystem::path& path = "");
    void DiscardChanges();
};
