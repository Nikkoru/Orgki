#include "Commands/Parser.hpp"
#include "Commands/Command.hpp"
#include "Logger.hpp"
#include "Context.hpp"

#include <functional>
#include <ranges>
#include <string_view>
#include <vector>

namespace Orgki {
Command::StatusData Parser::_BuiltinHelpCallback(Context* ctx, std::vector<std::string>& args) {
    const std::string ASCII_MAGENTA_FG  { "\x1b[35m" };
    const std::string ASCII_GRAY_FG     { "\x1b[38;5;238m" };
    const std::string ASCII_RESET       { "\x1b[0m" };

        for (const auto& cmd : GetCommands()) {
            if (args.size() == 1) {
                if (cmd.command == args[0]) {
                    ctx->PrintLnToBuffer("  {}", cmd.command);
                    ctx->PrintLnToBuffer("      {}", cmd.description.empty() ? "no desc" : cmd.description);
                    ctx->PrintLnToBuffer("  USAGE:");
                    ctx->PrintLnToBuffer("      {} {}", cmd.command, cmd.usage);
                    return { 
                        .msg = ctx->buffer.str(),
                        .status = Orgki::Command::Status::OK
                    };
                }
            }
            else {
                ctx->PrintLnToBuffer("{} {}", cmd.command, cmd.usage);
            }
        }
        return { 
            .msg = ctx->buffer.str(),
            .status = Orgki::Command::Status::OK
        };
}

Parser::Parser(Context* ctx, bool builtinCmd) : m_ParentContext(ctx) {

    // auto helpCallback = [&](std::vector<std::string>& args) -> Command::StatusData {
    //     for (const auto& cmd : GetCommands()) {
    //         if (args.size() == 1) {
    //             if (cmd.command == args[0]) {
    //                 std::println("  {}", cmd.command);
    //                 std::println("      {}", cmd.description.empty() ? "no desc" : cmd.description);
    //                 std::println("  USAGE:");
    //                 std::println("      {} {}", cmd.command, cmd.usage);
    //                 return { 
    //                     .msg = "",
    //                     .status = Orgki::Command::Status::OK
    //                 };
    //             }
    //         }
    //         else {
    //             std::println("{}{} \x1b[3m{}{}{}", ASCII_MAGENTA_FG, cmd.command, ASCII_GRAY_FG, cmd.usage, ASCII_RESET);
    //         }
    //     }
    //     return { 
    //         .msg = "",
    //         .status = Orgki::Command::Status::OK
    //     };
    // };

    if (builtinCmd) {
        auto helpCallback = [this](Context* ctx, std::vector<std::string> args) -> Command::StatusData {
            return _BuiltinHelpCallback(ctx, args);
        };

        AddCommand({
            .command = "help",
            .description = "Shows this screen or details about the provided command",
            .usage = "<CMD>",
            .minArgs = 0,
            .maxArgs = 1,
            // .callback = Command::Callback{ std::bind(&Parser::_BuiltinHelpCallback, this, std::placeholders::_1) }
            .callback = helpCallback
        });
    }
}

std::string Parser::StatusToString(Status s) {
    switch (s) {
    default:
        return "Unknown";
    case Status::REQUEST_EXIT:
        return "REQUEST_EXIT";
    case Status::ILL_FORMAT:
        return "ILL_FORMAT";
    case Status::OK:
        return "OK";
    case Status::NO_COMMAND_FOUND:
        return "NO_COMMAND_FOUND";
    case Status::BUFFER_EMPTY:
        return "BUFFER_EMPTY";
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
std::string Parser::StatusToString(Command::Status s) {
    switch (s) {
    default:
        return "Unknown";
    case Command::Status::OK:
        return "OK";
    case Command::Status::INVALID_ARGS:
        return "INVALID_ARGS";
    case Command::Status::FAILED:
        return "FAILED";
    case Command::Status::MISSING_ARGS:
        return "MISSING_ARGS";
    case Command::Status::TOO_MANY_ARGS:
        return "TOO_MANY_ARGS";
    }
}

std::expected<Command::StatusData, Parser::Status> Parser::Parse(std::string cmd) {
    if (cmd.empty()) return std::unexpected(Status::BUFFER_EMPTY);
    m_History.emplace(m_History.begin(), cmd);
    m_HistoryPos = 0;

    auto tokens = cmd | std::views::split(' ');
    std::string activeCmd{};
    std::vector<std::string> args{};

    bool waitForPair = false;
    char searchForChar{};
    std::string pair{};

    for (const auto& token : tokens) {
        auto str = std::string_view{ token };
        if (str.empty()) continue;

        if (activeCmd.empty()) {
            if (str.contains('\"') || str.contains('\'') || str.contains('&')) {
                Logger::AddError(typeid(Parser), "Command not found: {},", str);
                return std::unexpected(Status::ILL_FORMAT);
            }
            else 
                activeCmd = str;
            if (activeCmd == "exit")
                return std::unexpected(Status::REQUEST_EXIT);
            else if (!m_Cmds.contains(activeCmd))
                return std::unexpected(Status::NO_COMMAND_FOUND);
        }
        else if (str == "&") {
            m_Cmds.at(activeCmd).callback(m_ParentContext, args);
            m_ParentContext->buffer.str("");
            m_ParentContext->buffer.clear();
            args.clear();
            activeCmd.erase();
        }
        else {
            if (str[0] == ('\"') || str[0] == ('\'')) {
                if (waitForPair) {
                    if (str.contains(searchForChar))
						return std::unexpected(Status::ILL_FORMAT);

					using namespace std::string_literals;
					pair += " "s += str;
					continue;
                }
                else if (!waitForPair) {
                    if (str.back() == '\"' || str.back() == '\'') {
						using namespace std::string_literals;
						auto view = std::string_view{ str };
						view.remove_prefix(1);
						view.remove_suffix(1);
						args.emplace_back(view);
                        continue;
                    }
                    pair += str;
                    searchForChar = str[0];
                    waitForPair = true;
                    continue;
                }
                else {
                    using namespace std::string_literals;
                    pair += " "s += str;
                    auto view = std::string_view{ pair };
                    view.remove_prefix(1);
                    view.remove_suffix(1);
                    args.emplace_back(view);

                    pair.erase();
                    waitForPair = false;
                    continue;
                }
            }
            else if (str.back() == '\"' || str.back() == '\'') {
                if (!waitForPair) return std::unexpected(Status::ILL_FORMAT);
                else if (waitForPair && str.back() == searchForChar) {
                    using namespace std::string_literals;
                    pair += " "s += str;
                    auto view = std::string_view{ pair };
                    view.remove_prefix(1);
                    view.remove_suffix(1);
                    args.emplace_back(view);

                    pair.erase();
                    waitForPair = false;
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

    if (!pair.empty())
        return std::unexpected(Status::ILL_FORMAT);

    if (args.size() > m_Cmds.at(activeCmd).maxArgs)
        return std::unexpected(Status::TOO_MANY_ARGS);
    if (args.size() < m_Cmds.at(activeCmd).minArgs)
        return std::unexpected(Status::TOO_LITTLE_ARGS);

    auto status = m_Cmds.at(activeCmd).callback(m_ParentContext, args);
    m_ParentContext->buffer.str("");
    m_ParentContext->buffer.clear();
    return status;
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

bool Parser::HasActiveMatch() {
    return !m_FoundMatches.empty();
}

void Parser::FindMatches(const std::string& prefix) {
    m_FoundMatches.clear();
    m_MatchesPos = 0;

    for (const auto& [name, cmd] : m_Cmds) {
        if (name.starts_with(prefix))
            m_FoundMatches.emplace_back(cmd);
        else continue;
    }
}

void Parser::ClearMatches() {
    m_FoundMatches.clear();
}

std::expected<Command, Parser::Status> Parser::GetNextMatch() {
    if (m_FoundMatches.empty()) return std::unexpected(Status::NO_COMMAND_FOUND);

    if (m_MatchesPos == -1) m_MatchesPos += 2;
    if (m_MatchesPos >= m_FoundMatches.size()) return std::unexpected(Status::NO_COMMAND_FOUND);
    
    return m_FoundMatches.at(m_MatchesPos++);
}

std::expected<Command, Parser::Status> Parser::GetPreviousMatch() {
    if (m_FoundMatches.empty()) return std::unexpected(Status::NO_COMMAND_FOUND);

    if (m_MatchesPos == m_FoundMatches.size()) m_MatchesPos -= 2;
    if (m_MatchesPos < 0) return std::unexpected(Status::NO_COMMAND_FOUND);
    return m_FoundMatches.at(m_MatchesPos--);
}

std::string Parser::GetNextHistory() {
    if (m_History.empty()) return {};

    if (m_HistoryPos == -1) m_HistoryPos++;
    if (m_HistoryPos >= m_History.size()) return {};
    return m_History.at(m_HistoryPos++);
}

std::string Parser::GetPreviousHistory() {
    if (m_History.empty()) return {};

    if (m_HistoryPos == m_History.size()) m_HistoryPos--;
    if (m_HistoryPos < 0) return {};
    return m_History.at(m_HistoryPos--);
}

std::vector<Command> Parser::GetCommands() {
    std::vector<Command> vec{};

    for (const auto& [name, cmd] : m_Cmds)
        vec.emplace_back(cmd);

    return vec;
}

std::expected<Command, Parser::Status> Parser::GetCommand(const std::string& cmd) {
    if (m_Cmds.contains(cmd)) return m_Cmds.at(cmd);
    else return std::unexpected(Status::NO_COMMAND_FOUND);
}
}
