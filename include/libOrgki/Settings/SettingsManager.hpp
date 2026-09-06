#pragma once

#include <filesystem>
#include <string>
#include <nlohmann/json.hpp>

#include "SettingsData.hpp"
#include "../Commands/Command.hpp"

namespace Orgki {
class Parser;
class SettingsManager {
private:
    friend struct Context;
private:
    std::filesystem::path m_ActivePath{};
    nlohmann::json m_JsonObject{};
    bool m_Loaded{};
private:
    Command::StatusData _AddSettingCallback(std::vector<std::string>& args);
    Command::StatusData _SetSettingCallback(std::vector<std::string>& args);
    Command::StatusData _RemoveSettingCallback(std::vector<std::string>& args);
    Command::StatusData _GetSettingCallback(std::vector<std::string>& args);
    Command::StatusData _GetAllSettingCallback(std::vector<std::string>& args);
    void _AddCommands(Parser& parser);
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
}
