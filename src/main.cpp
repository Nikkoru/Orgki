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
#include <functional>
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

void helpCallback(std::vector<std::string>& args);

void createTableCallback(std::vector<std::string>& args);

void seeTablesCallback(std::vector<std::string>& args);

void editTableCallback(std::vector<std::string>& args);

void getTableCallback(std::vector<std::string>& args);

void addActivityToTableCallback(std::vector<std::string>& args);

void createActivityCallback(std::vector<std::string>& args);

void seeActivitiesCallback(std::vector<std::string>& args);

void editActivityCallback(std::vector<std::string>& args);

void getActivityCallback(std::vector<std::string>& args);

void whatOnCallback(std::vector<std::string>& args);

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
            .callback = helpCallback
        },
        {
            .command = "createTable",
            .description = "",
            .usage = "[NAME]",
            .callback = createTableCallback
        },
        {
            .command = "seeTables",
            .description = "",
            .usage = "",
            .callback = seeTablesCallback
        },
        {
            .command = "editTable",
            .description = "",
            .usage = "[ID] [ATTRIBUTE] <VALUE>",
            .callback = editTableCallback
        },
        {
            .command = "getTable",
            .description = "",
            .usage = "[ID]",
            .callback = getTableCallback
        },
        {
            .command = "addActivityToTable",
            .description = "[ACTIVITY_ID] [TABLE_ID] <TIME_RANGE>",
            .usage = "",
            .callback = addActivityToTableCallback
        },
        {
            .command = "seeActivities",
            .description = "",
            .usage = "",
            .callback = seeActivitiesCallback
        },
        {
            .command = "createActivity",
            .description = "",
            .usage = "[NAME] <DESCRIPTION> <\"dynamic\">",
            .callback = createActivityCallback
        },
        {
            .command = "editActivity",
            .description = "",
            .usage = "[ID] [ATTRIBUTE] <VALUE>",
            .callback = editActivityCallback
        },
        {
            .command = "whatOn",
            .description = "WIP",
            .usage = "[TIME / TIME_RANGE] <TABLE>",
            .callback = whatOnCallback
        },
    });

    std::string cmd{};
    while (true) {
        std::println();
        std::println("{}cli stuff{}", ASCII_GRAY_FG, ASCII_RESET);
        std::print("{}>> {}", ASCII_BLUE_FG, ASCII_RESET);
        std::getline(std::cin, cmd);

        if (auto status = parser.Parse(cmd); status != Parser::Status::OK)
            if (status == Parser::Status::REQUEST_EXIT)
                break;
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

void helpCallback(std::vector<std::string>& args);

void createTableCallback(std::vector<std::string>& args) {
    if (args.size() == 1) {
        embededPlan.AddTable(args[0]);
        std::println("{}created table {}\"{}\"", ASCII_GREEN_FG, ASCII_RESET, args[0]);
    }
    else if (args.empty())
        std::println("{}less arguments than expected{}", ASCII_RED_FG, ASCII_RESET);
    else
        std::println("{}more arguments than expected{}", ASCII_RED_FG, ASCII_RESET);
}

void seeTablesCallback(std::vector<std::string>& args) {
    if (!args.empty()) {
        std::println("{}more arguments than expected{}", ASCII_RED_FG, ASCII_RESET);
        return;
    }

    std::println("Tables: {}{}{}", ASCII_MAGENTA_FG, embededPlan.GetTableCount(), ASCII_RESET);
    for (const auto& [id, table] : embededPlan.GetTables()) {
        std::print("{}ID: {}{} ", ASCII_GRAY_FG, id, ASCII_RESET);
        std::println("{}{}{}", ASCII_MAGENTA_FG, table.GetTableName(), ASCII_RESET);
        std::println("  {}{} {}{}", ASCII_CYAN_FG, table.GetActivityCount(), ASCII_RESET, (table.GetActivityCount() == 0 || table.GetActivityCount() > 1) ? "Activities" : "Activity");
    }
}

void editTableCallback(std::vector<std::string>& args) {

}

void getTableCallback(std::vector<std::string>& args) {
    if (args.empty()) {
        std::println("{}less arguments than expected{}", ASCII_RED_FG, ASCII_RESET);
        return;
    }
    else if (args.size() > 1) {
        std::println("{}more arguments than expected{}", ASCII_RED_FG, ASCII_RESET);
        return;
    }

    auto tableID = std::stoi(args[0]);
    if (!embededPlan.TableExists(tableID)) {
        std::println("{}invalid ID:{} {}", ASCII_RED_FG, ASCII_RESET, tableID);
        return;
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
}

void addActivityToTableCallback(std::vector<std::string>& args) {
    if (args.size() > 3) {
        std::println("{}more arguments than expected{}", ASCII_RED_FG, ASCII_RESET);
        return;
    }
    else if (args.empty()) {
        std::println("{}less arguments than expected{}", ASCII_RED_FG, ASCII_RESET);
        return;
    }

    auto actID = std::stoi(args[0]);
    auto tableID = std::stoi(args[1]);

    if (!embededPlan.ActivityExists(actID)) {
        std::println("{}provided ActivityID is not valid:{} {}", ASCII_RED_FG, ASCII_RESET, actID);
        return;
    }
    if (!embededPlan.TableExists(tableID)) {
        std::println("{}provided TableID is not valid:{} {}", ASCII_RED_FG, ASCII_RESET, tableID);
        return;
    }

    const auto& act = embededPlan.GetActivity(actID);
    auto& table = embededPlan.GetTable(tableID);
    TimeRange range{};

    if (!act.IsDynamic() && args.size() < 3) {
        std::println("{}timerange missing{}", ASCII_RED_FG, ASCII_RESET);
        return;
    }
    else if (!act.IsDynamic()) {
        range = Helper::StringToTimeRange(args[2]);
    }

    embededPlan.AddActivityToTable(tableID, actID, range);

    std::println("{}Successfully added activity {}\"{}\" {}(ID : {}){} to table {}\"{}\" {}(ID: {}){}",
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
    );
}

void seeActivitiesCallback(std::vector<std::string>& args) {
    if (!args.empty()) {
        std::println("{}more arguments than expected{}", ASCII_RED_FG, ASCII_RESET);
        return;
    }

    std::println("Activities: {}{}{}", ASCII_MAGENTA_FG, embededPlan.GetActivityCount(), ASCII_RESET);
    for (const auto& [id, act] : embededPlan.GetActivities()) {
        std::print("{}ID: {} ", ASCII_GRAY_FG, id);
        std::println("{}{}{} {}", ASCII_MAGENTA_FG, act.GetActivityName(), ASCII_WHITE_FG, (act.IsDynamic()) ? ASCII_YELLOW_FG + "[DYNAMIC]" + ASCII_WHITE_FG : "");
        std::println("  {}", act.GetActivityDescription().empty() ? "no description" : std::format("\"{}\"", act.GetActivityDescription()), ASCII_RESET);
    }
}

void createActivityCallback(std::vector<std::string>& args) {
    if (args.size() > 3) {
        std::println("{}more arguments than expected{}", ASCII_RED_FG, ASCII_RESET);
        return;
    }
    else if (args.empty()) {
        std::println("{}less arguments than expected{}", ASCII_RED_FG, ASCII_RESET);
        return;
    }

    auto name = args[0];
    std::string desc{};
    bool dynamic = false;

    if (args.size() >= 2)
        desc = args[1];
    if (args.size() == 3)
        dynamic = true;

    embededPlan.CreateActivity(name, desc, dynamic);
    std::println("{}Successfully created Activity {}\"{}\"", ASCII_GREEN_FG, ASCII_RESET, name);
}

void editActivityCallback(std::vector<std::string>& args) {
    if (args.size() > 3) {
        std::println("{}more arguments than expected{}", ASCII_RED_FG, ASCII_RESET);
        return;
    }
    else if (args.empty()) {
        std::println("{}less arguments than expected{}", ASCII_RED_FG, ASCII_RESET);
        return;
    }
    auto actID = std::stoi(args[0]);
    auto attr = args[1];

    if (!embededPlan.ActivityExists(actID)) {
        std::println("{}invalid ID:{} {}", ASCII_RED_FG, ASCII_RESET, actID);
        return;
    }

    auto& act = embededPlan.GetActivity(actID);

    if (attr == "name") {
        if (args.size() == 3) {
            std::println("{}Set Activity {}\"{}\"{} name to {}\"{}\"", ASCII_GREEN_FG, ASCII_RESET, act.GetActivityName(), ASCII_GREEN_FG, ASCII_RESET, args[2]);
            act.SetActivityName(args[2]);
        }
        else {
            std::println("{}missing value{}", ASCII_RED_FG, ASCII_RESET);
            return;
        }
    }
    else if (std::string{"description"}.substr(0, attr.size()) == attr) {
        if (args.size() == 3) {
            act.SetActivityDescription(args[2]);
            std::println("{}Set Activity {}\"{}\"{} description to {}\"{}\"", ASCII_GREEN_FG, ASCII_RESET, act.GetActivityName(), ASCII_GREEN_FG, ASCII_RESET, args[2]);
        }
        else {
            std::println("{}missing value{}", ASCII_RED_FG, ASCII_RESET);
            return;
        }
    }
    else if (attr == "dynamic") {
        if (args.size() >= 3) {
            std::println("{}more arguments than expected{}", ASCII_RED_FG, ASCII_RESET);
            return;
        }
        
        act.SetDynamic(true);
        std::println("{}Set Activity {}\"{}\"{} to {}Dynamic{}", ASCII_GREEN_FG, ASCII_RESET, act.GetActivityName(), ASCII_GREEN_FG, ASCII_YELLOW_FG, ASCII_RESET);
    }
    else if (attr == "noDynamic") {
        if (args.size() >= 3) {
            std::println("{}more arguments than expected{}", ASCII_RED_FG, ASCII_RESET);
            return;
        }
        
        act.SetDynamic(false);
        std::println("{}Set Activity {}\"{}\"{} to {}Not Dynamic{}", ASCII_GREEN_FG, ASCII_RESET, act.GetActivityName(), ASCII_GREEN_FG, ASCII_RED_FG, ASCII_RESET);
    }
    else {
        std::println("{}attribute {}\"{}\"{} is not valid{}", ASCII_RED_FG, ASCII_RESET, attr, ASCII_RED_FG, ASCII_RESET);
    }
}

void getActivityCallback(std::vector<std::string>& args) {

}

void whatOnCallback(std::vector<std::string>& args) {

}

void helpCallback(std::vector<std::string>& args) {
    if (args.size() > 1) {
        std::println("{}more arguments than expected{}", ASCII_RED_FG, ASCII_RESET);
    }

    for (const auto& cmd : parser.GetCommands()) {
        if (args.size() == 1) {
            if (cmd.command == args[0]) {
                std::println("  {}", cmd.command);
                std::println("      {}", cmd.description.empty() ? "no desc" : cmd.description);
                std::println("  USAGE:");
                std::println("      {} {}", cmd.command, cmd.usage);
                return;
            }
        }
        else {
            std::println("{}{} \x1b[3m{}{}{}", ASCII_MAGENTA_FG, cmd.command, ASCII_GRAY_FG, cmd.usage, ASCII_RESET);
        }
    }
}
