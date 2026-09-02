#include "Orgki/Helpers/Time.hpp"
#include "libOrgki/Commands/Command.hpp"
#include "libOrgki/Commands/Parser.hpp"
#include "libOrgki/Plan.hpp"
#include "libOrgki/Time/TimeRange.hpp"
#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/component/component_options.hpp>
#include <ftxui/component/event.hpp>
#include <ftxui/component/loop.hpp>
#include <ftxui/dom/elements.hpp>
#include <iostream>
#include <print>
#include <string>
#include <vector>
#include <ftxui/ftxui.hpp>

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

Command::StatusData helpCallback(std::vector<std::string>& args);

Command::StatusData createTableCallback(std::vector<std::string>& args);

Command::StatusData seeTablesCallback(std::vector<std::string>& args);

Command::StatusData editTableCallback(std::vector<std::string>& args);

Command::StatusData getTableCallback(std::vector<std::string>& args);

Command::StatusData addActivityToTableCallback(std::vector<std::string>& args);

Command::StatusData createActivityCallback(std::vector<std::string>& args);

Command::StatusData seeActivitiesCallback(std::vector<std::string>& args);

Command::StatusData editActivityCallback(std::vector<std::string>& args);

Command::StatusData getActivityCallback(std::vector<std::string>& args);

Command::StatusData whatOnCallback(std::vector<std::string>& args);

Plan embededPlan{};
Parser parser{};

int main(int argc, char** argv) {
    // QApplication app{argc, argv};
    // MainWindow win{};
    // std::println("Orgki or smth");
    //
    // win.show();
    //
    // return app.exec();
    
    // auto screen = ftxui::App::Fullscreen();

#if true
    std::println("lost a week bc of mistakenly putting discard in the commit instead of reset");
    
    parser.AddCommandBulk({
        {
            .command = "help",
            .description = "Shows this or details about the provided command",
            .usage = "<CMD>",
            .minArgs = 0,
            .maxArgs = 1,
            .callback = helpCallback
        },
        {
            .command = "createTable",
            .description = "",
            .usage = "[NAME]",
            .minArgs = 1,
            .maxArgs = 1,
            .callback = createTableCallback
        },
        {
            .command = "seeTables",
            .description = "",
            .usage = "",
            .minArgs = 0,
            .maxArgs = 0,
            .callback = seeTablesCallback
        },
        {
            .command = "editTable",
            .description = "",
            .usage = "[ID] [ATTRIBUTE] <VALUE>",
            .minArgs = 2,
            .maxArgs = 3,
            .callback = editTableCallback
        },
        {
            .command = "getTable",
            .description = "",
            .usage = "[ID]",
            .minArgs = 1,
            .maxArgs = 1,
            .callback = getTableCallback
        },
        {
            .command = "addActivityToTable",
            .description = "[ACTIVITY_ID] [TABLE_ID] <TIME_RANGE>",
            .usage = "",
            .minArgs = 2,
            .maxArgs = 3,
            .callback = addActivityToTableCallback
        },
        {
            .command = "seeActivities",
            .description = "",
            .usage = "",
            .minArgs = 0,
            .maxArgs = 0,
            .callback = seeActivitiesCallback
        },
        {
            .command = "createActivity",
            .description = "",
            .usage = "[NAME] <DESCRIPTION> <\"dynamic\">",
            .minArgs = 1,
            .maxArgs = 3,
            .callback = createActivityCallback
        },
        {
            .command = "editActivity",
            .description = "",
            .usage = "[ID] [ATTRIBUTE] <VALUE>",
            .minArgs = 2,
            .maxArgs = 3,
            .callback = editActivityCallback
        },
        {
            .command = "whatOn",
            .description = "WIP",
            .usage = "[TIME / TIME_RANGE] <TABLE>",
            .minArgs = 1,
            .maxArgs = 2,
            .callback = whatOnCallback
        },
    });

    std::string cmd{};
    while (true) {
        std::println();
        std::println("{}cli stuff{}", ASCII_GRAY_FG, ASCII_RESET);
        std::print("{}>> {}", ASCII_BLUE_FG, ASCII_RESET);
        std::getline(std::cin, cmd);

        if (auto status = parser.Parse(cmd); !status.has_value()) {
            std::println("Parser returned: {}", Parser::StatusToString(status.error()));
            if (status.error() == Parser::Status::REQUEST_EXIT)
                break;
            else if (status.error() == Parser::Status::NO_COMMAND_FOUND) {
                std::println("{} command not found{}", ASCII_RED_FG, ASCII_RESET);
            }
        }
        else {
            std::println("Command returned: {}", Parser::StatusToString(status.value().status));
            std::println("Message: {}", status.value().msg);
        }
    }
#else
    std::string cmd{};

    auto option = ftxui::InputOption{};
    option.multiline = false;
    option.on_enter = [&] () {
        if (parseCommand(cmd))
            screen.Exit();
        else
            cmd.erase();
    };

    bool showCmd = false;

    auto input = ftxui::Input(&cmd, "", option);
    auto inputRenderer = ftxui::Renderer(input, [&]() {
        return ftxui::hbox({
            ftxui::text(">> "),
            input->Render(),
        });
    }) | ftxui::Maybe(&showCmd);
    auto button = ftxui::Renderer([&]() {
            return ftxui::text("Exit");
    });
    button |= ftxui::CatchEvent([&](ftxui::Event event) -> bool {
        if (
            event == ftxui::Event::Return || 
            (event.mouse().button == ftxui::Mouse::Left && button->Active())
        ) {
            screen.Exit();
        }
        return true;
    });
    
    // auto buttonOpt = ftxui::ButtonOption::Simple();
    // auto button = ftxui::Button("Quit", [&] { screen.Exit(); }, buttonOpt) | ftxui::center;

    // ftxui::Components components{ 
    //     input,
    //     button
   // };
    // ftxui::Elements elements{ ftxui::text(">> ") };
    //
    // auto createDocument = [&] () -> ftxui::Element {
    //
    //     return ftxui::hbox(elements);
    // };
    //
    auto layout = ftxui::Container::Vertical({
        ftxui::Container::Horizontal({
            inputRenderer,
        }),
        button
    });

    auto mainComponent = ftxui::Renderer(layout, [&]() { 
        return ftxui::vbox({
            inputRenderer->Render(),
            button->Render()
        }); 
    });

    mainComponent |= ftxui::CatchEvent([&](ftxui::Event event) -> bool {
        if (event == ftxui::Event::Character('c') && !input->Focused()) {
            showCmd = !showCmd;
            return true;
        }
        return false;
    });

    // auto updateState = [&] () -> ftxui::Component {
    //
    // };

    auto mainRenderer = ftxui::Renderer(mainComponent, [&]() {
        return mainComponent->Render();
    });

    ftxui::Loop loop{&screen, mainRenderer};

    while (!loop.HasQuitted()) {
        // mainComponent = updateState();
        // screen.RequestAnimationFrame();
        loop.RunOnce();
    }
#endif
}

Command::StatusData helpCallback(std::vector<std::string>& args);

Command::StatusData createTableCallback(std::vector<std::string>& args) {
    embededPlan.AddTable(args[0]);
    return {
        .msg = std::format("{}created table {}\"{}\"", ASCII_GREEN_FG, ASCII_RESET, args[0]),
        .status = Command::Status::OK
    };

}

Command::StatusData seeTablesCallback(std::vector<std::string>& args) {
    std::println("Tables: {}{}{}", ASCII_MAGENTA_FG, embededPlan.GetTableCount(), ASCII_RESET);
    for (const auto& [id, table] : embededPlan.GetTables()) {
        std::print("{}ID: {}{} ", ASCII_GRAY_FG, id, ASCII_RESET);
        std::println("{}{}{}", ASCII_MAGENTA_FG, table.GetTableName(), ASCII_RESET);
        std::println("  {}{} {}{}", ASCII_CYAN_FG, table.GetActivityCount(), ASCII_RESET, (table.GetActivityCount() == 0 || table.GetActivityCount() > 1) ? "Activities" : "Activity");
    }
    return {
        .msg = "",
        .status = Command::Status::OK
    };
}

Command::StatusData editTableCallback(std::vector<std::string>& args) {
    return {
        .msg = "Not implemented",
        .status = Command::Status::FAILED
    };
}

Command::StatusData getTableCallback(std::vector<std::string>& args) {
    auto tableID = std::stoi(args[0]);
    if (!embededPlan.TableExists(tableID)) {
        std::println();
        return {
            .msg = std::format("{}invalid ID:{} {}", ASCII_RED_FG, ASCII_RESET, tableID),
            .status = Command::Status::INVALID_ARGS 
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
        .status = Command::Status::OK
    };
}

Command::StatusData addActivityToTableCallback(std::vector<std::string>& args) {
    auto actID = std::stoi(args[0]);
    auto tableID = std::stoi(args[1]);

    if (!embededPlan.ActivityExists(actID)) {
        return {
            .msg = std::format("{}provided ActivityID is not valid:{} {}", ASCII_RED_FG, ASCII_RESET, actID),
            .status = Command::Status::INVALID_ARGS
        };
    }
    if (!embededPlan.TableExists(tableID)) {
        return {
            .msg = std::format("{}provided TableID is not valid:{} {}", ASCII_RED_FG, ASCII_RESET, tableID),
            .status = Command::Status::INVALID_ARGS
        };
    }

    const auto& act = embededPlan.GetActivity(actID);
    auto& table = embededPlan.GetTable(tableID);
    TimeRange range{};

    if (!act.IsDynamic() && args.size() < 3) {
        return {
            .msg = std::format("{}timerange missing{}", ASCII_RED_FG, ASCII_RESET),
            .status = Command::Status::MISSING_ARGS
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
    .status = Command::Status::OK
    };
}

Command::StatusData seeActivitiesCallback(std::vector<std::string>& args) {
    std::println("Activities: {}{}{}", ASCII_MAGENTA_FG, embededPlan.GetActivityCount(), ASCII_RESET);
    for (const auto& [id, act] : embededPlan.GetActivities()) {
        std::print("{}ID: {} ", ASCII_GRAY_FG, id);
        std::println("{}{}{} {}", ASCII_MAGENTA_FG, act.GetActivityName(), ASCII_WHITE_FG, (act.IsDynamic()) ? ASCII_YELLOW_FG + "[DYNAMIC]" + ASCII_WHITE_FG : "");
        std::println("  {}", act.GetActivityDescription().empty() ? "no description" : std::format("\"{}\"", act.GetActivityDescription()), ASCII_RESET);
    }

    return {
        .msg = "",
        .status = Command::Status::OK
    };
}

Command::StatusData createActivityCallback(std::vector<std::string>& args) {
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
        .status = Command::Status::OK
    };
}

Command::StatusData editActivityCallback(std::vector<std::string>& args) {
    auto actID = std::stoi(args[0]);
    auto attr = args[1];

    if (!embededPlan.ActivityExists(actID)) {
        return {
            .msg = std::format("{}invalid ID:{} {}", ASCII_RED_FG, ASCII_RESET, actID),
            .status = Command::Status::INVALID_ARGS
        };
    }

    auto& act = embededPlan.GetActivity(actID);

    if (attr == "name") {
        if (args.size() == 3) {
            act.SetActivityName(args[2]);
            return {
                .msg = std::format("{}Set Activity {}\"{}\"{} name to {}\"{}\"", ASCII_GREEN_FG, ASCII_RESET, act.GetActivityName(), ASCII_GREEN_FG, ASCII_RESET, args[2]),
                .status = Command::Status::OK
            };
        }
        else {
            return {
                .msg = std::format("{}missing value{}", ASCII_RED_FG, ASCII_RESET),
                .status = Command::Status::MISSING_ARGS
            };
        }
    }
    else if (std::string{"description"}.substr(0, attr.size()) == attr) {
        if (args.size() == 3) {
            act.SetActivityDescription(args[2]);
            return {
                .msg = std::format("{}Set Activity {}\"{}\"{} description to {}\"{}\"", ASCII_GREEN_FG, ASCII_RESET, act.GetActivityName(), ASCII_GREEN_FG, ASCII_RESET, args[2]),
                .status = Command::Status::OK
            };
        }
        else {
            return {
                .msg = std::format("{}missing value{}", ASCII_RED_FG, ASCII_RESET),
                .status = Command::Status::OK
            };
        }
    }
    else if (attr == "dynamic") {
        if (args.size() >= 3) {
            return {
                .msg = std::format("{}more arguments than expected{}", ASCII_RED_FG, ASCII_RESET),
                .status = Command::Status::TOO_MANY_ARGS
            };
        }
        
        act.SetDynamic(true);
        return {
            .msg = std::format("{}Set Activity {}\"{}\"{} to {}Dynamic{}", ASCII_GREEN_FG, ASCII_RESET, act.GetActivityName(), ASCII_GREEN_FG, ASCII_YELLOW_FG, ASCII_RESET),
            .status = Command::Status::OK
        };
    }
    else if (attr == "noDynamic") {
        if (args.size() >= 3) {
            return {
                .msg = std::format("{}more arguments than expected{}", ASCII_RED_FG, ASCII_RESET),
                .status = Command::Status::TOO_MANY_ARGS
            };
        }
        
        act.SetDynamic(false);
        return {
            .msg = std::format("{}Set Activity {}\"{}\"{} to {}Not Dynamic{}", ASCII_GREEN_FG, ASCII_RESET, act.GetActivityName(), ASCII_GREEN_FG, ASCII_RED_FG, ASCII_RESET),
            .status = Command::Status::OK
        };
    }
    else {
        return {
            .msg = std::format("{}attribute {}\"{}\"{} is not valid{}", ASCII_RED_FG, ASCII_RESET, attr, ASCII_RED_FG, ASCII_RESET),
            .status = Command::Status::INVALID_ARGS
        };
    }
}

Command::StatusData getActivityCallback(std::vector<std::string>& args) {
    return {
        .msg = "Not implemented",
        .status = Command::Status::FAILED
    };
}

Command::StatusData whatOnCallback(std::vector<std::string>& args) {
    return {
        .msg = "Not implemented",
        .status = Command::Status::FAILED
    };
}

Command::StatusData helpCallback(std::vector<std::string>& args) {
    for (const auto& cmd : parser.GetCommands()) {
        if (args.size() == 1) {
            if (cmd.command == args[0]) {
                std::println("  {}", cmd.command);
                std::println("      {}", cmd.description.empty() ? "no desc" : cmd.description);
                std::println("  USAGE:");
                std::println("      {} {}", cmd.command, cmd.usage);
                return { 
                    .msg = "",
                    .status = Command::Status::OK
                };
            }
        }
        else {
            std::println("{}{} \x1b[3m{}{}{}", ASCII_MAGENTA_FG, cmd.command, ASCII_GRAY_FG, cmd.usage, ASCII_RESET);
        }
    }
    return { 
        .msg = "",
        .status = Command::Status::OK
    };
}
