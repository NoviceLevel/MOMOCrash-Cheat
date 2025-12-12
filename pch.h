#pragma once

#define FRAMEWORK_VERSION 1.0.0
#define FRAMEWORK_MAJOR_VERSION 1
#define FRAMEWORK_MINOR_VERSION 0
#define FRAMEWORK_REWORK_VERSION 0

#define FRAMEWORK_CODENAME "MOMOCrash"
#define FRAMEWORK_TARGET_GAME "MomottoCrash"
#pragma warning(disable : 5056)
static_assert(FRAMEWORK_TARGET_GAME != "", "Target game not set.");

#define FRAMEWORK_OTHER 0
#define FRAMEWORK_UNREAL 0
#define FRAMEWORK_UNITY 1

#define IGNORE_32BIT_WARNING 1

#if !FRAMEWORK_OTHER && !FRAMEWORK_UNREAL && !FRAMEWORK_UNITY
#error "No framework selected"
#else
#if FRAMEWORK_OTHER && (FRAMEWORK_UNREAL || FRAMEWORK_UNITY)
#error "Multiple frameworks selected"
#endif
#if FRAMEWORK_UNREAL && (FRAMEWORK_OTHER || FRAMEWORK_UNITY)
#error "Multiple frameworks selected"
#endif
#if FRAMEWORK_UNITY && (FRAMEWORK_OTHER || FRAMEWORK_UNREAL)
#error "Multiple frameworks selected"
#endif
#endif

#define FRAMEWORK_RENDER_D3D11 1
#define FRAMEWORK_RENDER_D3D12 0

#if FRAMEWORK_RENDER_D3D11
#define KIERO_INCLUDE_D3D11  1
#define KIERO_INCLUDE_D3D12  0
#endif

#if FRAMEWORK_RENDER_D3D12
#define KIERO_INCLUDE_D3D11  0
#define KIERO_INCLUDE_D3D12  1
#endif

#include "Kiero/kiero.h"

#include <source_location>
#define CurrentLoc std::source_location::current()

#ifndef PCH_H
#define PCH_H

#if FRAMEWORK_UNREAL
#include "SDK.h"
#endif

#include <thread>
#include <chrono>
#include <memory>
#include <format>
#include <string>
#include <cmath>
#include <sstream>
#include <cstdio>
#include <vector>
#include <eh.h>

#define _USE_MATH_DEFINES
#include <math.h>

#include "Utils/Utils.h"
#include "Interfaces/EasyHook.h"

#if FRAMEWORK_UNREAL
#include "Interfaces/Unreal.h"
#endif

#if FRAMEWORK_UNITY
#include "Interfaces/Mono.h"
#endif

#define IMGUI_DEFINE_MATH_OPERATORS
#pragma warning(disable : 4244)
#include "ImGUI/imgui.h"
#include "ImGUI/imgui_impl_win32.h"

#if FRAMEWORK_RENDER_D3D11
#include "ImGUI/imgui_impl_dx11.h"
#endif

#if FRAMEWORK_RENDER_D3D12
#include "ImGUI/imgui_impl_dx12.h"
#endif

#include "ImGUI/Styles.h"
#include "GUI/Custom.h"
#include "GUI/GUI.h"

#define DEG2RAD(deg) deg * M_PI / 180
#define RAD2DEG(rad) rad * 180.0 / M_PI;
#define U8(str) reinterpret_cast<const char*>(u8##str)

namespace Cheat
{
	static const std::string Framework = FRAMEWORK_CODENAME;
	static const std::string Game = FRAMEWORK_TARGET_GAME;
	static const std::string Title = Framework + " (" + Game + ")";

	#ifdef _WIN64
	static constexpr bool bIs64Bit = true;
	#else
	static constexpr bool bIs64Bit = true;
	#endif

	inline bool bShouldRun = true;
	inline DWORD dwThreadID = NULL;
	inline HMODULE hModule = NULL;

	static constexpr DWORD dwMenuKey = VK_INSERT;
	static constexpr DWORD dwUnloadKey = VK_END;
	static constexpr DWORD dwConsoleKey = VK_HOME;

	inline std::unique_ptr<Console> console = std::make_unique<Console>(false, Title);

	#if FRAMEWORK_UNREAL
	inline std::unique_ptr<Unreal> unreal = std::make_unique<Unreal>();
	#endif

	#if FRAMEWORK_UNITY
	inline Mono mono = Mono::Instance();
	#endif
}

#include "SDK/SDK.h"
#include "Features/Feature.h"
#include "Features/Quit/Quit.h"
#include "Features/GameCheats/GameCheats.h"
#include "Features/SaveEditor/SaveEditor.h"
#include "Features/ScoreModifier/ScoreModifier.h"

inline std::vector<std::unique_ptr<Feature>> Features;

#endif //PCH_H
