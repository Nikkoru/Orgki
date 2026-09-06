#include "Logger.hpp"

#include <algorithm>
#include <cctype>
#include <cstring>
// #include <ctime>
#include <iostream>
#include <fstream>
#include <format>
#include <string>

template<typename... Args>
void Logger::AddLog(std::string logType, std::string caller, std::string_view fmt, Args&&... elms) {
    if (fmt == "") return;   

    std::string preFormattedLog{};
    std::string formattedLog{};
    std::string savedLog{};

    std::format_args fmt_elms = std::make_format_args();

    auto unescapedLogType = logType; 

    {
        while (true) {
            auto it = std::find_if(unescapedLogType.begin(), unescapedLogType.end(), [](unsigned char c) { return std::iscntrl(c); });
            if (it == unescapedLogType.end()) break;
			auto sub = std::string_view{ it, unescapedLogType.end() };
            auto mPos = sub.find('m') + 1;

            unescapedLogType.erase(it, it + mPos);
        }
        // std::find();
        // auto _ = std::remove_if(unescapedLogType.begin(), unescapedLogType.end(), [](unsigned char c) {
        //     return std::iscntrl(c);
        // });
        // unescapedLogType.
    }
        
    if constexpr (sizeof...(elms) > 0) {
        fmt_elms = std::make_format_args(elms...);
        preFormattedLog = std::vformat(fmt, fmt_elms);
    }
    else
        preFormattedLog = fmt;
    
	//    auto now = Clock::GetNow();
	// auto nowT = now.AsTimeT(); 
	//
	// std::tm* nowTM = std::localtime(&nowT);
	// char nowStr[80];
	// std::strftime(nowStr, sizeof(nowStr), "%T", nowTM);

	if (caller != "") {
		formattedLog = std::format("[{}] {} ({})", logType, preFormattedLog, caller);
        savedLog = std::format("[{}] {} ({})", unescapedLogType, preFormattedLog, caller);
	}
	else { 
		formattedLog = std::format("[{}] {}", logType, preFormattedLog);
        savedLog = std::format("[{}] {}", unescapedLogType, preFormattedLog);
    }

    std::cout << formattedLog << '\n';
    
    if (!GetInstance()->m_LogFile.empty()) {
        std::ofstream o{ GetInstance()->m_LogFile, std::ios::app };
        if (!o.is_open()) return;
        o << savedLog << '\n';
        o.close();
    }

	GetInstance()->m_Logs.emplace_back(LogTypeUtils::StringToType(logType), savedLog);
}
template<typename... Args>
void Logger::AddLog(LogType logType, std::string caller, std::string_view fmt, Args&&... elms) {
    AddLog(std::format("{}{}\033[0m", GetColorViaLogType(logType), LogTypeUtils::TypeToString(logType)), caller, fmt, elms...);
}

template<typename... Args>
void Logger::AddLog(std::string logType, std::type_index caller, std::string_view fmt, Args&&... elms) {
	if (caller == typeid(nullptr))
		AddLog(logType, "", fmt, elms...);
	else {
		std::string callerName = caller.name();
		if (callerName.contains("class")) {
			callerName = callerName.substr(callerName.find("class") + strlen("class") + 1);
		}
		else if (callerName.contains("struct"))
			callerName = callerName.substr(callerName.find("struct") + strlen("struct") + 1);
		else
			callerName = caller.name();
		AddLog(logType, callerName, fmt, elms...);
	}
}

template<typename... Args>
void Logger::AddLog(LogType logType, std::type_index caller, std::string_view fmt, Args&&... elms) {
    AddLog(std::format("{}{}\033[0m", GetColorViaLogType(logType), LogTypeUtils::TypeToString(logType)), caller, fmt, elms...);
}


template<typename... Args>
void Logger::AddDebug(std::string caller, std::string_view fmt, Args&&... elms) {
    AddLog(LogType::Debug, caller, fmt, elms...);
}
template<typename... Args>
void Logger::AddDebug(std::type_index caller, std::string_view fmt, Args&&... elms) {
    AddLog(LogType::Debug, caller, fmt, elms...);
}
template<typename... Args>
void Logger::AddInfo(std::string caller, std::string_view fmt, Args&&... elms) {
    AddLog(LogType::Info, caller, fmt, elms...);
}
template<typename... Args>
void Logger::AddInfo(std::type_index caller, std::string_view fmt, Args&&... elms) {
    AddLog(LogType::Info, caller, fmt, elms...);
}
template<typename... Args>
void Logger::AddWarning(std::string caller, std::string_view fmt, Args&&... elms) {
    AddLog(LogType::Warning, caller, fmt, elms...);
}
template<typename... Args>
void Logger::AddWarning(std::type_index caller, std::string_view fmt, Args&&... elms) {
    AddLog(LogType::Warning, caller, fmt, elms...);
}
template<typename... Args>
void Logger::AddError(std::string caller, std::string_view fmt, Args&&... elms) {
    AddLog(LogType::Error, caller, fmt, elms...);
}
template<typename... Args>
void Logger::AddError(std::type_index caller, std::string_view fmt, Args&&... elms) {
    AddLog(LogType::Error, caller, fmt, elms...);
}
template<typename... Args>
void Logger::AddCritical(std::string caller, std::string_view fmt, Args&&... elms) {
    AddLog(LogType::Critical, caller, fmt, elms...);
}
template<typename... Args>
void Logger::AddCritical(std::type_index caller, std::string_view fmt, Args&&... elms) {
    AddLog(LogType::Critical, caller, fmt, elms...);
}
