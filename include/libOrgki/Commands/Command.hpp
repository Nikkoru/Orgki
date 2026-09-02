#pragma once

#include <functional>
#include <string>
#include <vector>

using CommandArgs = std::vector<std::string>;
using CommandCallback = std::function<void(CommandArgs&)>;

struct Command {
    std::string command{};
    std::string description{};
    std::string usage{};
    CommandCallback callback;
};
