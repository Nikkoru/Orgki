#pragma once

#include "Command.hpp"
#include <cstdint>
#include <expected>
#include <optional>
#include <string>
#include <vector>
#include <map>

namespace Orgki {
class Context;
class Parser {
private:
    int m_HistoryPos{};
    int m_MatchesPos{};
    std::vector<Command> m_FoundMatches{};
    std::map<std::string, Command> m_Cmds;
    std::vector<std::string> m_History;
    Context* m_ParentContext = nullptr;
public:
    enum class Status : uint8_t {
        OK = 0,

        REQUEST_EXIT,

        ILL_FORMAT,
        NO_COMMAND_FOUND,
        BUFFER_EMPTY,
        TOO_MANY_ARGS,
        TOO_LITTLE_ARGS,

        COMMAND_ALREADY_EXISTS,
        COMMAND_MISSING_CALLBACK
    };
private:
    Command::StatusData _BuiltinHelpCallback(Context* ctx, std::vector<std::string>& args);
public:
    Parser(Context* ctx, bool builtInCmds = true);

    static std::string StatusToString(Status s);
    static std::string StatusToString(Command::Status s);

    std::expected<Command::StatusData, Status> Parse(std::string cmd);
    Status AddCommand(Command cmd);
    std::pair<Status, std::string> AddCommandBulk(std::vector<Command> cmds);

    bool HasActiveMatch();
    void FindMatches(const std::string& prefix);
    void ClearMatches();
    std::optional<Command> GetNextMatch();
    std::optional<Command> GetPreviousMatch();

	std::string GetNextHistory();
	std::string GetPreviousHistory();

    std::vector<Command> GetCommands();
    std::expected<Command, Status> GetCommand(const std::string& cmd);
};
}
