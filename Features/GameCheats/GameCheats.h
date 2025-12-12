#pragma once
#include "pch.h"

// MOMOCrash Global Instance Pointers
inline void* pSaveManagerInstance = nullptr;
inline void* pGameLogicInstance = nullptr;
inline void* pOnePlaySaveHolderInstance = nullptr;

// Hook target addresses (global for macro compatibility)
inline void* SaveManager_Save = nullptr;
inline void* SaveManager_getCurrent = nullptr;
inline void* GameLogic_Miss = nullptr;
inline void* GameLogic_Good = nullptr;
inline void* GameLogic_Bad = nullptr;
inline void* OnePlaySaveHolder_ctor = nullptr;

// All Perfect mode flag
inline bool bAllPerfectMode = false;

// Hook: SaveManager.Save - Capture SaveManager Instance
HOOK_DEF(void, SaveManager_Save, (void* __this))
{
    pSaveManagerInstance = __this;
    return oSaveManager_Save(__this);
}

// Hook: SaveManager.get_Current - Capture SaveManager Instance
HOOK_DEF(void*, SaveManager_getCurrent, (void* __this))
{
    pSaveManagerInstance = __this;
    return oSaveManager_getCurrent(__this);
}

// Hook: GameLogic.Miss - Block Miss (No Miss feature)
HOOK_DEF(void, GameLogic_Miss, (void* __this))
{
    pGameLogicInstance = __this;
    return;
}

// Hook: GameLogic.Good - Convert to Perfect when All Perfect mode
HOOK_DEF(void, GameLogic_Good, (void* __this))
{
    pGameLogicInstance = __this;
    if (bAllPerfectMode)
    {
        // Call Perfect instead of Good
        MonoMethod* Perfect = Mono::Instance().GetMethod("GameLogic", "Perfect", 0, "Assembly-CSharp", "MomottoCrash");
        if (Perfect != nullptr)
            Mono::Instance().Invoke(Perfect, __this, nullptr);
        return;
    }
    return oGameLogic_Good(__this);
}

// Hook: GameLogic.Bad - Convert to Perfect when All Perfect mode
HOOK_DEF(void, GameLogic_Bad, (void* __this))
{
    pGameLogicInstance = __this;
    if (bAllPerfectMode)
    {
        MonoMethod* Perfect = Mono::Instance().GetMethod("GameLogic", "Perfect", 0, "Assembly-CSharp", "MomottoCrash");
        if (Perfect != nullptr)
            Mono::Instance().Invoke(Perfect, __this, nullptr);
        return;
    }
    return oGameLogic_Bad(__this);
}

// Hook: OnePlaySaveHolder.ctor - Capture OnePlaySaveHolder Instance
HOOK_DEF(void, OnePlaySaveHolder_ctor, (void* __this))
{
    pOnePlaySaveHolderInstance = __this;
    return oOnePlaySaveHolder_ctor(__this);
}

class GameCheats : public Feature
{
private:
    bool Initalized = false;
    bool bHooksInitialized = false;
    int iHookRetryCount = 0;

    // Feature Toggles
    bool bAutoPlay = false;
    bool bAutoPlaySet = false;
    bool bNoMiss = false;
    bool bNoMissHookSet = false;
    bool bInfiniteGauge = false;
    bool bInfiniteGaugeSet = false;
    bool bSkipResult = false;
    bool bSkipResultSet = false;
    bool bAllPerfectHookSet = false;
    bool bForceFullCombo = false;
    bool bForceFullComboSet = false;
    bool bSkipAffinityAnim = false;
    bool bSkipAffinityAnimSet = false;

    // Values
    int iGaugeValue = 1000;

    void TryInitHooks()
    {
        if (bHooksInitialized || iHookRetryCount > 100)
            return;

        iHookRetryCount++;

        if (SaveManager_getCurrent == nullptr)
        {
            SaveManager_getCurrent = Mono::Instance().GetCompiledMethod("SaveManager", "get_Current", 0, "Assembly-CSharp", "MomottoCrash");
            if (SaveManager_getCurrent != nullptr)
            {
                Utils::LogHook(Utils::GetLocation(CurrentLoc), "SaveManager_getCurrent", "Info", "SaveManager.get_Current found");
                CreateHook(SaveManager_getCurrent);
                Utils::LogHook(Utils::GetLocation(CurrentLoc), "SaveManager_getCurrent", "Create", "Hook Created");
                EnableHook(SaveManager_getCurrent);
                Utils::LogHook(Utils::GetLocation(CurrentLoc), "SaveManager_getCurrent", "Enable", "Hook Enabled");
            }
        }

        if (SaveManager_Save == nullptr)
        {
            SaveManager_Save = Mono::Instance().GetCompiledMethod("SaveManager", "Save", 0, "Assembly-CSharp", "MomottoCrash");
            if (SaveManager_Save != nullptr)
            {
                Utils::LogHook(Utils::GetLocation(CurrentLoc), "SaveManager_Save", "Info", "SaveManager.Save found");
                CreateHook(SaveManager_Save);
                Utils::LogHook(Utils::GetLocation(CurrentLoc), "SaveManager_Save", "Create", "Hook Created");
                EnableHook(SaveManager_Save);
                Utils::LogHook(Utils::GetLocation(CurrentLoc), "SaveManager_Save", "Enable", "Hook Enabled");
            }
        }

        if (GameLogic_Miss == nullptr)
        {
            GameLogic_Miss = Mono::Instance().GetCompiledMethod("GameLogic", "Miss", 0, "Assembly-CSharp", "MomottoCrash");
            if (GameLogic_Miss != nullptr)
            {
                Utils::LogHook(Utils::GetLocation(CurrentLoc), "GameLogic_Miss", "Info", "GameLogic.Miss found");
                CreateHook(GameLogic_Miss);
                Utils::LogHook(Utils::GetLocation(CurrentLoc), "GameLogic_Miss", "Create", "Hook Created");
            }
        }

        if (GameLogic_Good == nullptr)
        {
            GameLogic_Good = Mono::Instance().GetCompiledMethod("GameLogic", "Good", 0, "Assembly-CSharp", "MomottoCrash");
            if (GameLogic_Good != nullptr)
            {
                Utils::LogHook(Utils::GetLocation(CurrentLoc), "GameLogic_Good", "Info", "GameLogic.Good found");
                CreateHook(GameLogic_Good);
                Utils::LogHook(Utils::GetLocation(CurrentLoc), "GameLogic_Good", "Create", "Hook Created");
            }
        }

        if (GameLogic_Bad == nullptr)
        {
            GameLogic_Bad = Mono::Instance().GetCompiledMethod("GameLogic", "Bad", 0, "Assembly-CSharp", "MomottoCrash");
            if (GameLogic_Bad != nullptr)
            {
                Utils::LogHook(Utils::GetLocation(CurrentLoc), "GameLogic_Bad", "Info", "GameLogic.Bad found");
                CreateHook(GameLogic_Bad);
                Utils::LogHook(Utils::GetLocation(CurrentLoc), "GameLogic_Bad", "Create", "Hook Created");
            }
        }

        if (OnePlaySaveHolder_ctor == nullptr)
        {
            OnePlaySaveHolder_ctor = Mono::Instance().GetCompiledMethod("OnePlaySaveHolder", ".ctor", 0, "Assembly-CSharp", "MomottoCrash");
            if (OnePlaySaveHolder_ctor != nullptr)
            {
                Utils::LogHook(Utils::GetLocation(CurrentLoc), "OnePlaySaveHolder_ctor", "Info", "OnePlaySaveHolder.ctor found");
                CreateHook(OnePlaySaveHolder_ctor);
                Utils::LogHook(Utils::GetLocation(CurrentLoc), "OnePlaySaveHolder_ctor", "Create", "Hook Created");
                EnableHook(OnePlaySaveHolder_ctor);
                Utils::LogHook(Utils::GetLocation(CurrentLoc), "OnePlaySaveHolder_ctor", "Enable", "Hook Enabled");
            }
        }

        if (SaveManager_getCurrent != nullptr && GameLogic_Miss != nullptr)
        {
            bHooksInitialized = true;
            Utils::LogDebug(Utils::GetLocation(CurrentLoc), "All hooks initialized");
        }
    }

public:
    GameCheats() {};

    virtual bool Setup()
    {
        TryInitHooks();
        Initalized = true;
        return true;
    }

    virtual void Destroy()
    {
        if (!Initalized) return;
        if (SaveManager_getCurrent != nullptr) DisableHook(SaveManager_getCurrent);
        if (SaveManager_Save != nullptr) DisableHook(SaveManager_Save);
        if (GameLogic_Miss != nullptr) DisableHook(GameLogic_Miss);
        if (GameLogic_Good != nullptr) DisableHook(GameLogic_Good);
        if (GameLogic_Bad != nullptr) DisableHook(GameLogic_Bad);
        if (OnePlaySaveHolder_ctor != nullptr) DisableHook(OnePlaySaveHolder_ctor);
    }

    virtual void HandleKeys() {}

    virtual void DrawMenuItems()
    {
        if (!Initalized) return;

        float colWidth = (GUI::WIDTH - 30) / 2.0f;
        float rowHeight = (GUI::HEIGHT - 60) / 2.0f;

        ImGui::BeginChild("##GameCheats", ImVec2(colWidth, rowHeight), ImGuiChildFlags_Border);
        {
            ImGui::Text(U8("游戏作弊"));
            ImGui::Separator();

            if (ImGui::Checkbox(U8("自动游玩"), &bAutoPlay))
            {
                if (pGameLogicInstance != nullptr)
                {
                    bool* pAuto = reinterpret_cast<bool*>((uintptr_t)pGameLogicInstance + GameLogicOffsets::DebugAutoPlayMode);
                    *pAuto = bAutoPlay;
                }
            }

            ImGui::Checkbox(U8("无Miss"), &bNoMiss);
            ImGui::Checkbox(U8("无限血条"), &bInfiniteGauge);
            ImGui::Checkbox(U8("全Perfect"), &bAllPerfectMode);
            ImGui::Checkbox(U8("强制全连"), &bForceFullCombo);
            ImGui::Checkbox(U8("跳过升级动画"), &bSkipAffinityAnim);
            
            if (ImGui::Checkbox(U8("跳过结算"), &bSkipResult))
            {
                if (pOnePlaySaveHolderInstance != nullptr)
                {
                    bool* pSkip = reinterpret_cast<bool*>((uintptr_t)pOnePlaySaveHolderInstance + OnePlaySaveHolderOffsets::DebugForceResultSkip);
                    *pSkip = bSkipResult;
                }
            }

            ImGui::Separator();
            ImGui::Text(U8("血条值:"));
            ImGui::SetNextItemWidth(-1);
            ImGui::SliderInt("##Gauge", &iGaugeValue, 0, 1000);
            if (ImGui::Button(U8("设置血条")))
            {
                if (pGameLogicInstance != nullptr)
                {
                    MonoMethod* SetGauge = Mono::Instance().GetMethod("GameLogic", "SetGauge", 1, "Assembly-CSharp", "MomottoCrash");
                    if (SetGauge != nullptr)
                    {
                        void* args[1] = { &iGaugeValue };
                        Mono::Instance().Invoke(SetGauge, pGameLogicInstance, args);
                    }
                }
            }

            ImGui::Separator();
            ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), U8("SM:%s GL:%s OP:%s"), 
                pSaveManagerInstance ? "OK" : "--",
                pGameLogicInstance ? "OK" : "--",
                pOnePlaySaveHolderInstance ? "OK" : "--");
        }
        ImGui::EndChild();
    }

    virtual void Render() {}

    virtual void Run()
    {
        if (!Initalized) return;

        if (!bHooksInitialized)
            TryInitHooks();

        // Auto Play toggle
        if (bAutoPlay && !bAutoPlaySet)
        {
            bAutoPlaySet = true;
            Utils::LogDebug(Utils::GetLocation(CurrentLoc), "Auto Play Enabled");
        }
        else if (!bAutoPlay && bAutoPlaySet)
        {
            bAutoPlaySet = false;
            Utils::LogDebug(Utils::GetLocation(CurrentLoc), "Auto Play Disabled");
        }

        // No Miss toggle
        if (bNoMiss && !bNoMissHookSet && GameLogic_Miss != nullptr)
        {
            bNoMissHookSet = true;
            EnableHook(GameLogic_Miss);
            Utils::LogDebug(Utils::GetLocation(CurrentLoc), "No Miss Enabled");
        }
        else if (!bNoMiss && bNoMissHookSet && GameLogic_Miss != nullptr)
        {
            bNoMissHookSet = false;
            DisableHook(GameLogic_Miss);
            Utils::LogDebug(Utils::GetLocation(CurrentLoc), "No Miss Disabled");
        }

        // Infinite Gauge toggle
        if (bInfiniteGauge && !bInfiniteGaugeSet)
        {
            bInfiniteGaugeSet = true;
            Utils::LogDebug(Utils::GetLocation(CurrentLoc), "Infinite Gauge Enabled");
        }
        else if (!bInfiniteGauge && bInfiniteGaugeSet)
        {
            bInfiniteGaugeSet = false;
            Utils::LogDebug(Utils::GetLocation(CurrentLoc), "Infinite Gauge Disabled");
        }

        // All Perfect mode toggle
        if (bAllPerfectMode && !bAllPerfectHookSet)
        {
            bAllPerfectHookSet = true;
            if (GameLogic_Good != nullptr) EnableHook(GameLogic_Good);
            if (GameLogic_Bad != nullptr) EnableHook(GameLogic_Bad);
            if (GameLogic_Miss != nullptr) EnableHook(GameLogic_Miss);
            Utils::LogDebug(Utils::GetLocation(CurrentLoc), "All Perfect Enabled");
        }
        else if (!bAllPerfectMode && bAllPerfectHookSet)
        {
            bAllPerfectHookSet = false;
            if (GameLogic_Good != nullptr) DisableHook(GameLogic_Good);
            if (GameLogic_Bad != nullptr) DisableHook(GameLogic_Bad);
            if (!bNoMiss && GameLogic_Miss != nullptr) DisableHook(GameLogic_Miss);
            Utils::LogDebug(Utils::GetLocation(CurrentLoc), "All Perfect Disabled");
        }

        // Force Full Combo toggle
        if (bForceFullCombo && !bForceFullComboSet)
        {
            bForceFullComboSet = true;
            Utils::LogDebug(Utils::GetLocation(CurrentLoc), "Force Full Combo Enabled");
        }
        else if (!bForceFullCombo && bForceFullComboSet)
        {
            bForceFullComboSet = false;
            Utils::LogDebug(Utils::GetLocation(CurrentLoc), "Force Full Combo Disabled");
        }

        // Skip Affinity Animation toggle
        if (bSkipAffinityAnim && !bSkipAffinityAnimSet)
        {
            bSkipAffinityAnimSet = true;
            Utils::LogDebug(Utils::GetLocation(CurrentLoc), "Skip Affinity Animation Enabled");
        }
        else if (!bSkipAffinityAnim && bSkipAffinityAnimSet)
        {
            bSkipAffinityAnimSet = false;
            Utils::LogDebug(Utils::GetLocation(CurrentLoc), "Skip Affinity Animation Disabled");
        }

        // Skip Result toggle
        if (bSkipResult && !bSkipResultSet)
        {
            bSkipResultSet = true;
            Utils::LogDebug(Utils::GetLocation(CurrentLoc), "Skip Result Enabled");
        }
        else if (!bSkipResult && bSkipResultSet)
        {
            bSkipResultSet = false;
            Utils::LogDebug(Utils::GetLocation(CurrentLoc), "Skip Result Disabled");
        }

        // Force Full Combo - set clear type
        if (bForceFullCombo && pOnePlaySaveHolderInstance != nullptr)
        {
            int* pClearType = reinterpret_cast<int*>((uintptr_t)pOnePlaySaveHolderInstance + OnePlaySaveHolderOffsets::LatestMusicClearType);
            if (*pClearType < 3)
                *pClearType = 3;
        }

        if (pGameLogicInstance != nullptr)
        {
            if (pOnePlaySaveHolderInstance == nullptr)
            {
                pOnePlaySaveHolderInstance = *reinterpret_cast<void**>((uintptr_t)pGameLogicInstance + 0x078);
            }

            bool* pAuto = reinterpret_cast<bool*>((uintptr_t)pGameLogicInstance + GameLogicOffsets::DebugAutoPlayMode);
            if (*pAuto != bAutoPlay)
                *pAuto = bAutoPlay;
        }

        if (bInfiniteGauge && pGameLogicInstance != nullptr)
        {
            void* pGaugeRp = *reinterpret_cast<void**>((uintptr_t)pGameLogicInstance + GameLogicOffsets::_gaugeRp);
            if (pGaugeRp != nullptr)
            {
                int* pGaugeValue = reinterpret_cast<int*>((uintptr_t)pGaugeRp + 0x20);
                if (*pGaugeValue < 1000)
                    *pGaugeValue = 1000;
            }
        }

        if (pOnePlaySaveHolderInstance != nullptr)
        {
            bool* pSkip = reinterpret_cast<bool*>((uintptr_t)pOnePlaySaveHolderInstance + OnePlaySaveHolderOffsets::DebugForceResultSkip);
            if (*pSkip != bSkipResult)
                *pSkip = bSkipResult;

            if (bSkipAffinityAnim)
            {
                int* pAffinityPoint = reinterpret_cast<int*>((uintptr_t)pOnePlaySaveHolderInstance + OnePlaySaveHolderOffsets::LatestAddCharacterAffinityPoint);
                if (*pAffinityPoint > 0)
                    *pAffinityPoint = 0;
            }
        }
    }
};
