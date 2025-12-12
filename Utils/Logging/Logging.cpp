#include "pch.h"

static std::string SimplifyFunctionName(const std::string& fullName)
{
	size_t parenPos = fullName.find('(');
	std::string beforeParen = (parenPos != std::string::npos) ? fullName.substr(0, parenPos) : fullName;
	
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
	std::cout << colors::cyan << "Hook[" << sHookName << "]" << colors::white << ": ";
	std::cout << colors::green << stLocation.m_sFilename << colors::white << " | ";
	std::cout << colors::green << SimplifyFunctionName(stLocation.m_sFunction) << colors::white;
	std::cout << " -> Ln: " << colors::magenta << stLocation.m_iLine << colors::white << " Col: " << colors::magenta << stLocation.m_iColumn << colors::white;
	std::cout << " | " << colors::yellow << sReason << colors::white << ": " << sMessage << std::endl;
}

void Utils::LogError(Location stLocation, int iErrorCode)
{
	std::cout << colors::red << "Error" << colors::white << ": ";
	std::cout << colors::green << stLocation.m_sFilename << colors::white << " | ";
	std::cout << colors::green << SimplifyFunctionName(stLocation.m_sFunction) << colors::white;
	std::cout << " -> Ln: " << colors::magenta << stLocation.m_iLine << colors::white << " Col: " << colors::magenta << stLocation.m_iColumn << colors::white;
	std::cout << " | " << colors::yellow << "Info" << colors::white << ": " << std::system_category().message(iErrorCode) << std::endl;
}

void Utils::LogError(Location stLocation, std::string sErrorMessage)
{
	std::cout << colors::red << "Error" << colors::white << ": ";
	std::cout << colors::green << stLocation.m_sFilename << colors::white << " | ";
	std::cout << colors::green << SimplifyFunctionName(stLocation.m_sFunction) << colors::white;
	std::cout << " -> Ln: " << colors::magenta << stLocation.m_iLine << colors::white << " Col: " << colors::magenta << stLocation.m_iColumn << colors::white;
	std::cout << " | " << colors::yellow << "Info" << colors::white << ": " << sErrorMessage << std::endl;
}

void Utils::LogDebug(Location stLocation, std::string sDebugMessage)
{
	std::cout << colors::cyan << "Debug" << colors::white << ": ";
	std::cout << colors::green << stLocation.m_sFilename << colors::white << " | ";
	std::cout << colors::green << SimplifyFunctionName(stLocation.m_sFunction) << colors::white;
	std::cout << " -> Ln: " << colors::magenta << stLocation.m_iLine << colors::white << " Col: " << colors::magenta << stLocation.m_iColumn << colors::white;
	std::cout << " | " << colors::yellow << "Info" << colors::white << ": " << sDebugMessage << std::endl;
}
