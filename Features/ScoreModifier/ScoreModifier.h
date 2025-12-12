#pragma once
#include "pch.h"

extern void* pGameLogicInstance;

class ScoreModifier : public Feature
{
private:
    bool Initalized = false;

    // Score/Combo display
    int iCurrentScore = 0;
    int iCurrentCombo = 0;
    int iCurrentGauge = 0;
    int iMaxCombo = 0;

    // Judgment statistics
    int iPerfectCount = 0;
    int iGoodCount = 0;
    int iBadCount = 0;
    int iMissCount = 0;

    // Gauge modifier values
    int iGaugePerfect = 10;
    int iGaugeGood = 5;
    int iGaugeBad = -10;
    bool bCustomGauge = false;
    bool bCustomGaugeSet = false;

    // Score modifier
    int iSetScore = 999999;
    int iSetCombo = 9999;

    void ReadGameState()
    {
        if (pGameLogicInstance == nullptr) return;

        void* pScoreRp = *reinterpret_cast<void**>((uintptr_t)pGameLogicInstance + GameLogicOffsets::_scoreRp);
        void* pComboRp = *reinterpret_cast<void**>((uintptr_t)pGameLogicInstance + GameLogicOffsets::_comboRp);
        void* pGaugeRp = *reinterpret_cast<void**>((uintptr_t)pGameLogicInstance + GameLogicOffsets::_gaugeRp);

        if (pScoreRp != nullptr)
            iCurrentScore = *reinterpret_cast<int*>((uintptr_t)pScoreRp + 0x20);

        if (pComboRp != nullptr)
            iCurrentCombo = *reinterpret_cast<int*>((uintptr_t)pComboRp + 0x20);

        if (pGaugeRp != nullptr)
            iCurrentGauge = *reinterpret_cast<int*>((uintptr_t)pGaugeRp + 0x20);

        iMaxCombo = *reinterpret_cast<int*>((uintptr_t)pGameLogicInstance + GameLogicOffsets::_maxCombo);

        // Read judgment statistics from GameLogicNotesHelper (offset 0x090 in GameLogic)
        void* pNoteHelper = *reinterpret_cast<void**>((uintptr_t)pGameLogicInstance + 0x090);
        if (pNoteHelper != nullptr)
        {
            MonoMethod* getPerfect = Mono::Instance().GetMethod("GameLogicNotesHelper", "get_PerfectCount", 0, "Assembly-CSharp", "MomottoCrash");
            MonoMethod* getGood = Mono::Instance().GetMethod("GameLogicNotesHelper", "get_GoodCount", 0, "Assembly-CSharp", "MomottoCrash");
            MonoMethod* getBad = Mono::Instance().GetMethod("GameLogicNotesHelper", "get_BadCount", 0, "Assembly-CSharp", "MomottoCrash");
            MonoMethod* getMiss = Mono::Instance().GetMethod("GameLogicNotesHelper", "get_MissCount", 0, "Assembly-CSharp", "MomottoCrash");
            
            if (getPerfect != nullptr)
            {
                MonoObject* result = Mono::Instance().Invoke(getPerfect, pNoteHelper, nullptr);
                if (result != nullptr) iPerfectCount = *reinterpret_cast<int*>((uintptr_t)result + 0x10);
            }
            if (getGood != nullptr)
            {
                MonoObject* result = Mono::Instance().Invoke(getGood, pNoteHelper, nullptr);
                if (result != nullptr) iGoodCount = *reinterpret_cast<int*>((uintptr_t)result + 0x10);
            }
            if (getBad != nullptr)
            {
                MonoObject* result = Mono::Instance().Invoke(getBad, pNoteHelper, nullptr);
                if (result != nullptr) iBadCount = *reinterpret_cast<int*>((uintptr_t)result + 0x10);
            }
            if (getMiss != nullptr)
            {
                MonoObject* result = Mono::Instance().Invoke(getMiss, pNoteHelper, nullptr);
                if (result != nullptr) iMissCount = *reinterpret_cast<int*>((uintptr_t)result + 0x10);
            }
        }

        // Read current gauge values
        if (!bCustomGauge)
        {
            iGaugePerfect = *reinterpret_cast<int*>((uintptr_t)pGameLogicInstance + GameLogicOffsets::_gaugePerfectValue);
            iGaugeGood = *reinterpret_cast<int*>((uintptr_t)pGameLogicInstance + GameLogicOffsets::_gaugeGoodValue);
            iGaugeBad = *reinterpret_cast<int*>((uintptr_t)pGameLogicInstance + GameLogicOffsets::_gaugeBadValue);
        }
    }


    void SetScore(int score)
    {
        if (pGameLogicInstance == nullptr) return;
        void* pScoreRp = *reinterpret_cast<void**>((uintptr_t)pGameLogicInstance + GameLogicOffsets::_scoreRp);
        if (pScoreRp != nullptr)
        {
            *reinterpret_cast<int*>((uintptr_t)pScoreRp + 0x20) = score;
            Utils::LogDebug(Utils::GetLocation(CurrentLoc), "Score set to " + std::to_string(score));
        }
    }

    void SetCombo(int combo)
    {
        if (pGameLogicInstance == nullptr) return;
        void* pComboRp = *reinterpret_cast<void**>((uintptr_t)pGameLogicInstance + GameLogicOffsets::_comboRp);
        if (pComboRp != nullptr)
        {
            *reinterpret_cast<int*>((uintptr_t)pComboRp + 0x20) = combo;
            Utils::LogDebug(Utils::GetLocation(CurrentLoc), "Combo set to " + std::to_string(combo));
        }
    }

    void ApplyGaugeValues()
    {
        if (pGameLogicInstance == nullptr) return;
        *reinterpret_cast<int*>((uintptr_t)pGameLogicInstance + GameLogicOffsets::_gaugePerfectValue) = iGaugePerfect;
        *reinterpret_cast<int*>((uintptr_t)pGameLogicInstance + GameLogicOffsets::_gaugeGoodValue) = iGaugeGood;
        *reinterpret_cast<int*>((uintptr_t)pGameLogicInstance + GameLogicOffsets::_gaugeBadValue) = iGaugeBad;
        Utils::LogDebug(Utils::GetLocation(CurrentLoc), "Gauge values applied: P=" + std::to_string(iGaugePerfect) + " G=" + std::to_string(iGaugeGood) + " B=" + std::to_string(iGaugeBad));
    }

public:
    ScoreModifier() {};

    virtual bool Setup()
    {
        Initalized = true;
        return true;
    }

    virtual void Destroy() {}
    virtual void HandleKeys() {}

    virtual void DrawMenuItems()
    {
        if (!Initalized) return;

        ImGui::SameLine();
        ImGui::BeginChild("##ScoreInfo", ImVec2(g_ColWidth, g_RowHeight), ImGuiChildFlags_Border);
        {
            ImGui::Text(U8("游戏状态/修改"));
            ImGui::Separator();

            ImGui::Text(U8("分数: %d  连击: %d"), iCurrentScore, iCurrentCombo);
            ImGui::Text(U8("最大连击: %d  血条: %d"), iMaxCombo, iCurrentGauge);
            
            // Judgment statistics
            ImGui::TextColored(ImVec4(1.0f, 0.9f, 0.0f, 1.0f), "P:%d", iPerfectCount);
            ImGui::SameLine();
            ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.5f, 1.0f), "G:%d", iGoodCount);
            ImGui::SameLine();
            ImGui::TextColored(ImVec4(0.5f, 0.5f, 1.0f, 1.0f), "B:%d", iBadCount);
            ImGui::SameLine();
            ImGui::TextColored(ImVec4(1.0f, 0.3f, 0.3f, 1.0f), "M:%d", iMissCount);

            ImGui::Separator();

            // Score/Combo modification
            ImGui::SetNextItemWidth(80);
            ImGui::InputInt(U8("##SetScore"), &iSetScore, 0);
            ImGui::SameLine();
            if (ImGui::Button(U8("设置分数")))
                SetScore(iSetScore);

            ImGui::SetNextItemWidth(80);
            ImGui::InputInt(U8("##SetCombo"), &iSetCombo, 0);
            ImGui::SameLine();
            if (ImGui::Button(U8("设置连击")))
                SetCombo(iSetCombo);

            ImGui::Separator();

            // Gauge value modification
            ImGui::Checkbox(U8("自定义血条增减"), &bCustomGauge);
            if (bCustomGauge && pGameLogicInstance != nullptr)
            {
                ImGui::SetNextItemWidth(60);
                ImGui::InputInt("Perfect##g", &iGaugePerfect, 0);
                ImGui::SameLine();
                ImGui::SetNextItemWidth(60);
                ImGui::InputInt("Good##g", &iGaugeGood, 0);
                ImGui::SameLine();
                ImGui::SetNextItemWidth(60);
                ImGui::InputInt("Bad##g", &iGaugeBad, 0);

                if (ImGui::Button(U8("应用血条值")))
                    ApplyGaugeValues();
            }

            ImGui::Separator();
            ImGui::Text(U8("游戏逻辑: %s"), pGameLogicInstance ? "OK" : "---");
        }
        ImGui::EndChild();
    }

    virtual void Render() {}

    virtual void Run()
    {
        if (!Initalized) return;
        ReadGameState();

        // Custom Gauge toggle
        if (bCustomGauge && !bCustomGaugeSet)
        {
            bCustomGaugeSet = true;
            Utils::LogDebug(Utils::GetLocation(CurrentLoc), "Custom Gauge Enabled");
        }
        else if (!bCustomGauge && bCustomGaugeSet)
        {
            bCustomGaugeSet = false;
            Utils::LogDebug(Utils::GetLocation(CurrentLoc), "Custom Gauge Disabled");
        }

        // Keep custom gauge values applied
        if (bCustomGauge && pGameLogicInstance != nullptr)
            ApplyGaugeValues();
    }
};
