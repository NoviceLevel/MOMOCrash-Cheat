#include "pch.h"

// Include the respective rendering API hooks
#if FRAMEWORK_RENDER_D3D11
#include "Hooks/D3D11/D3D11Hooks.h"
#endif

#if FRAMEWORK_RENDER_D3D12
#include "Hooks/D3D12/D3D12Hooks.h"
#endif

#define DO_THREAD_SLEEP 0
#define THREAD_SLEEP_TIME 100

namespace Cheat
{
	bool Init()
	{
		// Initialize file logging first
		Utils::InitFileLog();
		Utils::LogToFile("=== Init() started ===");

	#if FRAMEWORK_RENDER_D3D11
		Utils::LogToFile("Initializing D3D11...");
		kiero::Status::Enum kieroStatus = kiero::init(kiero::RenderType::D3D11);
		Utils::LogToFile("kiero::init result: " + std::to_string(static_cast<int>(kieroStatus)));
		
		if (kieroStatus == kiero::Status::Success)
		{
			Utils::LogToFile("kiero::init succeeded, binding Present hook...");
			kiero::Status::Enum bindStatus = kiero::bind(8, reinterpret_cast<void**>(&oPresent), hkPresent);
			Utils::LogToFile("kiero::bind result: " + std::to_string(static_cast<int>(bindStatus)));
			
			if (bindStatus != kiero::Status::Success)
			{
				Utils::LogToFile("ERROR: kiero::bind failed!");
				return false;
			}
			Utils::LogToFile("Present hook bound successfully");
		}
		else
		{
			Utils::LogToFile("ERROR: kiero::init failed!");
			return false;
		}
	#endif

	#if FRAMEWORK_UNREAL // If the framework is Unreal initalize the SDK assuming the SDK was generated with CheatGeat by Cormm
		if (!CG::InitSdk())
			return false;

		if (!(*CG::UWorld::GWorld))
			Utils::LogError(Utils::GetLocation(CurrentLoc), "Waiting for GWorld to initalize");

		while (!(*CG::UWorld::GWorld))
			continue;
	#endif

	#if FRAMEWORK_RENDER_D3D12
		if (kiero::init(kiero::RenderType::D3D12) == kiero::Status::Success)
		{
			if (kiero::bind(54, (void**)&oExecuteCommandLists, hkExecuteCommandLists) != kiero::Status::Success)
				return false;

			if (kiero::bind(140, (void**)&oPresent, hkPresent) != kiero::Status::Success)
				return false;
		}
		else
			return false;
	#endif

		Utils::LogDebug(Utils::GetLocation(CurrentLoc), "Initalizing Globals, this can take a bit"); // Log that the globals are being initalized

		MH_STATUS Status = MH_Initialize();
		if (Status == MH_ERROR_ALREADY_INITIALIZED)
			Utils::LogError(Utils::GetLocation(CurrentLoc), "MinHook already initalized");

	#if FRAMEWORK_UNREAL // If using the Unreal framework print the pointer to the Unreal class to make sure it was initalized
		Utils::LogDebug(Utils::GetLocation(CurrentLoc), (std::stringstream() << "Unreal: 0x" << unreal.get()).str());
	#endif

		// Add other globals that need to be initalized here

		Utils::LogDebug(Utils::GetLocation(CurrentLoc), "Globals Initalized"); // Log that the globals have been initalized

		// https://stackoverflow.com/questions/16711697/is-there-any-use-for-unique-ptr-with-array
		// Features - GameCheats must be first to setup hooks that capture game instances
		Utils::LogToFile("Creating Features...");
		
		Utils::LogToFile("Creating Quit...");
		Features.push_back(std::make_unique<Quit>());
		
		Utils::LogToFile("Creating GameCheats (hooks for instance capture)...");
		Features.push_back(std::make_unique<GameCheats>());
		
		Utils::LogToFile("Creating SaveEditor...");
		Features.push_back(std::make_unique<SaveEditor>());
		
		Utils::LogToFile("Creating ScoreModifier...");
		Features.push_back(std::make_unique<ScoreModifier>());

		Utils::LogToFile("Setting up Features...");
		for (size_t i = 0; i < Features.size(); i++) // A loop to grap the feature pointers and call their respective setup functions
		{
			Utils::LogToFile("Setting up Feature " + std::to_string(i) + "...");
			Features[i].get()->Setup();
			Utils::LogToFile("Feature " + std::to_string(i) + " setup complete");
		}
		Utils::LogToFile("All Features setup complete");

		return true; // Return true if the initalization was successful
	}

	void HandleKeys() // A function to handle the keys of both the menu and the features
	{
		if (GetAsyncKeyState(dwMenuKey) & 0x1)
		{
			GUI::bMenuOpen = !GUI::bMenuOpen;

			ImGui::GetIO().MouseDrawCursor = GUI::bMenuOpen;

			if (ImGui::GetIO().MouseDrawCursor)
				SetCursor(NULL);
		}

		if (GetAsyncKeyState(dwConsoleKey) & 0x1)
			console.get()->ToggleVisibility();

		if (GetAsyncKeyState(dwUnloadKey))
			bShouldRun = false;

		for (size_t i = 0; i < Features.size(); i++)
		{
			Features[i].get()->HandleKeys(); // Call the handle keys function for each feature

			// This is mostly outdated but is still useful for some things, using the ImGui::Hotkey function is better which is located in GUI/Custom.h
		}
	}

	DWORD __stdcall CheatThread(LPVOID lpParam)
	{
		hModule = reinterpret_cast<HMODULE>(lpParam); // Store the module handle which is used for unloading the module

		console.get()->SetVisibility(true); // Set the console to be visible if the framework is in debug mode

		if (!Init())
		{
			// If the initalization failed log an error and set the boolean to false to stop the cheat from running
			Utils::LogError(Utils::GetLocation(CurrentLoc), Cheat::Title + ": Failed to initalize");
			bShouldRun = false;
		}
		else // If the initalization was successful log that the cheat was initalized
			Utils::LogDebug(Utils::GetLocation(CurrentLoc), Cheat::Title + ": Initalized");

		while (bShouldRun) // the main process loop used to asynchonously run the features and handle the keys independently from the game
		{
			HandleKeys();

			for (size_t i = 0; i < Features.size(); i++)
			{
				Features[i].get()->Run();
			}

// If the thread sleep is enabled sleep for the specified amount of time
// This is used to reduce the CPU usage of the module, I would recommend keeping this enabled but added the option to disable it if needed for testing and when messing with less CPU intensive games
#if DO_THREAD_SLEEP
			std::this_thread::sleep_for(std::chrono::milliseconds(THREAD_SLEEP_TIME));
#endif
		}

		console.get()->SetVisibility(true); // Set the console to be visible when the cheat is unloading
		Utils::LogDebug(Utils::GetLocation(CurrentLoc), Cheat::Title + ": Unloading..."); // Log that the cheat is unloading

		#if FRAMEWORK_RENDER_D3D12 // If the framework is using D3D12 unbind the hooks and shutdown kiero, we do this here because the game might crash if we do it in the D3D12Hooks.cpp file like we do with D3D11
		// Destroy features first (disable their hooks before MH_Uninitialize)
		for (size_t i = 0; i < Features.size(); i++)
		{
			Features[i].get()->Destroy();
		}
		D3D12Release();
		kiero::shutdown();
		#endif

		// For D3D11, features are destroyed in hkPresent before kiero::shutdown
		// Wait for D3D11 cleanup to complete
		#if FRAMEWORK_RENDER_D3D11
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
		#endif

		console.get()->Destroy(); // Destroy/Free the console because if we don't the console window will stay open after the cheat is unloaded and can also cause a crash in rare cases

		std::this_thread::sleep_for(std::chrono::seconds(3)); // Sleep for 3 seconds to make sure the console is destroyed and that the hooks are released before unloading the module

		FreeLibraryAndExitThread(hModule, EXIT_SUCCESS); // Unload the module and exit the thread
		return TRUE; // Return true not sure if this is needed at all TBH but it's here
	}
}

// Simple and barebones DllMain to initalize the main thread
// Really the only thing that should be in DllMain is the thread creation
BOOL APIENTRY DllMain(HMODULE hModule, DWORD ulReasonForCall, LPVOID lpReserved)
{
	DisableThreadLibraryCalls(hModule); // Disable unwanted and unneeded thread calls

	if (ulReasonForCall != DLL_PROCESS_ATTACH)
		return TRUE;

	CreateThread(nullptr, NULL, Cheat::CheatThread, hModule, NULL, &Cheat::dwThreadID);

	return TRUE;
}
