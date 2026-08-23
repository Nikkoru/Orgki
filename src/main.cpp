#include "Orgki/Helpers/Time.hpp"
#include "libOrgki/Plan.hpp"
#include "libOrgki/Time/TimeRange.hpp"
#include <functional>
#include <iostream>
#include <print>
#include <string>
#include <vector>
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

bool parseCommand(std::string cmd);

Plan embededPlan{};

int main(int argc, char** argv) {
    // QApplication app{argc, argv};
    // MainWindow win{};
    // std::println("Orgki or smth");
    //
    // win.show();
    //
    // return app.exec();
    
    std::println("lost a week bc of mistakenly putting discard in the commit instead of reset");

    std::string cmd{};
    while (true) {
        std::println();
        std::println("{}cli stuff{}", ASCII_GRAY_FG, ASCII_RESET);
        std::print("{}>> {}", ASCII_BLUE_FG, ASCII_RESET);
        std::getline(std::cin, cmd);
        if (parseCommand(cmd))
            break;
    }
}

struct CMD {
    std::string cmd{};
    std::string desc{};
    std::string usage{};
    std::function<void(std::vector<std::string>&)> callback;
};

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


std::vector<CMD> cmds {
    CMD {
        .cmd = "help",
        .desc = "Shows this or details about the provided command",
        .usage = "<CMD>",
        .callback = helpCallback
    },
    CMD {
        .cmd = "createTable",
        .desc = "",
        .usage = "[NAME]",
        .callback = createTableCallback
    },
    CMD {
        .cmd = "seeTables",
        .desc = "",
        .usage = "",
        .callback = seeTablesCallback
    },
    CMD {
        .cmd = "editTable",
        .desc = "",
        .usage = "[ID] [ATTRIBUTE] <VALUE>",
        .callback = editTableCallback
    },
    CMD {
        .cmd = "getTable",
        .desc = "",
        .usage = "[ID]",
        .callback = getTableCallback
    },
    CMD {
        .cmd = "addActivityToTable",
        .desc = "[ACTIVITY_ID] [TABLE_ID] <TIME_RANGE>",
        .usage = "",
        .callback = addActivityToTableCallback
    },
    CMD {
        .cmd = "seeActivities",
        .desc = "",
        .usage = "",
        .callback = seeActivitiesCallback
    },
    CMD {
        .cmd = "createActivity",
        .desc = "",
        .usage = "[NAME] <DESCRIPTION> <\"dynamic\">",
        .callback = createActivityCallback
    },
    CMD {
        .cmd = "editActivity",
        .desc = "",
        .usage = "[ID] [ATTRIBUTE] <VALUE>",
        .callback = editActivityCallback
    },
    CMD {
        .cmd = "whatOn",
        .desc = "WIP",
        .usage = "[TIME / TIME_RANGE] <TABLE>",
        .callback = whatOnCallback
    },
};

void helpCallback(std::vector<std::string>& args) {
    if (args.size() > 1) {
        std::println("{}more arguments than expected{}", ASCII_RED_FG, ASCII_RESET);
    }

    for (const auto& cmd : cmds) {
        if (args.size() == 1) {
            if (cmd.cmd == args[0]) {
                std::println("  {}", cmd.cmd);
                std::println("      {}", cmd.desc.empty() ? "no desc" : cmd.desc);
                std::println("  USAGE:");
                std::println("      {} {}", cmd.cmd, cmd.usage);
                return;
            }
        }
        else {
            std::println("{}{} \x1b[3m{}{}{}", ASCII_MAGENTA_FG, cmd.cmd, ASCII_GRAY_FG, cmd.usage, ASCII_RESET);
        }
    }
}


bool parseCommand(std::string cmd) {
    if (cmd == "exit")
        return true;

    std::string action{};
    std::vector<std::string> args;
    std::function<void(std::vector<std::string>&)> callback;

    while (!cmd.empty()) {
        std::string splitCmd{};

        if (cmd[0] == '\"') {
            if (action.empty()) {
                std::println("{}the command shouldn't start with {}'\"'", ASCII_RED_FG, ASCII_RESET);
                return false;
            }
            else {
                splitCmd = cmd.substr(1, cmd.find('\"', 1) - 1);
                cmd.erase(0, cmd.find('\"', 1) + 2);
            }
        }
        else {
            splitCmd = cmd.substr(0, cmd.find(' '));
            if (!cmd.contains(' '))
                cmd.erase();
            else
				cmd.erase(0, cmd.find(' ') + 1);
        }

        if (action.empty())
            action = splitCmd;
        else
            args.emplace_back(splitCmd);
    }
    // std::println("{}{}{}", ASCII_MAGENTA_FG, action, ASCII_RESET);
    // for (const auto& arg : args) {
    //     std::println("{}{}{}", ASCII_CYAN_FG, arg, ASCII_RESET);
    // }

    for (const auto& cmdAct : cmds) {
        if (cmdAct.cmd == action) {
            callback = cmdAct.callback;
        }
    }

    if (!callback)
        std::println("{}command not found: {}\"{}\"", ASCII_RED_FG, ASCII_RESET, action);
    else
        callback(args);

    std::print("{}", ASCII_RESET);
    return false;
}
