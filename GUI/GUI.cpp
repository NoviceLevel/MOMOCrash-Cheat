#include "pch.h"
#include "Watermark.h"

bool bWatermark = true;
bool bWatermarkFPS = true;

void GUI::Render()
{
	if (bWatermark)
		showWatermark(bWatermarkFPS, Cheat::Title.c_str(), ImVec4(255, 255, 255, 255), ImVec4(255, 255, 255, 0));

	if (bMenuOpen)
	{
		ImGui::SetNextWindowSize(ImVec2(WIDTH, HEIGHT));
		ImGui::Begin(Cheat::Title.c_str(), NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse);

		float columnWidth = ImGui::GetContentRegionAvail().x / 2.0f - 5.0f;
		float rowHeight = ImGui::GetContentRegionAvail().y / 2.0f - 5.0f;

		ImGui::BeginChild("##Cheat", ImVec2(columnWidth, rowHeight), ImGuiChildFlags_Border);
		{
			ImGui::Text(U8("作弊设置"));
			ImGui::Separator();

			if (ImGui::Button(U8("卸载")))
				Cheat::bShouldRun = false;
			ImGui::SameLine();
			if (ImGui::Button(Cheat::console.get()->GetVisibility() ? U8("隐藏控制台") : U8("显示控制台")))
				Cheat::console.get()->ToggleVisibility();

			ImGui::Checkbox(U8("水印"), &bWatermark);
			if (bWatermark)
				ImGui::Checkbox(U8("显示FPS"), &bWatermarkFPS);
		}
		ImGui::EndChild();

		ImGui::SameLine();

		for (size_t i = 0; i < Features.size(); i++)
		{
			Features[i].get()->DrawMenuItems();
		}

		ImGui::End();
	}

#if FRAMEWORK_UNREAL
	auto pUnreal = Cheat::unreal.get();
	pUnreal->RefreshActorList();
#endif

	for (size_t i = 0; i < Features.size(); i++)
	{
		Features[i].get()->Render();
	}
}
