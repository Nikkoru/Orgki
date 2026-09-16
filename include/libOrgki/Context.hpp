#pragma once

#include <filesystem>
#include <sstream>

#include <lua.hpp>
#include <string_view>

#include "Commands/Parser.hpp"
#include "Settings/SettingsManager.hpp"
#include "PlanManager.hpp"

namespace Orgki {
struct ContextInitSettings{
    bool initLua{ true };
    bool luaOpenLibs{ true };
    bool loadPlugins{ true };
    bool addBuiltInCmds{ true };
    bool loadSettings{ true };
    std::filesystem::path settingsPath{};
};

class Context {
public:
    lua_State* luaState{};
    SettingsManager settingsMgr{};
    PlanManager planMgr{};
    std::ostringstream buffer;
    Parser parser{ nullptr };
private:
    Command::StatusData _BuiltinLuaCallback(std::vector<std::string>& args);
    Command::StatusData _BuiltinLuaSourceCallback(std::vector<std::string>& args);
public:
    Context() : Context(ContextInitSettings{}) {}
    Context(ContextInitSettings settings);
    ~Context();

    void GetLastStatus();

    template<class ...Args>
    void PrintToBuffer(const std::string_view fmt, Args&&... elms);
    template<class ...Args>
    void PrintLnToBuffer(const std::string_view fmt, Args&&... elms);
};
}

#include "Context.inl"
