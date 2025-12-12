#pragma once
#include "pch.h"

inline void* pSaveManagerInstance = nullptr;
inline void* pGameLogicInstance = nullptr;
inline void* pOnePlaySaveHolderInstance = nullptr;

inline void* SaveManager_Save = nullptr;
inline void* SaveManager_getCurrent = nullptr;
inline void* GameLogic_Miss = nullptr;
inline void* GameLogic_Good = nullptr;
inline void* GameLogic_Bad = nullptr;
inline void* OnePlaySaveHolder_ctor = nullptr;

inline bool bAllPerfectMode = false;

HOOK_DEF(void, SaveManager_Save, (void* __this))
{
    pSaveManagerInstance = __this;
    return oSaveManager_Save(__this);
}

HOOK_DEF(void*, SaveManager_getCurrent, (void* __this))
{
    pSaveManagerInstance = __this;
    return oSaveManager_getCurrent(__this);
}

HOOK_DEF(void, GameLogic_Miss, (void* __this))
{
    pGameLogicInstance = __this;
    return;
}

HOOK_DEF(void, GameLogic_Good, (void* __this))
{
    pGameLogicInstance = __this;
    if (bAllPerfectMode)
    {
        MonoMethod* Perfect = Mono::Instance().GetMethod("GameLogic", "Perfect", 0, "Assembly-CSharp", "MomottoCrash");
        if (Perfect != nullptr)
            Mono::Instance().Invoke(Perfect, __this, nullptr);
        return;
    }
    return oGameLogic_Good(__this);
}

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

    bool bAutoPlay = false;
    bool bAutoPlaySet = false;
    bool bInfiniteGauge = false;
    bool bInfiniteGaugeSet = false;

    bool bAllPerfectHookSet = false;
    bool bForceFullCombo = false;
    bool bForceFullComboSet = false;
    bool bSkipAffinityAnim = false;
    bool bSkipAffinityAnimSet = false;

    int iGaugeValue = 1000;

    void TryInitHooks()
    {
        if (bHooksInitialized || iHookRetryCount > 100) return;
        iHookRetryCount++;

        if (SaveManager_getCurrent == nullptr)
        {
            SaveManager_getCurrent = Mono::Instance().GetCompiledMethod("SaveManager", "get_Current", 0, "Assembly-CSharp", "MomottoCrash");
            if (SaveManager_getCurrent != nullptr)
            {
                Utils::LogHook(Utils::GetLocation(CurrentLoc), "SaveManager_getCurrent", "Info", "Found");
                CreateHook(SaveManager_getCurrent);
                EnableHook(SaveManager_getCurrent);
            }
        }

        if (SaveManager_Save == nullptr)
        {
            SaveManager_Save = Mono::Instance().GetCompiledMethod("SaveManager", "Save", 0, "Assembly-CSharp", "MomottoCrash");
            if (SaveManager_Save != nullptr)
            {
                Utils::LogHook(Utils::GetLocation(CurrentLoc), "SaveManager_Save", "Info", "Found");
                CreateHook(SaveManager_Save);
                EnableHook(SaveManager_Save);
            }
        }

        if (GameLogic_Miss == nullptr)
        {
            GameLogic_Miss = Mono::Instance().GetCompiledMethod("GameLogic", "Miss", 0, "Assembly-CSharp", "MomottoCrash");
            if (GameLogic_Miss != nullptr)
            {
                Utils::LogHook(Utils::GetLocation(CurrentLoc), "GameLogic_Miss", "Info", "Found");
                CreateHook(GameLogic_Miss);
            }
        }

        if (GameLogic_Good == nullptr)
        {
            GameLogic_Good = Mono::Instance().GetCompiledMethod("GameLogic", "Good", 0, "Assembly-CSharp", "MomottoCrash");
            if (GameLogic_Good != nullptr)
            {
                Utils::LogHook(Utils::GetLocation(CurrentLoc), "GameLogic_Good", "Info", "Found");
                CreateHook(GameLogic_Good);
            }
        }

        if (GameLogic_Bad == nullptr)
        {
            GameLogic_Bad = Mono::Instance().GetCompiledMethod("GameLogic", "Bad", 0, "Assembly-CSharp", "MomottoCrash");
            if (GameLogic_Bad != nullptr)
            {
                Utils::LogHook(Utils::GetLocation(CurrentLoc), "GameLogic_Bad", "Info", "Found");
                CreateHook(GameLogic_Bad);
            }
        }

        if (OnePlaySaveHolder_ctor == nullptr)
        {
            OnePlaySaveHolder_ctor = Mono::Instance().GetCompiledMethod("OnePlaySaveHolder", ".ctor", 0, "Assembly-CSharp", "MomottoCrash");
            if (OnePlaySaveHolder_ctor != nullptr)
            {
                Utils::LogHook(Utils::GetLocation(CurrentLoc), "OnePlaySaveHolder_ctor", "Info", "Found");
                CreateHook(OnePlaySaveHolder_ctor);
                EnableHook(OnePlaySaveHolder_ctor);
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

        ImGui::BeginChild("##GameCheats", ImVec2(g_ColWidth, g_RowHeight), ImGuiChildFlags_Border);
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

            ImGui::Checkbox(U8("无限血条"), &bInfiniteGauge);
            ImGui::Checkbox(U8("全Perfect"), &bAllPerfectMode);
            ImGui::Checkbox(U8("强制全连"), &bForceFullCombo);
            ImGui::Checkbox(U8("跳过好感度动画"), &bSkipAffinityAnim);

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
        if (!bHooksInitialized) TryInitHooks();

        if (bAutoPlay && !bAutoPlaySet && GameLogic_Miss != nullptr)
        {
            bAutoPlaySet = true;
            EnableHook(GameLogic_Miss);
            Utils::LogDebug(Utils::GetLocation(CurrentLoc), "Auto Play Enabled");
        }
        else if (!bAutoPlay && bAutoPlaySet && GameLogic_Miss != nullptr)
        {
            bAutoPlaySet = false;
            if (!bAllPerfectMode) DisableHook(GameLogic_Miss);
            Utils::LogDebug(Utils::GetLocation(CurrentLoc), "Auto Play Disabled");
        }

        if (bInfiniteGauge && !bInfiniteGaugeSet) { bInfiniteGaugeSet = true; Utils::LogDebug(Utils::GetLocation(CurrentLoc), "Infinite Gauge Enabled"); }
        else if (!bInfiniteGauge && bInfiniteGaugeSet) { bInfiniteGaugeSet = false; Utils::LogDebug(Utils::GetLocation(CurrentLoc), "Infinite Gauge Disabled"); }

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
            if (!bAutoPlay && GameLogic_Miss != nullptr) DisableHook(GameLogic_Miss);
            Utils::LogDebug(Utils::GetLocation(CurrentLoc), "All Perfect Disabled");
        }

        if (bForceFullCombo && !bForceFullComboSet) { bForceFullComboSet = true; Utils::LogDebug(Utils::GetLocation(CurrentLoc), "Force Full Combo Enabled"); }
        else if (!bForceFullCombo && bForceFullComboSet) { bForceFullComboSet = false; Utils::LogDebug(Utils::GetLocation(CurrentLoc), "Force Full Combo Disabled"); }

        if (bSkipAffinityAnim && !bSkipAffinityAnimSet) { bSkipAffinityAnimSet = true; Utils::LogDebug(Utils::GetLocation(CurrentLoc), "Skip Affinity Animation Enabled"); }
        else if (!bSkipAffinityAnim && bSkipAffinityAnimSet) { bSkipAffinityAnimSet = false; Utils::LogDebug(Utils::GetLocation(CurrentLoc), "Skip Affinity Animation Disabled"); }

        if (bForceFullCombo && pOnePlaySaveHolderInstance != nullptr)
        {
            int* pClearType = reinterpret_cast<int*>((uintptr_t)pOnePlaySaveHolderInstance + OnePlaySaveHolderOffsets::LatestMusicClearType);
            if (*pClearType < 3) *pClearType = 3;
        }

        if (pGameLogicInstance != nullptr)
        {
            if (pOnePlaySaveHolderInstance == nullptr)
                pOnePlaySaveHolderInstance = *reinterpret_cast<void**>((uintptr_t)pGameLogicInstance + 0x078);

            bool* pAuto = reinterpret_cast<bool*>((uintptr_t)pGameLogicInstance + GameLogicOffsets::DebugAutoPlayMode);
            if (*pAuto != bAutoPlay) *pAuto = bAutoPlay;
        }

        if (bInfiniteGauge && pGameLogicInstance != nullptr)
        {
            void* pGaugeRp = *reinterpret_cast<void**>((uintptr_t)pGameLogicInstance + GameLogicOffsets::_gaugeRp);
            if (pGaugeRp != nullptr)
            {
                int* pGaugeValue = reinterpret_cast<int*>((uintptr_t)pGaugeRp + 0x20);
                if (*pGaugeValue < 1000) *pGaugeValue = 1000;
            }
        }

        // Skip affinity animation by setting LatestAddCharacterAffinityPoint to 0
        if (bSkipAffinityAnim && pOnePlaySaveHolderInstance != nullptr)
        {
            int* pAffinityPoint = reinterpret_cast<int*>((uintptr_t)pOnePlaySaveHolderInstance + OnePlaySaveHolderOffsets::LatestAddCharacterAffinityPoint);
            if (*pAffinityPoint > 0) *pAffinityPoint = 0;
        }
    }
};
