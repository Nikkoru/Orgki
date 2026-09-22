#include "libOrgki/Commands/Command.hpp"
#include "libOrgki/Commands/Parser.hpp"
#include "libOrgki/Context.hpp"
#include "libOrgki/Settings/SettingsManager.hpp"
#include "libOrgki/Logger.hpp"
#include <filesystem>
#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/component/component_options.hpp>
#include <ftxui/component/event.hpp>
#include <ftxui/component/loop.hpp>
#include <ftxui/dom/elements.hpp>
#include <print>
#include <ftxui/ftxui.hpp>

#include <lua.hpp>

#ifdef ORGKI_GUI
#include <QApplication>
#include "Orgki/MainWindow.hpp"
#else
#include <string>
#endif

int main(int argc, char** argv) {
    Logger::GetInstance()->SaveToLogFile("log.txt");

    auto settings = Orgki::ContextInitSettings{
        .settingsPath = "settings.json"
    };
    auto ctx = Orgki::Context{ settings };

    std::println("lost a week bc of mistakenly putting discard in the commit instead of reset");
    std::println("current path = {}", std::filesystem::current_path().string());
#ifdef ORGKI_CLI
    std::string cmd{};
    Logger::GetInstance()->LogToConsole(false);
    
    while (true) {
        std::println();
        std::println("{}cli stuff{}", ASCII_GRAY_FG, ASCII_RESET);
        std::print("{}>> {}", ASCII_BLUE_FG, ASCII_RESET);
        std::getline(std::cin, cmd);

        if (auto status = ctx.parser.Parse(cmd); !status.has_value()) {
            // std::println("Parser returned: {}", Orgki::Parser::StatusToString(status.error()));
            if (status.error() == Orgki::Parser::Status::REQUEST_EXIT)
                break;
            // else {
            //     std::println("ERR: {}", Orgki::Parser::StatusToString(status.error()));
            // }
            switch (status.error()) {
            case Orgki::Parser::Status::ILL_FORMAT:
                std::println("{}The command was ill formatted{}", ASCII_RED_FG, ASCII_RESET);
                break;
            case Orgki::Parser::Status::NO_COMMAND_FOUND:
                std::println("{}Command not found{}", ASCII_RED_FG, ASCII_RESET);
                break;
            case Orgki::Parser::Status::TOO_LITTLE_ARGS:
                std::println("{}Command requires more arguments{}", ASCII_RED_FG, ASCII_RESET);
                break;
            case Orgki::Parser::Status::TOO_MANY_ARGS:
                std::println("{}Command requires less arguments{}", ASCII_RED_FG, ASCII_RESET);
                break;
            default:
                std::println("{}Parser failed: {}{}", ASCII_RED_FG, ASCII_RESET, Orgki::Parser::StatusToString(status.error()));
            };
        }
        else {
            // std::println("Command returned: {}", Orgki::Parser::StatusToString(status.value().status));
            // std::println("Message: {}", status.value().msg);
            if (status.value().status != Orgki::Command::Status::OK)
                std::println("ERR {}: {}",  Orgki::Parser::StatusToString(status.value().status), status.value().msg);
            else if (!status.value().msg.empty())
                std::println("{}", status.value().msg);
        }
    }
#elif defined(ORGKI_GUI)
    QApplication app{argc, argv};
    MainWindow win{ ctx, app };
    std::println("Orgki or smth");

    win.show();

    return app.exec();
#else
    Logger::GetInstance()->LogToConsole(false);

    std::string cmd{};
    auto screen= ftxui::App::Fullscreen();
    auto elms = ftxui::Container::Vertical({});

    int depth = 0;
    bool requestDetach = false;
    auto renderers = ftxui::Container::Tab({}, &depth);

    bool showSidePanel = false;
    auto sidePanel = ftxui::Renderer([] {
        return ftxui::hbox({
            ftxui::vbox({
                ftxui::text("Side Panel"),
                ftxui::separator(),
                ftxui::text("Bottom")
            }),
            ftxui::separator()
        });
    }) | ftxui::Maybe(&showSidePanel);

    bool showCommandLine = false;
    std::string cmdMsg{};
    ftxui::Color cmdColor{};
    int cmdCurPos = 0;
    auto cmdLineConfig = ftxui::InputOption{
        .multiline = false,
        .on_enter = [&screen, &cmd, &cmdMsg, &ctx, &cmdColor] {
            auto status = ctx.parser.Parse(cmd);
            if (!status.has_value()) {
                switch (status.error()) {
                case Orgki::Parser::Status::OK:
                case Orgki::Parser::Status::BUFFER_EMPTY:
                case Orgki::Parser::Status::COMMAND_ALREADY_EXISTS:
                    break;
                case Orgki::Parser::Status::REQUEST_EXIT:
                    screen.Exit();
                    break;
                case Orgki::Parser::Status::ILL_FORMAT:
                    cmdMsg = "Ill formated";
                    break;
                case Orgki::Parser::Status::NO_COMMAND_FOUND:
                    cmdMsg = format("Command \"{}\" was not found", cmd);
                    break;
                case Orgki::Parser::Status::TOO_MANY_ARGS:
                    cmdMsg = "Command expects less args";
                    break;
                case Orgki::Parser::Status::TOO_LITTLE_ARGS:
                    cmdMsg = "Command expects more args";
                    break;
                case Orgki::Parser::Status::COMMAND_MISSING_CALLBACK:
                    cmdMsg = "Command doesn't have a callback to handle this command";
                    break;
                  break;
                }
                cmdColor = ftxui::Color::Red;
            }
            else {
                cmdColor = ftxui::Color::Green;
                cmdMsg = status.value().msg;
            }
			cmd.erase();
        },
        .cursor_position = &cmdCurPos
    };


    auto commandLineInput = ftxui::Input(&cmd, cmdLineConfig);

    bool showCreatePlanModal = false;
    elms->Add(ftxui::Button("Create Plan", [&] {
        showCreatePlanModal = true;
    }));

    std::string planName{};
    std::string planDesc{};

    auto planCreateElms = ftxui::Container::Vertical({
        ftxui::Input(&planName),
        ftxui::Input(&planDesc),
        ftxui::Container::Horizontal({
            ftxui::Button("Create", [&] {
                ctx.planMgr.CreatePlan(planName, planDesc);
                requestDetach = true;
                planName.erase();
                planDesc.erase();
            }),
            ftxui::Button("Cancel", [&] {
                requestDetach = true;
                planName.erase();
                planDesc.erase();
            })
        })
    });
    
    auto mainBody = ftxui::Container::Vertical({
        elms
    });
    auto mainBodyRenderer = ftxui::Renderer(mainBody, [&] {
        return ftxui::vbox({
            ftxui::text("Main Body"),
            ftxui::separator(),
            elms->Render()
        });
    });

    auto globalLayout = ftxui::Container::Horizontal({
        sidePanel,
        mainBodyRenderer
    });

    renderers->Add(ftxui::Renderer(globalLayout, [&] {
        return ftxui::hbox({
            sidePanel->Render(),
            globalLayout->Render()
        });
    }));

    auto addCreatePlanModal = [&] {
        renderers->Add(ftxui::Renderer(planCreateElms, [&] {
            ftxui::Elements elms{};

            for (size_t i = 0; i < planCreateElms->ChildCount(); ++i) {
                elms.emplace_back(planCreateElms->ChildAt(i)->Render()); 
            }
            return ftxui::vbox(elms) | ftxui::border | ftxui::clear_under | ftxui::center;
        }));
    };

    auto addCmd = [&] {
        renderers->Add(ftxui::Renderer(commandLineInput, [&] {
            ftxui::Elements elms{
                ftxui::filler()
            };
            if (!cmdMsg.empty())
                elms.emplace_back(ftxui::color(cmdColor, ftxui::text(cmdMsg)));
            elms.emplace_back(ftxui::hbox({ ftxui::text(">> "), commandLineInput->Render() }));

            return ftxui::vbox(elms);
        }) | ftxui::Maybe(&showCommandLine) | ftxui::CatchEvent([&](ftxui::Event event) -> bool {
            if (event.is_character()) {
                if (ctx.parser.HasActiveMatch())
                    ctx.parser.ClearMatches();
                cmdMsg.erase();
            }
            else if (event == ftxui::Event::Escape) {
                showCommandLine = false;
                cmdMsg.erase();
                cmd.erase();
                requestDetach = true;
                return true;
            }
            else if (event == ftxui::Event::Tab || event == ftxui::Event::TabReverse) {
                Orgki::Command matchCmd{};
                if (!ctx.parser.HasActiveMatch()) {
                    if (event == ftxui::Event::TabReverse)
                        return false;

                    ctx.parser.FindMatches(cmd);
                }

                std::expected<Orgki::Command, Orgki::Parser::Status> match{};

                if (event == ftxui::Event::Tab)
                    match = ctx.parser.GetNextMatch();
                else if (event == ftxui::Event::TabReverse)
                    match = ctx.parser.GetPreviousMatch();

                if (match.has_value()) {
                    matchCmd = match.value();
                    cmd = matchCmd.command;
					cmdCurPos = cmd.size();
                }

                return true;
            }
            else if (event == ftxui::Event::ArrowUp) {
                cmd = ctx.parser.GetNextHistory();
                cmdCurPos = cmd.size();
                return true;
            }
            else if (event == ftxui::Event::ArrowDown) {
                cmd = ctx.parser.GetPreviousHistory();
                cmdCurPos = cmd.size();
                return true;
            }

            return false;
        }));
    };

    auto mainRenderer = ftxui::Renderer(renderers, [&] {
        ftxui::Elements elms{};

        for (size_t i = 0; i < renderers->ChildCount(); ++i) {
            auto renderer = renderers->ChildAt(i);
            if (requestDetach) {
                requestDetach = false;
                renderers->ChildAt(depth)->Detach();
                depth--;
                continue;
            }
            elms.emplace_back(renderer->Render());
        }

        return ftxui::dbox(elms);
    });

    mainRenderer |=  ftxui::CatchEvent([&](ftxui::Event event) -> bool {
        if (event.is_character() && showCommandLine) return false;
        if (event.is_character() && depth != 0) return false;

        if (event == ftxui::Event::Character('q')) {
            screen.Exit();
            return true;
        }
        else if (event == ftxui::Event::Character(' ')) {
            showSidePanel = !showSidePanel;
            return true;
        }
        else if (event == ftxui::Event::Character('c')) {
            showCommandLine = !showCommandLine; 
            if (showCommandLine) {
                addCmd();
                depth++;
            }
            return true;
        }

        return false;
    });

    ftxui::Loop mainLoop{ &screen, mainRenderer };

    while (!mainLoop.HasQuitted()) {
        mainLoop.RunOnce();
        if (showCreatePlanModal) {
            showCreatePlanModal = false;
            addCreatePlanModal();
            depth++;
        }
    }
#endif
}
