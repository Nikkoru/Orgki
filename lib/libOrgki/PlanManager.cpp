#include "PlanManager.hpp"

#include "Logger.hpp"

#include "Helpers/Time.hpp"
#include "Context.hpp"

namespace Orgki {
Command::StatusData PlanManager::_GetPlansCallback(Context* ctx, std::vector<std::string>& args) {
    using namespace std::string_literals;
    for (const auto& plan : m_LoadedPlans) {
        ctx->PrintLnToBuffer("ID: {} -> {}, {}", 0, plan.GetName(), !plan.GetDescription().empty() ? "\""s + plan.GetDescription() + "\""s : "No desc"s);
    }

    return {
        .msg = ctx->buffer.str(),
        .status = Command::Status::OK
    };
}

Command::StatusData PlanManager::_CreatePlanCallback(Context* ctx, std::vector<std::string>& args) {
    auto name = args[0];
    std::string description{};
    if (args.size() == 2)
        description = args[1];

    CreatePlan(name, description);

    return {
        .msg = std::format("created plan \"{}\"", name),
        .status = Command::Status::OK
    };
}

Command::StatusData PlanManager::_SetActivePlanCallback(Context* ctx, std::vector<std::string>& args) {
    auto planID = std::stoi(args[0]);

    if (planID >= m_LoadedPlans.size()) return {
        .msg = std::format("Invalid ID: {}", planID),
        .status = Command::Status::INVALID_ARGS
    };

    m_ActivePlan = &m_LoadedPlans.at(planID);

    return {
        .msg = std::format("loaded: {}", m_ActivePlan->GetName()),
        .status = Command::Status::OK
    };
}

Command::StatusData PlanManager::_CreateTableCallback(Context* ctx, std::vector<std::string>& args) {
    if (!m_ActivePlan) return {
        .msg = "There's not an active plan",
        .status = Command::Status::FAILED
    };

    m_ActivePlan->AddTable(args[0]);

    return {
        .msg = std::format("created table \"{}\"", args[0]),
        .status = Orgki::Command::Status::OK
    };
}

Command::StatusData PlanManager::_GetTablesCallback(Context* ctx, std::vector<std::string>& args) {
    if (!m_ActivePlan) return {
        .msg = "There's not an active plan",
        .status = Command::Status::FAILED
    };

    ctx->PrintLnToBuffer("Tables: {}", m_ActivePlan->GetTableCount());
    for (const auto& [id, table] : m_ActivePlan->GetTables()) {
        ctx->PrintToBuffer("ID: {} ", id);
        ctx->PrintLnToBuffer("{}", table.GetTableName());
        ctx->PrintLnToBuffer("  {} {}", table.GetActivityCount(), (table.GetActivityCount() == 0 || table.GetActivityCount() > 1) ? "Activities" : "Activity");
    }
    return {
        .msg = ctx->buffer.str(),
        .status = Orgki::Command::Status::OK
    };
}

Command::StatusData PlanManager::_EditTableCallback(Context* ctx, std::vector<std::string>& args) {
    if (!m_ActivePlan) return {
        .msg = "There's not an active plan",
        .status = Command::Status::FAILED
    };
    
    return {
        .msg = "Not implemented",
        .status = Command::Status::FAILED
    };
}

Command::StatusData PlanManager::_GetTableCallback(Context* ctx, std::vector<std::string>& args) {
    if (!m_ActivePlan) return {
        .msg = "There's not an active plan",
        .status = Command::Status::FAILED
    };

    auto tableID = std::stoi(args[0]);
    if (!m_ActivePlan->TableExists(tableID)) {
        return {
            .msg = std::format("invalid ID: {}", tableID),
            .status = Orgki::Command::Status::INVALID_ARGS 
        };
    }

    auto table = m_ActivePlan->GetTable(tableID);

    ctx->PrintLnToBuffer("Table: \"{}\"", table.GetTableName());
    ctx->PrintLnToBuffer("{} {}", table.GetActivityCount(), (table.GetActivityCount() > 1 || table.GetActivityCount() == 0) ? "Activities" : "Activity");
    for (auto& [timeRange, actID] : table.GetActivities()) {
        ctx->PrintToBuffer("{}\t", Helper::TimeRangeToString(timeRange));
        if (!m_ActivePlan->ActivityExists(actID)) {
            ctx->PrintToBuffer("Invalid ID");
            continue;
        }
        auto act = m_ActivePlan->GetActivity(actID);
        ctx->PrintToBuffer("{} ", act.IsDynamic() ? "[DYNAMIC]" : "");
        ctx->PrintToBuffer("ID: {}", actID);
        ctx->PrintLnToBuffer("{} -> {}", act.GetActivityName(), act.GetActivityDescription());
    }

    return {
        .msg = ctx->buffer.str(),
        .status = Orgki::Command::Status::OK
    };
}

Command::StatusData PlanManager::_CreateActivityCallback(Context* ctx, std::vector<std::string>& args) {
    return {
        .msg = "Not implemented",
        .status = Command::Status::FAILED
    };
}

Command::StatusData PlanManager::_GetActivitiesCallback(Context* ctx, std::vector<std::string>& args) {
    if (!m_ActivePlan) return {
        .msg = "There's not an active plan",
        .status = Command::Status::FAILED
    };

    ctx->PrintLnToBuffer("Activities: {}", m_ActivePlan->GetActivityCount());
    for (const auto& [id, act] : m_ActivePlan->GetActivities()) {
        ctx->PrintToBuffer("{}ID: {} ", id);
        ctx->PrintLnToBuffer("{} {}", act.GetActivityName(), (act.IsDynamic()) ? "[DYNAMIC]" : "");
        ctx->PrintLnToBuffer("  {}", act.GetActivityDescription().empty() ? "no description" : std::format("\"{}\"", act.GetActivityDescription()));
    }

    return {
        .msg = ctx->buffer.str(),
        .status = Orgki::Command::Status::OK
    };
}

Command::StatusData PlanManager::_EditActivityCallback(Context* ctx, std::vector<std::string>& args) {
    if (!m_ActivePlan) return {
        .msg = "There's not an active plan",
        .status = Command::Status::FAILED
    };

    auto actID = std::stoi(args[0]);
    auto attr = args[1];

    if (!m_ActivePlan->ActivityExists(actID)) {
        return {
            .msg = std::format("invalid ID: {}", actID),
            .status = Orgki::Command::Status::INVALID_ARGS
        };
    }

    auto& act = m_ActivePlan->GetActivity(actID);

    if (attr == "name") {
        if (args.size() == 3) {
            act.SetActivityName(args[2]);
            return {
                .msg = std::format("Set Activity \"{}\" name to \"{}\"", act.GetActivityName(), args[2]),
                .status = Orgki::Command::Status::OK
            };
        }
        else {
            return {
                .msg = "missing value",
                .status = Orgki::Command::Status::MISSING_ARGS
            };
        }
    }
    else if (std::string{"description"}.substr(0, attr.size()) == attr) {
        if (args.size() == 3) {
            act.SetActivityDescription(args[2]);
            return {
                .msg = std::format("Set Activity \"{}\" description to \"{}\"", act.GetActivityName(), args[2]),
                .status = Orgki::Command::Status::OK
            };
        }
        else {
            return {
                .msg = "missing value",
                .status = Orgki::Command::Status::OK
            };
        }
    }
    else if (attr == "dynamic") {
        if (args.size() >= 3) {
            return {
                .msg = "more arguments than expected",
                .status = Orgki::Command::Status::TOO_MANY_ARGS
            };
        }
        
        act.SetDynamic(true);
        return {
            .msg = std::format("Set Activity \"{}\" to Dynamic", act.GetActivityName()),
            .status = Orgki::Command::Status::OK
        };
    }
    else if (attr == "noDynamic") {
        if (args.size() >= 3) {
            return {
                .msg = "more arguments than expected",
                .status = Orgki::Command::Status::TOO_MANY_ARGS
            };
        }
        
        act.SetDynamic(false);
        return {
            .msg = std::format("Set Activity \"{}\" to Not Dynamic", act.GetActivityName()),
            .status = Orgki::Command::Status::OK
        };
    }
    else {
        return {
            .msg = std::format("Attribute \"{}\" is not valid", attr),
            .status = Orgki::Command::Status::INVALID_ARGS
        };
    }

}

Command::StatusData PlanManager::_GetActivityCallback(Context* ctx, std::vector<std::string>& args) {
    return {
        .msg = "Not implemented",
        .status = Command::Status::FAILED
    };
}

Command::StatusData PlanManager::_AddActivityToTableCallback(Context* ctx, std::vector<std::string>& args) {
    return {
        .msg = "Not implemented",
        .status = Command::Status::FAILED
    };
}

void PlanManager::_AddCommands(Parser& parser) {
    auto getPlansCallback = [this](Context* ctx, std::vector<std::string>& args) -> Command::StatusData {
        return _GetPlansCallback(ctx, args);
    };
    auto createPlanCallback = [this](Context* ctx, std::vector<std::string>& args) -> Command::StatusData {
        return _CreatePlanCallback(ctx, args);
    };
    auto setActivePlanCallback = [this](Context* ctx, std::vector<std::string>& args) -> Command::StatusData {
        return _SetActivePlanCallback(ctx, args);
    };
    auto createTableCallback = [this](Context* ctx, std::vector<std::string>& args) -> Command::StatusData {
        return _CreateTableCallback(ctx, args);
    };
    auto getTablesCallback = [this](Context* ctx, std::vector<std::string>& args) -> Command::StatusData {
        return _GetTablesCallback(ctx, args);
    };
    auto editTableCallback = [this](Context* ctx, std::vector<std::string>& args) -> Command::StatusData {
        return _EditTableCallback(ctx, args);
    };
    auto getTableCallback = [this](Context* ctx, std::vector<std::string>& args) -> Command::StatusData {
        return _GetTableCallback(ctx, args);
    };
    auto editActivityCallback = [this](Context* ctx, std::vector<std::string>& args) -> Command::StatusData {
        return _EditActivityCallback(ctx, args);
    };
    auto getActivityCallback = [this](Context* ctx, std::vector<std::string>& args) -> Command::StatusData {
        return _GetActivityCallback(ctx, args);
    };

    parser.AddCommandBulk({
        Command {
            .command = "getPlans",
            .description = "Gets the active plans",
            .usage = "",
            .minArgs = 0,
            .maxArgs = 0,
            .callback = getPlansCallback
        },
        Command {
            .command = "createPlan",
            .description = "Creates a new plan with the given name and description",
            .usage = "[NAME] <DESCRIPTION>",
            .minArgs = 1,
            .maxArgs = 2,
            .callback = createPlanCallback
        },
        Command {
            .command = "setActivePlan",
            .description = "Sets the active plan for accessing its data",
            .usage = "[ID]",
            .minArgs = 1,
            .maxArgs = 1,
            .callback = setActivePlanCallback
        },
        Command {
            .command = "createTable",
            .description = "Creates a table with the provided name",
            .usage = "[NAME]",
            .minArgs = 1,
            .maxArgs = 1,
            .callback = createTableCallback
        },
        Command {
            .command = "getTables",
            .description = "Get all tables loaded",
            .usage = "",
            .minArgs = 0,
            .maxArgs = 0,
            .callback = getTablesCallback
        },
        Command {
            .command = "editTable",
            .description = "Edits a table attributes by the given value and attribute to edit",
            .usage = "[ID] [ATTRIBUTE] <VALUE>",
            .minArgs = 2,
            .maxArgs = 3,
            .callback = editTableCallback
        },
        Command {
            .command = "getTable",
            .description = "Retrieves the table data from the given ID",
            .usage = "[ID]",
            .minArgs = 1,
            .maxArgs = 1,
            .callback = getTableCallback
        },
        Command {
            .command = "editActivity",
            .description = "Edits a activity attributes by the given value and attribute to edit",
            .usage = "[ID] [ATTRIBUTE] <VALUE>",
            .minArgs = 2,
            .maxArgs = 3,
            .callback = editActivityCallback
        },
        Command {
            .command = "getActivity",
            .description = "Retrieves the Activity data from the given ID",
            .usage = "[ID]",
            .minArgs = 1,
            .maxArgs = 1,
            .callback = getActivityCallback
        }
    });
}

bool PlanManager::IsEmpty() {
    return m_LoadedPlans.empty();
}

std::vector<Plan>& PlanManager::GetAllPlans() {
    return m_LoadedPlans;
}

PlanID PlanManager::CreatePlan(const std::string& name, const std::string& desc) {
    Logger::AddInfo(typeid(PlanManager), "CreatePlan; name = {}, desc = {}", name, desc);

    m_LoadedPlans.emplace_back(name, desc); 
    return m_LoadedPlans.size() - 1;
}
}
