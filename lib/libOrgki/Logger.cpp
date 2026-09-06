#include "Logger.hpp"
#include "Enums/LogType.hpp"

std::shared_ptr<Logger> Logger::m_Instance = nullptr;

std::string Logger::GetColorViaLogType(LogType logType) {
    const std::string red = "\x1b[0;31m";
    const std::string yellow = "\x1b[0;33m";
    const std::string purple = "\x1b[0;35m";
    const std::string cyan = "\x1b[0;36m";
    const std::string blue = "\x1b[0;34m";
    const std::string hiRed = "\x1b[0;91m";

    std::string color;

    switch (logType) {
        case LogType::Custom:
        case LogType::None:
        case LogType::All:
        case LogType::NoLogs:
        case LogType::Debug:
            color = blue;
            break;
        case LogType::Info:
            color = cyan;
            break;
        case LogType::Warning:
            color = yellow;
            break;
        case LogType::Error:
            color = red;
            break;
        case LogType::Critical:
            color = hiRed;
            break;
        case LogType::DebugTarget:
            color = purple;
            break;
    }

    return color;
}

std::shared_ptr<Logger> Logger::GetInstance() {
	 if (m_Instance == nullptr) 
		 m_Instance = std::make_shared<Logger>(); 
	 return m_Instance;
}

void Logger::SaveToLogFile(const std::filesystem::path& path) {
    if (path.empty()) return;
    m_LogFile = path;
}

std::vector<std::pair<LogType, std::string>> Logger::GetLogs() const {
	return m_Logs;
}

void Logger::ClearLogs() {
	m_Logs.clear();
}
