#include "pch.h"

// Helper to simplify function name (extract just ClassName::FunctionName)
static std::string SimplifyFunctionName(const std::string& fullName)
{
	// Find the last "::" before "(" to get ClassName::FunctionName
	size_t parenPos = fullName.find('(');
	std::string beforeParen = (parenPos != std::string::npos) ? fullName.substr(0, parenPos) : fullName;
	
	// Find last space (before the actual function name part)
	size_t lastSpace = beforeParen.rfind(' ');
	if (lastSpace != std::string::npos)
		beforeParen = beforeParen.substr(lastSpace + 1);
	
	return beforeParen + "()";
}

Utils::Location Utils::GetLocation(std::source_location stLocation)
{
	return { std::filesystem::path(stLocation.file_name()).filename().string(), stLocation.function_name(), stLocation.line(), stLocation.column() };
}

void Utils::LogHook(Location stLocation, std::string sHookName, std::string sReason, std::string sMessage)
{
	// Hook[HookName]: Filename | Function() -> Ln: 1 Col: 1 | Reason: Message
	std::cout << colors::cyan << "Hook[" << sHookName << "]" << colors::white << ": ";
	std::cout << colors::green << stLocation.m_sFilename << colors::white << " | ";
	std::cout << colors::green << SimplifyFunctionName(stLocation.m_sFunction) << colors::white;
	std::cout << " -> Ln: " << colors::magenta << stLocation.m_iLine << colors::white << " Col: " << colors::magenta << stLocation.m_iColumn << colors::white;
	std::cout << " | " << colors::yellow << sReason << colors::white << ": " << sMessage << std::endl;
}

void Utils::LogError(Location stLocation, int iErrorCode)
{
	// Error: Filename | Function() -> Ln: 1 Col: 1 | Info: Message
	std::cout << colors::red << "Error" << colors::white << ": ";
	std::cout << colors::green << stLocation.m_sFilename << colors::white << " | ";
	std::cout << colors::green << SimplifyFunctionName(stLocation.m_sFunction) << colors::white;
	std::cout << " -> Ln: " << colors::magenta << stLocation.m_iLine << colors::white << " Col: " << colors::magenta << stLocation.m_iColumn << colors::white;
	std::cout << " | " << colors::yellow << "Info" << colors::white << ": " << std::system_category().message(iErrorCode) << std::endl;
}

void Utils::LogError(Location stLocation, std::string sErrorMessage)
{
	// Error: Filename | Function() -> Ln: 1 Col: 1 | Info: Message
	std::cout << colors::red << "Error" << colors::white << ": ";
	std::cout << colors::green << stLocation.m_sFilename << colors::white << " | ";
	std::cout << colors::green << SimplifyFunctionName(stLocation.m_sFunction) << colors::white;
	std::cout << " -> Ln: " << colors::magenta << stLocation.m_iLine << colors::white << " Col: " << colors::magenta << stLocation.m_iColumn << colors::white;
	std::cout << " | " << colors::yellow << "Info" << colors::white << ": " << sErrorMessage << std::endl;
}

void Utils::LogDebug(Location stLocation, std::string sDebugMessage)
{
	// Debug: Filename | Function() -> Ln: 1 Col: 1 | Info: Message
	std::cout << colors::cyan << "Debug" << colors::white << ": ";
	std::cout << colors::green << stLocation.m_sFilename << colors::white << " | ";
	std::cout << colors::green << SimplifyFunctionName(stLocation.m_sFunction) << colors::white;
	std::cout << " -> Ln: " << colors::magenta << stLocation.m_iLine << colors::white << " Col: " << colors::magenta << stLocation.m_iColumn << colors::white;
	std::cout << " | " << colors::yellow << "Info" << colors::white << ": " << sDebugMessage << std::endl;
}
