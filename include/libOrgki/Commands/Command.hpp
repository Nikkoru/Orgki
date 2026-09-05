#pragma once

#include <functional>
#include <string>
#include <vector>

namespace Orgki {
struct Command {
    enum class Status {
        OK = 0,
        FAILED,
        INVALID_ARGS,
        MISSING_ARGS,
        TOO_MANY_ARGS
    };
    struct StatusData {
        std::string msg{};
        Status status{}; 
    };

    using Args = std::vector<std::string>;
    using Callback = std::function<StatusData(Args&)>;

    std::string command{};
    std::string description{};
    std::string usage{};
    size_t minArgs{};
    size_t maxArgs{};
    Callback callback{};
};
}
