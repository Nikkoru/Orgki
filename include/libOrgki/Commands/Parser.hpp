#pragma once

#include "Command.hpp"
#include <cstdint>
#include <expected>
#include <string>
#include <vector>
#include <map>

class Parser {
private:
    std::map<std::string, Command> m_Cmds;
public:
    enum class Status : uint8_t {
        OK = 0,

        REQUEST_EXIT,

        ILL_FORMAT,
        NO_COMMAND,
        TOO_MANY_ARGS,
        TOO_LITTLE_ARGS,

        COMMAND_ALREADY_EXISTS,
        COMMAND_MISSING_CALLBACK
    };

    static std::string StatusToString(Status s);

    Status Parse(std::string cmd);
    Status AddCommand(Command cmd);
    std::pair<Status, std::string> AddCommandBulk(std::vector<Command> cmds);

    std::vector<Command> GetCommands();
};
