#pragma once
#include "Colors.hpp" // Include the colors header file which contains the color class used to set the color of the console text to make it look pretty

#include <source_location>
#include <filesystem>
#include <fstream>
#include <mutex>

namespace Utils
{
	// File logger for debugging crashes
	inline std::ofstream g_LogFile;
	inline std::mutex g_LogMutex;
	inline bool g_LogFileInitialized = false;

	inline void InitFileLog()
	{
		if (!g_LogFileInitialized)
		{
			g_LogFile.open("MOMOCrash_log.txt", std::ios::out | std::ios::trunc);
			g_LogFileInitialized = true;
			if (g_LogFile.is_open())
				g_LogFile << "=== MOMOCrash Log Started ===" << std::endl;
		}
	}

	inline void LogToFile(const std::string& message)
	{
		std::lock_guard<std::mutex> lock(g_LogMutex);
		if (!g_LogFileInitialized)
			InitFileLog();
		if (g_LogFile.is_open())
		{
			g_LogFile << message << std::endl;
			g_LogFile.flush();
		}
	}

	inline void CloseFileLog()
	{
		if (g_LogFile.is_open())
		{
			g_LogFile << "=== MOMOCrash Log Ended ===" << std::endl;
			g_LogFile.close();
		}
	}
	struct Location // A struct to hold the location of the error used in the logging function for pretty printing
	{
		std::string m_sFilename;
		std::string m_sFunction;
		unsigned int m_iLine;
		unsigned int m_iColumn;
	};

	Location GetLocation(std::source_location stLocation); // Convert the source location to a location struct

	// Various logging functions for different types of messages
	void LogHook(Location stLocation, std::string sHookName, std::string sReason, std::string sMessage);

	void LogError(Location stLocation, int iErrorCode);
	void LogError(Location stLocation, std::string sErrorMessage);

	void LogDebug(Location stLocation, std::string sDebugMessage);

	// Because std::format verifies values at compile time sometimes we need to use its runtime version std::vforamt this is my wrapper for it
	template<typename... Args>
	inline std::string _Format(const std::format_string<Args...> fmt, Args&&... args)
	{
		return std::vformat(fmt.get(), std::make_format_args(args...));
	}
}