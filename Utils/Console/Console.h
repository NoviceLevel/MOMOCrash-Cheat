#pragma once
#include <iostream>
#include <format>

class Console
{
private:
	FILE* m_pSTDOutDummy = nullptr;
	FILE* m_pSTDInDummy = nullptr;

	bool bAllocated = false;
	bool bInitalized = false;
	bool bVisible = false;

#ifdef _WIN64
	static constexpr bool bIs64Bit = true;
#else
	static constexpr bool bIs64Bit = true;
#endif

public:
	Console(bool bVisibility, std::string sConsoleTitle = std::format("DEBUG CONSOLE | {}", (bIs64Bit) ? "x64" : "x32"));
	void Destroy();

	void SetTitle(std::string sTitle) { SetConsoleTitleA(sTitle.c_str()); }

	bool GetVisibility() { return this->bVisible; };
	void SetVisibility(bool bVisibility);
	void ToggleVisibility();
};