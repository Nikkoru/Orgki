#pragma once

#include <string>
#include <vector>

#include "Plan.hpp"
#include "Commands/Command.hpp"

namespace Orgki {
class Parser;
class Context;
class PlanManager {
private:
    friend class Context;
    std::vector<Plan> m_LoadedPlans;
    Plan* m_ActivePlan{ nullptr };
private:
    Command::StatusData _GetPlansCallback(Context* ctx, std::vector<std::string>& args);
    Command::StatusData _CreatePlanCallback(Context* ctx, std::vector<std::string>& args);
    Command::StatusData _DeletePlanCallback(Context* ctx, std::vector<std::string>& args);
    Command::StatusData _SetActivePlanCallback(Context* ctx, std::vector<std::string>& args);

    Command::StatusData _CreateTableCallback(Context* ctx, std::vector<std::string>& args);
    Command::StatusData _GetTablesCallback(Context* ctx, std::vector<std::string>& args);
    Command::StatusData _EditTableCallback(Context* ctx, std::vector<std::string>& args);
    Command::StatusData _GetTableCallback(Context* ctx, std::vector<std::string>& args);

    Command::StatusData _CreateActivityCallback(Context* ctx, std::vector<std::string>& args);
    Command::StatusData _GetActivitiesCallback(Context* ctx, std::vector<std::string>& args);
    Command::StatusData _EditActivityCallback(Context* ctx, std::vector<std::string>& args);
    Command::StatusData _GetActivityCallback(Context* ctx, std::vector<std::string>& args);

    Command::StatusData _AddActivityToTableCallback(Context* ctx, std::vector<std::string>& args);

    void _AddCommands(Parser& parser);
public:
    bool IsEmpty();

    std::vector<Plan>& GetAllPlans();

    PlanID CreatePlan(const std::string& name, const std::string& desc = "");
    void DeletePlan(PlanID id);
};
}
