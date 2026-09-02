#include "Commands/Parser.hpp"
#include "Commands/Command.hpp"

#include <ranges>
#include <string_view>
#include <vector>

std::string Parser::StatusToString(Status s) {
    switch (s) {
    default:
        return "Unknown";
    case Status::ILL_FORMAT:
        return "ILL_FORMAT";
    case Status::OK:
        return "OK";
    case Status::NO_COMMAND:
        return "NO_COMMAND";
    case Status::TOO_LITTLE_ARGS:
        return "TOO_LITTLE_ARGS";
    case Status::TOO_MANY_ARGS:
        return "TOO_MANY_ARGS";
    case Status::COMMAND_ALREADY_EXISTS:
        return "COMMAND_ALREADY_EXISTS";
    case Status::COMMAND_MISSING_CALLBACK:
        return "COMMAND_MISSING_CALLBACK";
    }
}

Parser::Status Parser::Parse(std::string cmd) {
    if (cmd.empty()) return Status::NO_COMMAND;

    auto tokens = cmd | std::views::split(' ');
    std::string activeCmd{};
    std::vector<std::string> args{};

    bool waitForPair = false;
    std::string pair{};

    for (const auto& token : tokens) {
        auto str = std::string_view{ token };
        if (str == "&") {
            m_Cmds.at(activeCmd).callback(args);
            args.clear();
            activeCmd.erase();
        }
        else if (activeCmd.empty()) {
            if (str.contains('\"')) 
                return Status::ILL_FORMAT;
            else 
                activeCmd = str;
            if (!m_Cmds.contains(activeCmd))
                return Status::NO_COMMAND;
        }
        else {
            if (str.contains('\"')) {
                if (waitForPair) {
                    using namespace std::string_literals;
                    pair += " "s += str;
                    auto view = std::string_view{ pair };
                    view.remove_prefix(1);
                    view.remove_suffix(1);
                    args.emplace_back(view);

                    pair.erase();
                }
                else {
                    pair += str;
                    waitForPair = true;
                    continue;
                }
            }

            if (waitForPair) {
                using namespace std::string_literals;

                pair += " "s += str;
                continue;
            }
            args.emplace_back(str);
        }
    }

    m_Cmds.at(activeCmd).callback(args);

    return Status::OK;
}

Parser::Status Parser::AddCommand(Command cmd) {
    if (!cmd.callback)
        return Status::COMMAND_MISSING_CALLBACK;
    if (m_Cmds.contains(cmd.command))
        return Status::COMMAND_ALREADY_EXISTS;

    m_Cmds[cmd.command] = cmd;

    return Status::OK;
}

std::pair<Parser::Status, std::string> Parser::AddCommandBulk(std::vector<Command> cmds) {
    for (const auto& cmd : cmds) {
        auto status = AddCommand(cmd);
        if (status != Status::OK)
            return { status, cmd.command };
    }

    return { Status::OK, "" };
}

std::vector<Command> Parser::GetCommands() {
    std::vector<Command> vec{};

    for (const auto& [name, cmd] : m_Cmds)
        vec.emplace_back(cmd);

    return vec;
}
