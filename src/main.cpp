#include "Orgki/Helpers/Time.hpp"
#include "libOrgki/Commands/Command.hpp"
#include "libOrgki/Commands/Parser.hpp"
#include "libOrgki/Context.hpp"
#include "libOrgki/Logger.hpp"
#include "libOrgki/Plan.hpp"
#include "libOrgki/Settings/SettingsManager.hpp"
#include "libOrgki/Time/TimeRange.hpp"
#include <filesystem>
#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/component/component_options.hpp>
#include <ftxui/component/event.hpp>
#include <ftxui/component/loop.hpp>
#include <ftxui/dom/elements.hpp>
#include <iostream>
#include <fstream>
#include <print>
#include <sstream>
#include <string>
#include <vector>
#include <ftxui/ftxui.hpp>

#include <lua.hpp>

// #include <QApplication>
//
// #include "Orgki/MainWindow.hpp"

const std::string ASCII_RESET       { "\x1b[0m" };

const std::string ASCII_BLACK_FG    { "\x1b[30m" };
const std::string ASCII_RED_FG      { "\x1b[31m" };
const std::string ASCII_GREEN_FG    { "\x1b[32m" };
const std::string ASCII_YELLOW_FG   { "\x1b[33m" };
const std::string ASCII_BLUE_FG     { "\x1b[34m" };
const std::string ASCII_MAGENTA_FG  { "\x1b[35m" };
const std::string ASCII_CYAN_FG     { "\x1b[36m" };
const std::string ASCII_WHITE_FG    { "\x1b[37m" };
const std::string ASCII_GRAY_FG     { "\x1b[38;5;238m" };

const std::string ASCII_BLACK_BG    { "\x1b[40m" };
const std::string ASCII_RED_BG      { "\x1b[41m" };
const std::string ASCII_GREEN_BG    { "\x1b[42m" };
const std::string ASCII_YELLOW_BG   { "\x1b[43m" };
const std::string ASCII_BLUE_BG     { "\x1b[44m" };
const std::string ASCII_MAGENTA_BG  { "\x1b[45m" };
const std::string ASCII_CYAN_BG     { "\x1b[46m" };
const std::string ASCII_WHITE_BG    { "\x1b[47m" };
const std::string ASCII_GRAY_BG     { "\x1b[48;5;238m" };

Orgki::Command::StatusData helpCallback(std::vector<std::string>& args);

Orgki::Command::StatusData luaCallback(std::vector<std::string>& args);
Orgki::Command::StatusData luaSourceCallback(std::vector<std::string>& args);

Orgki::Command::StatusData createTableCallback(std::vector<std::string>& args);
Orgki::Command::StatusData seeTablesCallback(std::vector<std::string>& args);
Orgki::Command::StatusData editTableCallback(std::vector<std::string>& args);
Orgki::Command::StatusData getTableCallback(std::vector<std::string>& args);

Orgki::Command::StatusData addActivityToTableCallback(std::vector<std::string>& args);

Orgki::Command::StatusData createActivityCallback(std::vector<std::string>& args);
Orgki::Command::StatusData seeActivitiesCallback(std::vector<std::string>& args);
Orgki::Command::StatusData editActivityCallback(std::vector<std::string>& args);
Orgki::Command::StatusData getActivityCallback(std::vector<std::string>& args);

Orgki::Command::StatusData whatOnCallback(std::vector<std::string>& args);

Orgki::Command::StatusData addSettingCallback(std::vector<std::string>& args);
Orgki::Command::StatusData setSettingCallback(std::vector<std::string>& args);
Orgki::Command::StatusData getSettingCallback(std::vector<std::string>& args);
Orgki::Command::StatusData removeSettingCallback(std::vector<std::string>& args);
Orgki::Command::StatusData getAllSettingsCallback(std::vector<std::string>& args);

Orgki::Plan embededPlan{};

int main(int argc, char** argv) {
    auto settings = Orgki::ContextInitSettings{
        .settingsPath = "settings.json"
    };
    auto ctx = Orgki::Context{ settings };
    std::string cmd{};

    std::println("lost a week bc of mistakenly putting discard in the commit instead of reset");
    std::println("current path = {}", std::filesystem::current_path().string());

#ifdef ORGKI_BUILD_CLI
    Logger::GetInstance()->SaveToLogFile("log.txt");
    Logger::GetInstance()->LogToConsole(false);
    // QApplication app{argc, argv};
    // MainWindow win{};
    // std::println("Orgki or smth");
    //
    // win.show();
    //
    // return app.exec();
    
    while (true) {
        std::println();
        std::println("{}cli stuff{}", ASCII_GRAY_FG, ASCII_RESET);
        std::print("{}>> {}", ASCII_BLUE_FG, ASCII_RESET);
        std::getline(std::cin, cmd);

        if (auto status = ctx.parser.Parse(cmd); !status.has_value()) {
            // std::println("Parser returned: {}", Orgki::Parser::StatusToString(status.error()));
            if (status.error() == Orgki::Parser::Status::REQUEST_EXIT)
                break;
            // else {
            //     std::println("ERR: {}", Orgki::Parser::StatusToString(status.error()));
            // }
            switch (status.error()) {
            case Orgki::Parser::Status::ILL_FORMAT:
                std::println("{}The command was ill formatted{}", ASCII_RED_FG, ASCII_RESET);
                break;
            case Orgki::Parser::Status::NO_COMMAND_FOUND:
                std::println("{}Command not found{}", ASCII_RED_FG, ASCII_RESET);
                break;
            case Orgki::Parser::Status::TOO_LITTLE_ARGS:
                std::println("{}Command requires more arguments{}", ASCII_RED_FG, ASCII_RESET);
                break;
            case Orgki::Parser::Status::TOO_MANY_ARGS:
                std::println("{}Command requires less arguments{}", ASCII_RED_FG, ASCII_RESET);
                break;
            default:
                std::println("{}Parser failed: {}{}", ASCII_RED_FG, ASCII_RESET, Orgki::Parser::StatusToString(status.error()));
            };
        }
        else {
            // std::println("Command returned: {}", Orgki::Parser::StatusToString(status.value().status));
            // std::println("Message: {}", status.value().msg);
            if (status.value().status != Orgki::Command::Status::OK)
                std::println("ERR {}: {}",  Orgki::Parser::StatusToString(status.value().status), status.value().msg);
            else if (!status.value().msg.empty())
                std::println("{}", status.value().msg);
        }
    }
#else
    auto screen{ ftxui::App::Fullscreen() };
    auto elms = ftxui::Container::Vertical({});

    int depth = 0;
    auto renderers = ftxui::Container::Tab({}, &depth);

    bool showSidePanel = true;
    auto sidePanel = ftxui::Renderer([] {
        return ftxui::vbox({
            ftxui::text("Side Panel"),
            ftxui::separator()
        });
    }) | ftxui::Maybe(&showSidePanel);

    bool showCommandLine = false;
    std::string cmdMsg{};
    auto cmdLineConfig = ftxui::InputOption{
        .multiline = false,
        .on_enter = [&screen, &cmd, &cmdMsg, &ctx] {
            auto status = ctx.parser.Parse(cmd);
            if (!status.has_value()) {
                switch (status.error()) {
                case Orgki::Parser::Status::OK:
                case Orgki::Parser::Status::BUFFER_EMPTY:
                case Orgki::Parser::Status::COMMAND_ALREADY_EXISTS:
                    break;
                case Orgki::Parser::Status::REQUEST_EXIT:
                    screen.Exit();
                    break;
                case Orgki::Parser::Status::ILL_FORMAT:
                    cmdMsg = "Ill formated";
                    break;
                case Orgki::Parser::Status::NO_COMMAND_FOUND:
                    cmdMsg = format("Command \"{}\" was not found", cmd);
                    break;
                case Orgki::Parser::Status::TOO_MANY_ARGS:
                    cmdMsg = "Command expects less args";
                    break;
                case Orgki::Parser::Status::TOO_LITTLE_ARGS:
                    cmdMsg = "Command expects more args";
                    break;
                case Orgki::Parser::Status::COMMAND_MISSING_CALLBACK:
                    cmdMsg = "Command doesn't have a callback to handle this command";
                    break;
                  break;
                }
            }
            cmd.erase();
        }
    };
    auto commandLineInput = ftxui::Input(cmdLineConfig);

    auto mainBody = ftxui::Container::Vertical({
        elms
    });
    auto mainBodyRenderer = ftxui::Renderer(mainBody, [&] {
        return ftxui::vbox({
            ftxui::text("Main Body"),
            ftxui::separator(),
            elms->Render() | ftxui::border | ftxui::frame
        });
    });

    auto globalLayout = ftxui::Container::Horizontal({
        sidePanel,
        mainBodyRenderer
    });

    renderers->Add(ftxui::Renderer(globalLayout, [&] {
        return ftxui::hbox({
            sidePanel->Render(),
            ftxui::separator(),
            globalLayout->Render()
        }) | ftxui::border; 
    }));
    renderers->Add(ftxui::Renderer(commandLineInput, [&] {
        auto elms = ftxui::Elements{ 
            ftxui::filler(), 
            commandLineInput->Render() 
        };

        if (!cmdMsg.empty())
            elms.emplace_back(ftxui::text(cmdMsg));

        return ftxui::vbox(elms);
    }) | ftxui::Maybe(&showCommandLine) | ftxui::CatchEvent([&](ftxui::Event event) -> bool {
        if (event == ftxui::Event::Escape) {
            showCommandLine = false;
            return true;
        }
        return false;
    }));

    auto mainRenderer = ftxui::Renderer(renderers, [&] {
        ftxui::Elements elms{};

        for (size_t i = 0; i < renderers->ChildCount(); ++i) {
            auto renderer = renderers->ChildAt(i);
            elms.emplace_back(renderer->Render());
        }

        return ftxui::dbox({
            elms
        });
    });

    mainRenderer |=  ftxui::CatchEvent([&](ftxui::Event event) -> bool {
        if (event.is_character() && commandLineInput->Active()) return false;

        if (event == ftxui::Event::Character('q')) {
            screen.Exit();
            return true;
        }
        else if (event == ftxui::Event::Character(' ')) {
            showSidePanel = !showSidePanel;
            return true;
        }
        else if (event == ftxui::Event::Character('c')) {
            showCommandLine = !showCommandLine; 
            return true;
        }

        return false;
    });

    screen.Loop(mainRenderer);
#endif
}

Orgki::Command::StatusData createTableCallback(std::vector<std::string>& args) {
    embededPlan.AddTable(args[0]);
    return {
        .msg = std::format("{}created table {}\"{}\"", ASCII_GREEN_FG, ASCII_RESET, args[0]),
        .status = Orgki::Command::Status::OK
    };

}

Orgki::Command::StatusData seeTablesCallback(std::vector<std::string>& args) {
    std::println("Tables: {}{}{}", ASCII_MAGENTA_FG, embededPlan.GetTableCount(), ASCII_RESET);
    for (const auto& [id, table] : embededPlan.GetTables()) {
        std::print("{}ID: {}{} ", ASCII_GRAY_FG, id, ASCII_RESET);
        std::println("{}{}{}", ASCII_MAGENTA_FG, table.GetTableName(), ASCII_RESET);
        std::println("  {}{} {}{}", ASCII_CYAN_FG, table.GetActivityCount(), ASCII_RESET, (table.GetActivityCount() == 0 || table.GetActivityCount() > 1) ? "Activities" : "Activity");
    }
    return {
        .msg = "",
        .status = Orgki::Command::Status::OK
    };
}

Orgki::Command::StatusData editTableCallback(std::vector<std::string>& args) {
    return {
        .msg = "Not implemented",
        .status = Orgki::Command::Status::FAILED
    };
}

Orgki::Command::StatusData getTableCallback(std::vector<std::string>& args) {
    auto tableID = std::stoi(args[0]);
    if (!embededPlan.TableExists(tableID)) {
        std::println();
        return {
            .msg = std::format("{}invalid ID:{} {}", ASCII_RED_FG, ASCII_RESET, tableID),
            .status = Orgki::Command::Status::INVALID_ARGS 
        };
    }

    auto table = embededPlan.GetTable(tableID);

    std::println("{}Table: {}\"{}\"", ASCII_MAGENTA_FG, ASCII_RESET, table.GetTableName());
    std::println("{}{} {}{}", ASCII_BLUE_FG, table.GetActivityCount(), ASCII_RESET, (table.GetActivityCount() > 1 || table.GetActivityCount() == 0) ? "Activities" : "Activity");
    for (auto& [timeRange, actID] : table.GetActivities()) {
        std::print("{}{}{}\t", ASCII_CYAN_FG, Helper::TimeRangeToString(timeRange), ASCII_RESET);
        if (!embededPlan.ActivityExists(actID)) {
            std::println("{}Invalid ID{}", ASCII_RED_BG, ASCII_RESET);
            continue;
        }
        auto act = embededPlan.GetActivity(actID);
        std::print("{}", act.IsDynamic() ? ASCII_YELLOW_BG : ASCII_CYAN_BG);
        std::print("{}ID: {}{}", ASCII_GRAY_FG, actID, ASCII_RESET);
        std::print("{} ", act.IsDynamic() ? ASCII_YELLOW_BG : ASCII_CYAN_BG);
        std::print("{}{} {}-> {}", ASCII_MAGENTA_FG, act.GetActivityName(), ASCII_WHITE_FG, act.GetActivityDescription());
        std::println("{}", ASCII_RESET);
    }

    return {
        .msg = "",
        .status = Orgki::Command::Status::OK
    };
}

Orgki::Command::StatusData addActivityToTableCallback(std::vector<std::string>& args) {
    auto actID = std::stoi(args[0]);
    auto tableID = std::stoi(args[1]);

    if (!embededPlan.ActivityExists(actID)) {
        return {
            .msg = std::format("{}provided ActivityID is not valid:{} {}", ASCII_RED_FG, ASCII_RESET, actID),
            .status = Orgki::Command::Status::INVALID_ARGS
        };
    }
    if (!embededPlan.TableExists(tableID)) {
        return {
            .msg = std::format("{}provided TableID is not valid:{} {}", ASCII_RED_FG, ASCII_RESET, tableID),
            .status = Orgki::Command::Status::INVALID_ARGS
        };
    }

    const auto& act = embededPlan.GetActivity(actID);
    auto& table = embededPlan.GetTable(tableID);
    Orgki::TimeRange range{};

    if (!act.IsDynamic() && args.size() < 3) {
        return {
            .msg = std::format("{}timerange missing{}", ASCII_RED_FG, ASCII_RESET),
            .status = Orgki::Command::Status::MISSING_ARGS
        };
    }
    else if (!act.IsDynamic()) {
        range = Helper::StringToTimeRange(args[2]);
    }

    embededPlan.AddActivityToTable(tableID, actID, range);

    return {
    .msg = std::format("{}Successfully added activity {}\"{}\" {}(ID : {}){} to table {}\"{}\" {}(ID: {}){}",
            ASCII_GREEN_FG, 
            ASCII_RESET, 
            act.GetActivityName(), 
            ASCII_GRAY_FG, 
            actID, 
            ASCII_GREEN_FG, 
            ASCII_RESET, 
            table.GetTableName(),
            ASCII_GRAY_FG,
            tableID,
            ASCII_RESET
        ),
    .status = Orgki::Command::Status::OK
    };
}

Orgki::Command::StatusData seeActivitiesCallback(std::vector<std::string>& args) {
    std::println("Activities: {}{}{}", ASCII_MAGENTA_FG, embededPlan.GetActivityCount(), ASCII_RESET);
    for (const auto& [id, act] : embededPlan.GetActivities()) {
        std::print("{}ID: {} ", ASCII_GRAY_FG, id);
        std::println("{}{}{} {}", ASCII_MAGENTA_FG, act.GetActivityName(), ASCII_WHITE_FG, (act.IsDynamic()) ? ASCII_YELLOW_FG + "[DYNAMIC]" + ASCII_WHITE_FG : "");
        std::println("  {}", act.GetActivityDescription().empty() ? "no description" : std::format("\"{}\"", act.GetActivityDescription()), ASCII_RESET);
    }

    return {
        .msg = "",
        .status = Orgki::Command::Status::OK
    };
}

Orgki::Command::StatusData createActivityCallback(std::vector<std::string>& args) {
    auto name = args[0];
    std::string desc{};
    bool dynamic = false;

    if (args.size() >= 2)
        desc = args[1];
    if (args.size() == 3)
        dynamic = true;

    embededPlan.CreateActivity(name, desc, dynamic);
    return {
        .msg = std::format("{}Successfully created Activity {}\"{}\"", ASCII_GREEN_FG, ASCII_RESET, name),
        .status = Orgki::Command::Status::OK
    };
}

Orgki::Command::StatusData editActivityCallback(std::vector<std::string>& args) {
    auto actID = std::stoi(args[0]);
    auto attr = args[1];

    if (!embededPlan.ActivityExists(actID)) {
        return {
            .msg = std::format("{}invalid ID:{} {}", ASCII_RED_FG, ASCII_RESET, actID),
            .status = Orgki::Command::Status::INVALID_ARGS
        };
    }

    auto& act = embededPlan.GetActivity(actID);

    if (attr == "name") {
        if (args.size() == 3) {
            act.SetActivityName(args[2]);
            return {
                .msg = std::format("{}Set Activity {}\"{}\"{} name to {}\"{}\"", ASCII_GREEN_FG, ASCII_RESET, act.GetActivityName(), ASCII_GREEN_FG, ASCII_RESET, args[2]),
                .status = Orgki::Command::Status::OK
            };
        }
        else {
            return {
                .msg = std::format("{}missing value{}", ASCII_RED_FG, ASCII_RESET),
                .status = Orgki::Command::Status::MISSING_ARGS
            };
        }
    }
    else if (std::string{"description"}.substr(0, attr.size()) == attr) {
        if (args.size() == 3) {
            act.SetActivityDescription(args[2]);
            return {
                .msg = std::format("{}Set Activity {}\"{}\"{} description to {}\"{}\"", ASCII_GREEN_FG, ASCII_RESET, act.GetActivityName(), ASCII_GREEN_FG, ASCII_RESET, args[2]),
                .status = Orgki::Command::Status::OK
            };
        }
        else {
            return {
                .msg = std::format("{}missing value{}", ASCII_RED_FG, ASCII_RESET),
                .status = Orgki::Command::Status::OK
            };
        }
    }
    else if (attr == "dynamic") {
        if (args.size() >= 3) {
            return {
                .msg = std::format("{}more arguments than expected{}", ASCII_RED_FG, ASCII_RESET),
                .status = Orgki::Command::Status::TOO_MANY_ARGS
            };
        }
        
        act.SetDynamic(true);
        return {
            .msg = std::format("{}Set Activity {}\"{}\"{} to {}Dynamic{}", ASCII_GREEN_FG, ASCII_RESET, act.GetActivityName(), ASCII_GREEN_FG, ASCII_YELLOW_FG, ASCII_RESET),
            .status = Orgki::Command::Status::OK
        };
    }
    else if (attr == "noDynamic") {
        if (args.size() >= 3) {
            return {
                .msg = std::format("{}more arguments than expected{}", ASCII_RED_FG, ASCII_RESET),
                .status = Orgki::Command::Status::TOO_MANY_ARGS
            };
        }
        
        act.SetDynamic(false);
        return {
            .msg = std::format("{}Set Activity {}\"{}\"{} to {}Not Dynamic{}", ASCII_GREEN_FG, ASCII_RESET, act.GetActivityName(), ASCII_GREEN_FG, ASCII_RED_FG, ASCII_RESET),
            .status = Orgki::Command::Status::OK
        };
    }
    else {
        return {
            .msg = std::format("{}attribute {}\"{}\"{} is not valid{}", ASCII_RED_FG, ASCII_RESET, attr, ASCII_RED_FG, ASCII_RESET),
            .status = Orgki::Command::Status::INVALID_ARGS
        };
    }
}

Orgki::Command::StatusData getActivityCallback(std::vector<std::string>& args) {
    return {
        .msg = "Not implemented",
        .status = Orgki::Command::Status::FAILED
    };
}

Orgki::Command::StatusData whatOnCallback(std::vector<std::string>& args) {
    return {
        .msg = "Not implemented",
        .status = Orgki::Command::Status::FAILED
    };
}
