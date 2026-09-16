#include "Context.hpp"
#include <format>
#include <lauxlib.h>
#include <print>
#include <string_view>

// namespace {
// void AddBuiltInCommands(Orgki::Parser& parser) {
//     parser.AddCommandBulk({
//         // {
//         //     .command = "lua",
//         //     .description = "Evaluates the next argument as Lua",
//         //     .usage = "[LUA_CODE]",
//         //     .minArgs = 1,
//         //     .maxArgs = 1,
//         //     .callback = luaCallback
//         // },
//         // {
//         //     .command = "luaSource",
//         //     .description = "Evaluates or sources the given Lua file",
//         //     .usage = "[FILE]",
//         //     .minArgs = 1,
//         //     .maxArgs = 1,
//         //     .callback = luaSourceCallback
//         // },
//         {
//             .command = "createTable",
//             .description = "",
//             .usage = "[NAME]",
//             .minArgs = 1,
//             .maxArgs = 1,
//             .callback = createTableCallback
//         },
//         {
//             .command = "seeTables",
//             .description = "",
//             .usage = "",
//             .minArgs = 0,
//             .maxArgs = 0,
//             .callback = seeTablesCallback
//         },
//         {
//             .command = "editTable",
//             .description = "",
//             .usage = "[ID] [ATTRIBUTE] <VALUE>",
//             .minArgs = 2,
//             .maxArgs = 3,
//             .callback = editTableCallback
//         },
//         {
//             .command = "getTable",
//             .description = "",
//             .usage = "[ID]",
//             .minArgs = 1,
//             .maxArgs = 1,
//             .callback = getTableCallback
//         },
//         {
//             .command = "addActivityToTable",
//             .description = "[ACTIVITY_ID] [TABLE_ID] <TIME_RANGE>",
//             .usage = "",
//             .minArgs = 2,
//             .maxArgs = 3,
//             .callback = addActivityToTableCallback
//         },
//         {
//             .command = "seeActivities",
//             .description = "",
//             .usage = "",
//             .minArgs = 0,
//             .maxArgs = 0,
//             .callback = seeActivitiesCallback
//         },
//         {
//             .command = "createActivity",
//             .description = "",
//             .usage = "[NAME] <DESCRIPTION> <\"dynamic\">",
//             .minArgs = 1,
//             .maxArgs = 3,
//             .callback = createActivityCallback
//         },
//         {
//             .command = "editActivity",
//             .description = "",
//             .usage = "[ID] [ATTRIBUTE] <VALUE>",
//             .minArgs = 2,
//             .maxArgs = 3,
//             .callback = editActivityCallback
//         },
//         {
//             .command = "whatOn",
//             .description = "WIP",
//             .usage = "[TIME / TIME_RANGE] <TABLE>",
//             .minArgs = 1,
//             .maxArgs = 2,
//             .callback = whatOnCallback
//         },
//         {
//             .command = "addSetting",
//             .description = "Adds a setting based on the given key and optional default value",
//             .usage = "[SETTING_KEY] <VALUE>",
//             .minArgs = 1,
//             .maxArgs = 2,
//             .callback = addSettingCallback
//         },
//         {
//             .command = "setSetting",
//             .description = "Sets a setting based on the given key and value",
//             .usage = "[SETTING_KEY] [VALUE]",
//             .minArgs = 2,
//             .maxArgs = 2,
//             .callback = setSettingCallback
//         },
//         {
//             .command = "removeSetting",
//             .description = "Removes the setting on the given key",
//             .usage = "[SETTING_KEY]",
//             .minArgs = 1,
//             .maxArgs = 1,
//             .callback = removeSettingCallback
//         },
//         {
//             .command = "getSetting",
//             .description = "Gets a setting value based on the given key",
//             .usage = "[SETTING_KEY]",
//             .minArgs = 1,
//             .maxArgs = 1,
//             .callback = getSettingCallback
//         },
//         {
//             .command = "getAllSettings",
//             .description = "Gets all the settings",
//             .usage = "",
//             .minArgs = 0,
//             .maxArgs = 0,
//             .callback = getAllSettingsCallback
//         },
//     });
// }
// }

namespace Orgki {
Command::StatusData Context::_BuiltinLuaCallback(std::vector<std::string>& args) {
    const std::string ASCII_RESET       { "\x1b[0m" };
    const std::string ASCII_RED_FG      { "\x1b[31m" };
    auto code = args[0];

    auto status = luaL_dostring(luaState, code.c_str());

    if (status != LUA_OK) {
        auto errStr = lua_tostring(luaState, -1);   
        return {
            .msg = std::format("{}Lua failed to execute: {}{}", ASCII_RED_FG, ASCII_RESET, errStr),
            .status = Orgki::Command::Status::FAILED
        };
    }

    return {
        .msg = "evaluated",
        .status = Orgki::Command::Status::OK
    };
}

Command::StatusData Context::_BuiltinLuaSourceCallback(std::vector<std::string>& args) {
    const std::string ASCII_RESET       { "\x1b[0m" };
    const std::string ASCII_RED_FG      { "\x1b[31m" };

    auto code = args[0];

    if (!std::filesystem::exists(code)) return {
        .msg = std::format("{}file not found: {}\"{}\"", ASCII_RED_FG, ASCII_RESET, code),
        .status = Orgki::Command::Status::INVALID_ARGS
    };

    auto status = luaL_dofile(luaState, code.c_str());

    if (status != LUA_OK) {
        auto errStr = lua_tostring(luaState, -1);   
        return {
            .msg = std::format("{}Lua failed to execute: {}{}", ASCII_RED_FG, ASCII_RESET, errStr),
            .status = Orgki::Command::Status::FAILED
        };
    }
    
    return {
        .msg = "evaluated",
        .status = Orgki::Command::Status::OK
    };
}

Context::Context(ContextInitSettings settings) :
    luaState(nullptr), settingsMgr(settings.settingsPath), parser(this, settings.addBuiltInCmds) {
    if (settings.initLua) {
        luaState = luaL_newstate();
        if (settings.luaOpenLibs)
            luaL_openlibs(luaState);
    }

    if (settings.addBuiltInCmds) {
        settingsMgr._AddCommands(parser);
        planMgr._AddCommands(parser);
        if (settings.initLua) {
            parser.AddCommandBulk({
                {
                    .command = "lua",
                    .description = "Evaluates the next argument as Lua",
                    .usage = "[LUA_CODE]",
                    .minArgs = 1,
                    .maxArgs = 1,
                    .callback = &Context::_BuiltinLuaCallback
                },
                {
                    .command = "luaSource",
                    .description = "Evaluates or sources the given Lua file",
                    .usage = "[FILE]",
                    .minArgs = 1,
                    .maxArgs = 1,
                    .callback = &Context::_BuiltinLuaSourceCallback 
                },
            });

            if (settings.loadSettings) {
                if (settings.settingsPath.empty()) {
                    settings.settingsPath = "settings.json";
                }

                settingsMgr.LoadSettings(settings.settingsPath);
            }
        }
    }
}

Context::~Context() {
    lua_close(luaState);
}

}
