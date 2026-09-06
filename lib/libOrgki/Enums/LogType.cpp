#include "Enums/LogType.hpp"

std::string LogTypeUtils::TypeToString(const LogType type) {
	if (m_TypeString.contains(type))
		return m_TypeString.at(type);
	else
		return m_TypeString.at(LogType::None);
}

LogType LogTypeUtils::StringToType(const std::string& str) {
	for (auto& pair : m_TypeString) {
		if (str == pair.second)
			return pair.first;
	}
	return LogType::Custom;
}
