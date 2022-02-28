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

#include "console/console.hpp"

namespace RAOE::Console
{
    const int32 Max_History = 32;

    DisplayConsole::DisplayConsole()
    {
        ring_buffer = std::make_shared<spdlog::sinks::ringbuffer_sink_mt>(40);
        spdlog::default_logger()->sinks().push_back(ring_buffer);

        history.reserve(Max_History);
        history_pos = -1;
    }

    DisplayConsole::~DisplayConsole()    
    {
        //Remove the ring buffer sink
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

        std::vector<std::string> log_messages = ring_buffer->last_formatted(40);
        for(const std::string& str : log_messages)
        {
            ImGui::TextUnformatted(str.c_str());
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
                DisplayConsole* console = static_cast<DisplayConsole*>(data->UserData);
                switch(data->EventFlag)
                {
                case ImGuiInputTextFlags_CallbackHistory:
                    const int prev_history = console->history_pos;
                    int32& history_pos = console->history_pos;
                    if(data->EventKey == ImGuiKey_UpArrow)
                    {
                        if(history_pos == -1)
                        {
                            history_pos = console->history.size() - 1;
                        }
                        else if(history_pos > 0)
                        {
                            history_pos--;
                        }
                    }
                    else if(data->EventKey == ImGuiKey_DownArrow)
                    {
                        if(history_pos != -1)
                        {
                            if(++history_pos >= console->history.size())
                            {
                                history_pos = -1;
                            }
                        }
                    }

                    if(prev_history != history_pos)
                    {
                        std::string history_str = history_pos >= 0 ? console->history[history_pos] : "";

                        data->DeleteChars(0, data->BufTextLen);
                        data->InsertChars(0, history_str.c_str());
                    }
                break;
                }
                return 0;
            }
            ,(void*)this)
        )
        {
            raoe::core::trim(input_buffer);
            if(!input_buffer.empty())
            {
                ExecCommand(input_buffer);

                if(history.size() >= Max_History)
                {
                    history.erase(history.begin());
                }
                history.push_back(input_buffer);
                history_pos = -1;
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

        if(RAOE::Console::EConsoleError err = RAOE::Console::execute(command_line); err != RAOE::Console::EConsoleError::None)
        {
            switch(err)
            {
                case RAOE::Console::EConsoleError::Command_Not_Found:
                spdlog::warn("> Command Not Found");
                break;
                case RAOE::Console::EConsoleError::Incorrect_Arguments:
                spdlog::warn("> Incorrect Arguments");
                break;
                case RAOE::Console::EConsoleError::Need_Cheats:
                spdlog::warn("> Cheats are not enabled");
                break;
                case RAOE::Console::EConsoleError::Not_Authorized:
                spdlog::warn("> You are not authorized to run this networked command");
                break;
                default:
                spdlog::warn("> Unknown Error");
                break;
            }
        }
    }
}

