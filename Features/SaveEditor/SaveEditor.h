#pragma once
#include "pch.h"

extern void* pSaveManagerInstance;

class SaveEditor : public Feature
{
private:
    bool Initalized = false;

    // Affinity modification
    int iAffinityLevel = 10;
    int iAffinityPoint = 99999;
    int iSelectedCharacter = 0; // 0=Daia, 1=Hikaru, 2=Seika

    void SaveGame()
    {
        if (pSaveManagerInstance == nullptr)
        {
            Utils::LogError(Utils::GetLocation(CurrentLoc), "SaveManager not found");
            return;
        }

        MonoMethod* Save = Mono::Instance().GetMethod("SaveManager", "Save", 0, "Assembly-CSharp", "MomottoCrash");
        if (Save != nullptr)
        {
            Mono::Instance().Invoke(Save, pSaveManagerInstance, nullptr);
            Utils::LogDebug(Utils::GetLocation(CurrentLoc), "Game Saved!");
        }
    }

    void ResetSaveData()
    {
        if (pSaveManagerInstance == nullptr) return;

        MonoMethod* ResetData = Mono::Instance().GetMethod("SaveManager", "ResetData", 0, "Assembly-CSharp", "MomottoCrash");
        if (ResetData != nullptr)
        {
            Mono::Instance().Invoke(ResetData, pSaveManagerInstance, nullptr);
            Utils::LogDebug(Utils::GetLocation(CurrentLoc), "Save Data Reset!");
        }
    }

    void ResetStoryData()
    {
        if (pSaveManagerInstance == nullptr) return;

        MonoMethod* ResetStory = Mono::Instance().GetMethod("SaveManager", "ResetStoryData", 0, "Assembly-CSharp", "MomottoCrash");
        if (ResetStory != nullptr)
        {
            Mono::Instance().Invoke(ResetStory, pSaveManagerInstance, nullptr);
            Utils::LogDebug(Utils::GetLocation(CurrentLoc), "Story Data Reset!");
        }
    }


    void ModifyAffinity(int characterIndex, int level, int point)
    {
        if (pSaveManagerInstance == nullptr) return;

        // Get SaveData from SaveManager
        MonoMethod* get_Current = Mono::Instance().GetMethod("SaveManager", "get_Current", 0, "Assembly-CSharp", "MomottoCrash");
        if (get_Current == nullptr) return;

        MonoObject* pSaveData = Mono::Instance().Invoke(get_Current, pSaveManagerInstance, nullptr);
        if (pSaveData == nullptr) return;

        // SaveData.CharacterAffinityList is at offset 0x068
        void* pAffinityList = *reinterpret_cast<void**>((uintptr_t)pSaveData + SaveDataOffsets::CharacterAffinityList);
        if (pAffinityList == nullptr) return;

        // List<T> internal array is at offset 0x10, count at 0x18
        void* pItems = *reinterpret_cast<void**>((uintptr_t)pAffinityList + 0x10);
        int count = *reinterpret_cast<int*>((uintptr_t)pAffinityList + 0x18);

        if (characterIndex >= count) return;

        // Get the SaveCharacterAffinity object
        void* pAffinity = *reinterpret_cast<void**>((uintptr_t)pItems + 0x20 + characterIndex * 8);
        if (pAffinity == nullptr) return;

        // Modify Level and Point
        int* pLevel = reinterpret_cast<int*>((uintptr_t)pAffinity + SaveCharacterAffinityOffsets::Level);
        int* pPoint = reinterpret_cast<int*>((uintptr_t)pAffinity + SaveCharacterAffinityOffsets::Point);
        int* pHavePeaceCount = reinterpret_cast<int*>((uintptr_t)pAffinity + SaveCharacterAffinityOffsets::HavePeaceCount);
        bool* pIsCheckNewPeace = reinterpret_cast<bool*>((uintptr_t)pAffinity + SaveCharacterAffinityOffsets::isCheckNewPeace);

        *pLevel = level;
        *pPoint = point;
        *pHavePeaceCount = 99; // Max puzzle pieces
        *pIsCheckNewPeace = false; // Disable new piece notification

        Utils::LogDebug(Utils::GetLocation(CurrentLoc), "Affinity modified for character " + std::to_string(characterIndex));
    }

    void UnlockAllContent()
    {
        if (pSaveManagerInstance == nullptr)
        {
            Utils::LogError(Utils::GetLocation(CurrentLoc), "SaveManager not found");
            return;
        }

        // Get SaveData
        MonoMethod* get_Current = Mono::Instance().GetMethod("SaveManager", "get_Current", 0, "Assembly-CSharp", "MomottoCrash");
        if (get_Current == nullptr) return;

        MonoObject* pSaveData = Mono::Instance().Invoke(get_Current, pSaveManagerInstance, nullptr);
        if (pSaveData == nullptr) return;

        // Max out all character affinity
        for (int i = 0; i < 3; i++)
        {
            ModifyAffinity(i, 10, 99999);
        }

        SaveGame();
        Utils::LogDebug(Utils::GetLocation(CurrentLoc), "All content unlocked via max affinity!");
    }

public:
    SaveEditor() {}

    virtual bool Setup()
    {
        Initalized = true;
        return true;
    }

    virtual void Destroy() {}
    virtual void HandleKeys() {}

    virtual void DrawMenuItems()
    {
        ImGui::BeginChild("##SaveManagement", ImVec2(g_ColWidth, g_RowHeight), ImGuiChildFlags_Border);
        {
            ImGui::Text(U8("存档管理"));
            ImGui::Separator();

            if (ImGui::Button(U8("保存游戏")))
                SaveGame();

            ImGui::SameLine();
            if (ImGui::Button(U8("重置剧情")))
                ResetStoryData();

            ImGui::SameLine();
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.2f, 0.2f, 1.0f));
            if (ImGui::Button(U8("全部重置")))
            {
                ImGui::OpenPopup("ConfirmReset");
            }
            ImGui::PopStyleColor();

            if (ImGui::BeginPopupModal("ConfirmReset", NULL, ImGuiWindowFlags_AlwaysAutoResize))
            {
                ImGui::Text(U8("确定要重置所有存档数据吗?"));
                ImGui::Separator();
                if (ImGui::Button(U8("是"), ImVec2(60, 0)))
                {
                    ResetSaveData();
                    ImGui::CloseCurrentPopup();
                }
                ImGui::SameLine();
                if (ImGui::Button(U8("否"), ImVec2(60, 0)))
                    ImGui::CloseCurrentPopup();
                ImGui::EndPopup();
            }

            ImGui::Separator();

            ImGui::Text(U8("角色好感度:"));
            const char* characters[] = { "Daia", "Hikaru", "Seika" };
            ImGui::Combo("##Char", &iSelectedCharacter, characters, IM_ARRAYSIZE(characters));

            ImGui::SliderInt(U8("等级"), &iAffinityLevel, 1, 10);
            ImGui::InputInt(U8("点数"), &iAffinityPoint);

            if (ImGui::Button(U8("设置好感度")))
            {
                ModifyAffinity(iSelectedCharacter, iAffinityLevel, iAffinityPoint);
                SaveGame();
            }

            ImGui::Separator();

            // Unlock buttons
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.6f, 0.2f, 1.0f));
            if (ImGui::Button(U8("满好感度")))
            {
                UnlockAllContent();
            }
            ImGui::PopStyleColor();

            ImGui::Separator();
            ImGui::Text(U8("存档管理器: %s"), pSaveManagerInstance ? "OK" : "---");
        }
        ImGui::EndChild();
    }

    virtual void Render() {}
    virtual void Run() {}
};
