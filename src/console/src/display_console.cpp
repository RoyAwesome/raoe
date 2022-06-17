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
#include "engine.hpp"
#include "imgui.h"
#include "spdlog/spdlog.h"
#include "misc/cpp/imgui_stdlib.h"
#include "string.hpp"

#include "console/console.hpp"
#include "rapidfuzz/fuzz.hpp"
#include <map>

namespace RAOE::Console
{
    const int32 Max_History = 32;
    const int32 Ringbuffer_Size = 40;

    DisplayConsole::DisplayConsole(RAOE::Engine& in_engine)
        : m_engine(in_engine)
        , history_pos(-1)
    {
        ring_buffer = std::make_shared<spdlog::sinks::ringbuffer_sink_mt>(Ringbuffer_Size);
        spdlog::default_logger()->sinks().push_back(ring_buffer);

        history.reserve(Max_History);

        input_buffer.reserve(128); //NOLINT Magic Number.  128 is a decent number, and we don't use it anywhere else
    }

    void DisplayConsole::Draw(const std::string& title, bool* p_open)
    {
        ImGui::SetNextWindowSize(ImVec2(520, 600), ImGuiCond_FirstUseEver);
        if(!ImGui::Begin(title.c_str(), p_open, ImGuiWindowFlags_NoBringToFrontOnFocus))
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
        ImGuiInputTextFlags input_text_flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CallbackCompletion | ImGuiInputTextFlags_CallbackHistory;
        if(ImGui::InputText("##Input", 
            &input_buffer, 
            input_text_flags, 
            [](ImGuiInputTextCallbackData* data) -> int 
            {
                auto* console = static_cast<DisplayConsole*>(data->UserData);
                switch(data->EventFlag)
                {
                case ImGuiInputTextFlags_CallbackHistory:
                    const int prev_history = console->history_pos;
                    int32& history_pos = console->history_pos;
                    if(data->EventKey == ImGuiKey_UpArrow)
                    {
                        if(history_pos == -1)
                        {
                            history_pos = (int32)console->history.size() - 1;
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
            raoe::string::trim(input_buffer);
            if(!input_buffer.empty())
            {
                ExecCommand(input_buffer);

                if(history.size() >= Max_History)
                {
                    history.erase(history.begin());
                }
                history.push_back(input_buffer);
                history_pos = -1;

                input_buffer = "";
            }          
            reclaim_focus = true;
        }

        //Text Helper
        //Shows either the closest matching console command to what is being typed
        //or the history.

        const bool show_input_tip = ImGui::GetIO().WantCaptureKeyboard;

        if(show_input_tip)
        {            
            const bool show_hint = history.size() > 0 || !input_buffer.empty(); //NOLINT TODO: Implement this

            std::vector<std::string> matching_possible_commands;
            //If the input buffer isn't empty, lets try fuzzy matching the input text to all of the possible commands
            if(!input_buffer.empty())
            {
                std::map<double, RAOE::Console::IConsoleElement*> ordered_command_match;
                for(const auto& command : RAOE::Console::CommandRegistry::Get().elements())
                {
                    double score = rapidfuzz::fuzz::partial_ratio(command->name(), input_buffer);
                    if(score > 51)
                    {
                        ordered_command_match.insert({score, command.get()});
                    }
                    
                }

                //Get the top 15 matches (or fewer, if there are less matches)
                auto back_inserter = std::back_inserter(matching_possible_commands);
                for(int32 i = 0; i < 15; i++)
                {
                    if(ordered_command_match.size() <= i)
                    {
                        break;
                    }
                    
                    auto element = ordered_command_match.begin();
                    for(int j = 0; j < i; j++)
                    {
                        element++;
                    }

                    *(back_inserter++) = fmt::format("{}", element->second->name());
                }
            }

            std::vector<std::string>& source_text = input_buffer.empty() ? history : matching_possible_commands;

            ImGuiWindowFlags popup_flags = ImGuiWindowFlags_NoDecoration 
                | ImGuiWindowFlags_AlwaysAutoResize 
                | ImGuiWindowFlags_NoSavedSettings
                | ImGuiWindowFlags_NoFocusOnAppearing
                | ImGuiWindowFlags_NoNav
                ;
            
            ImVec2 popup_pos = ImGui::GetItemRectMin();

            ImGui::SetNextWindowPos(popup_pos, ImGuiCond_Always, ImVec2(0, 1));
            ImGui::SetNextWindowBgAlpha(0.35f);
            static bool p_open = true;
            if(!source_text.empty() && ImGui::Begin("helper", &p_open, popup_flags))
            {    
                for(int32 i = 0; i < source_text.size(); i++)
                {
                    const std::string& str = source_text[i];
                    ImGui::PushID(i);
                    if(ImGui::Selectable(str.c_str()))
                    {
                        input_buffer = str;
                        p_open = false;
                    }
                    ImGui::PopID();
                }
                ImGui::End();
            }


        }    

        ImGui::SetItemDefaultFocus();
        if (reclaim_focus)
        {
            ImGui::SetKeyboardFocusHere(-1); // Auto focus previous widget
        }


        ImGui::End();
    }

    void DisplayConsole::ExecCommand(const std::string& command_line)    
    {
        spdlog::info("> {}", command_line);

        if(RAOE::Console::EConsoleError err = RAOE::Console::execute(engine(), command_line); err != RAOE::Console::EConsoleError::None)
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

