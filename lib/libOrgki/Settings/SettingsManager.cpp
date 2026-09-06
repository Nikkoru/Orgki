#include "Settings/SettingsManager.hpp"
#include "Commands/Command.hpp"
#include "Commands/Parser.hpp"
#include "Settings/SettingsData.hpp"
#include <fstream>
#include <nlohmann/json.hpp>

namespace fs = std::filesystem;
using json = nlohmann::json;

namespace {
bool isNumeric(const std::string& str) {
    std::istringstream s(str);
    double d;

    return (s >> d) && (s.eof());
}
}

namespace Orgki {
Command::StatusData SettingsManager::_AddSettingCallback(std::vector<std::string>& args) {
    const std::string ASCII_RED_FG      { "\x1b[31m" };
    const std::string ASCII_GREEN_FG    { "\x1b[32m" };
    const std::string ASCII_RESET       { "\x1b[0m" };

    ReloadSettings();
    if (!Contains("testSettings")) {
        AddSetting("testSettings");
        SaveToDisk();
    }
    auto setting = GetSetting("testSettings");

    auto key = args[0];
    
    if (setting.HasValue(key)) return {
        .msg = std::format("{}key {}\"{}\"{} already exists", ASCII_RED_FG, ASCII_RESET, key, ASCII_RED_FG),
        .status = Orgki::Command::Status::INVALID_ARGS
    };

    nlohmann::json value{};
    if (args.size() == 2)
        value = nlohmann::json::parse(args[1]);

    setting.AddValue(key, value);

    SaveToDisk();

    return { 
        .msg = std::format("{}correctly created key {}\"{}\"", ASCII_GREEN_FG, ASCII_RESET, key),
        .status = Orgki::Command::Status::OK 
    };
}

Command::StatusData SettingsManager::_SetSettingCallback(std::vector<std::string>& args) {
    const std::string ASCII_RED_FG      { "\x1b[31m" };
    const std::string ASCII_GREEN_FG    { "\x1b[32m" };
    const std::string ASCII_RESET       { "\x1b[0m" };

    ReloadSettings();
    if (!Contains("testSettings")) {
        AddSetting("testSettings");
        SaveToDisk();
    }
    auto setting = GetSetting("testSettings");

    auto key = args[0];
    if (!setting.HasValue(key)) return {
        .msg = std::format("{}key {}\"{}\"{} doesn't exists{}", ASCII_RED_FG, ASCII_RESET, key, ASCII_RED_FG, ASCII_RESET),
        .status = Orgki::Command::Status::INVALID_ARGS
    };

    nlohmann::json value;
    
    auto valueStr = args[1];
    if (!(
        isNumeric(valueStr) ||
        (valueStr.starts_with('\"') && valueStr.ends_with('\"')) ||
        (valueStr.starts_with('\'') && valueStr.ends_with('\'')) ||
        (valueStr.contains('{') && valueStr.contains('}')) ||
        (valueStr.contains('[') && valueStr.contains(']')) ||
        valueStr == "true" || valueStr == "false" || valueStr == "null"
    )) {
        valueStr.insert(valueStr.begin(), '\"');
        valueStr.push_back('\"');
    }

    value = nlohmann::json::parse(valueStr);

    setting.SetValue(key, value);
    SaveToDisk();

    return {
        .msg = std::format("{}key {}\"{}\"{} was setted to {}{}{}", ASCII_GREEN_FG, ASCII_RESET, key, ASCII_GREEN_FG, ASCII_RESET, valueStr, ASCII_RESET),
        .status = Orgki::Command::Status::OK
    };
}

Command::StatusData SettingsManager::_RemoveSettingCallback(std::vector<std::string>& args) {
    const std::string ASCII_RED_FG      { "\x1b[31m" };
    const std::string ASCII_GREEN_FG    { "\x1b[32m" };
    const std::string ASCII_RESET       { "\x1b[0m" };

    ReloadSettings();
    if (!Contains("testSettings")) {
        AddSetting("testSettings");
        SaveToDisk();
    }
    auto setting = GetSetting("testSettings");

    auto key = args[0];

    if (!setting.HasValue(key)) return {
        .msg = std::format("{}key {}\"{}\"{} doesn't exists{}", ASCII_RED_FG, ASCII_RESET, key, ASCII_RED_FG, ASCII_RESET),
        .status = Orgki::Command::Status::INVALID_ARGS
    };

    setting.RemoveValue(key);
    SaveToDisk();

    return {
        .msg = std::format("{}Successfully removed key {}\"{}\"", ASCII_GREEN_FG, ASCII_RESET, key),
        .status = Orgki::Command::Status::OK
    };
}

Command::StatusData SettingsManager::_GetSettingCallback(std::vector<std::string>& args) {
    return {
        .msg = "Not implemented",
        .status = Orgki::Command::Status::FAILED
    };
}

Command::StatusData SettingsManager::_GetAllSettingCallback(std::vector<std::string>& args) {
    return {
        .msg = "Not implemented",
        .status = Orgki::Command::Status::FAILED
    };
}

void SettingsManager::_AddCommands(Parser& parser) {
    const std::string ASCII_RED_FG      { "\x1b[31m" };
    const std::string ASCII_GREEN_FG    { "\x1b[32m" };
    const std::string ASCII_RESET       { "\x1b[0m" };

    auto addSettingCallback = Command::Callback{ std::bind(&SettingsManager::_AddSettingCallback, this, std::placeholders::_1) };
    auto setSettingCallback = Command::Callback{ std::bind(&SettingsManager::_SetSettingCallback, this, std::placeholders::_1) };
    auto removeSettingCallback = Command::Callback{ std::bind(&SettingsManager::_RemoveSettingCallback, this, std::placeholders::_1) };
    auto getSettingCallback = Command::Callback{ std::bind(&SettingsManager::_GetSettingCallback, this, std::placeholders::_1) };
    auto getAllSettingsCallback = Command::Callback{ std::bind(&SettingsManager::_GetAllSettingCallback, this, std::placeholders::_1) };

    parser.AddCommandBulk({
        {
            .command = "addSetting",
            .description = "Adds a setting based on the given key and optional default value",
            .usage = "[SETTING_KEY] <VALUE>",
            .minArgs = 1,
            .maxArgs = 2,
            .callback = addSettingCallback
        },
        {
            .command = "setSetting",
            .description = "Sets a setting based on the given key and value",
            .usage = "[SETTING_KEY] [VALUE]",
            .minArgs = 2,
            .maxArgs = 2,
            .callback = setSettingCallback
        },
        {
            .command = "removeSetting",
            .description = "Removes the setting on the given key",
            .usage = "[SETTING_KEY]",
            .minArgs = 1,
            .maxArgs = 1,
            .callback = removeSettingCallback
        },
        {
            .command = "getSetting",
            .description = "Gets a setting value based on the given key",
            .usage = "[SETTING_KEY]",
            .minArgs = 1,
            .maxArgs = 1,
            .callback = getSettingCallback
        },
        {
            .command = "getAllSettings",
            .description = "Gets all the settings",
            .usage = "",
            .minArgs = 0,
            .maxArgs = 0,
            .callback = getAllSettingsCallback
        },
    });
}

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
