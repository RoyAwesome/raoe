/*
Copyright 2022 Roy Awesome's Open Engine (RAOE)

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

#include "display_console.hpp"
#include "imgui.h"
#include "spdlog/spdlog.h"
#include "misc/cpp/imgui_stdlib.h"
#include "string.hpp"

namespace RAOE::Console
{
    DisplayConsole::DisplayConsole()
    {
        
    }

    void DisplayConsole::Draw(std::string title, bool* p_open)
    {
        ImGui::SetNextWindowSize(ImVec2(520, 600), ImGuiCond_FirstUseEver);
        if(!ImGui::Begin(title.c_str(), p_open))
        {
            ImGui::End();
            return;
        }

        if(ImGui::BeginPopupContextItem())
        {
            if(ImGui::MenuItem("Close"))
            {
                *p_open = false;
            }
            ImGui::EndPopup();
        }

        ImGui::Separator();

        const float footer_height_to_reserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
        ImGui::BeginChild("ScrollingRegion", ImVec2(0, -footer_height_to_reserve), false, ImGuiWindowFlags_HorizontalScrollbar);
        if(ImGui::BeginPopupContextWindow())
        {
            if(ImGui::Selectable("Clear"))
            {
                //Clear the console
            }
            ImGui::EndPopup();
        }

        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1));

        for(int i = 0; i < 100; i++)
        {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0, 0, 1, 1));
            ImGui::TextUnformatted("[Info] ");
            ImGui::PopStyleColor();
            ImGui::SameLine();
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 1, 1));
            ImGui::TextUnformatted(fmt::format("iteration {}", i).c_str());
            ImGui::PopStyleColor();
        }

        ImGui::PopStyleVar();
        ImGui::EndChild();

        ImGui::Separator();

        //Command Line
        bool reclaim_focus = false;

        std::string input_buffer;
        input_buffer.reserve(128);
        ImGuiInputTextFlags input_text_flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CallbackCompletion | ImGuiInputTextFlags_CallbackHistory;
        if(ImGui::InputText("Input", 
            &input_buffer, 
            input_text_flags, 
            [](ImGuiInputTextCallbackData* data) -> int 
            {
                return 0;
            }
            ,(void*)this)
        )
        {
            raoe::core::trim(input_buffer);
            if(!input_buffer.empty())
            {
                ExecCommand(input_buffer);
            }          
            reclaim_focus = true;
        }

        ImGui::SetItemDefaultFocus();
        if (reclaim_focus)
            ImGui::SetKeyboardFocusHere(-1); // Auto focus previous widget

        ImGui::End();
    }

    void DisplayConsole::ExecCommand(const std::string& command_line)    
    {
        spdlog::info("> {}", command_line);
    
    }
}

