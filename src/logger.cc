#include "logger.h"

#include <vector>
#include <cstdio>

#include "imgui.h"

struct LogMessage {
    std::string message;
    double duration;
    logger::eLoggingLevel level;
};

static std::vector<LogMessage> sg_log_messages;

void logger::log_debug(const std::string& message, eLoggingLevel level, double duration) {
    LogMessage new_message = {};
    new_message.message = message;
    new_message.duration = duration;
    new_message.level = level;
    sg_log_messages.push_back(new_message);

    const char* prefix = "";
    switch (level) {
        case eLoggingLevel::kInfo:      prefix = "[INFO]"; break;
        case eLoggingLevel::kWarning:   prefix = "[WARNING]"; break;
        case eLoggingLevel::kError:     prefix = "[ERROR]"; break;
    }

    printf("%s %s\n", prefix, message.c_str());
}

/*
    Prints the message to a console
*/
void logger::print_messages(float delta_time) {
    ImGui::SetNextWindowSize(ImVec2(400, 300), ImGuiCond_Always);
    ImGui::Begin("Console: ");

    for (int i = sg_log_messages.size() - 1; i >= 0; i--) {
        auto& msg = sg_log_messages[i];
        ImVec4 color;
        std::string prefix = "";
        switch (msg.level) {
            case eLoggingLevel::kInfo: {
                color = ImVec4(1, 1, 1, 1);
                prefix = "[INFO] ";
            } break;
            case eLoggingLevel::kWarning: {
                color = ImVec4(1, 1, 0, 1);
                prefix = "[WARNING] ";
            } break;
            case eLoggingLevel::kError: {
                color = ImVec4(1, 0, 0, 1);
                prefix = "[ERROR] ";
            } break;
        }

        prefix += msg.message + "\n";
        ImGui::PushStyleColor(ImGuiCol_Text, color);
        ImGui::TextUnformatted(prefix.c_str());
        ImGui::PopStyleColor();

        msg.duration -= delta_time;
        if (msg.duration <= 0.0) {
            sg_log_messages.erase(sg_log_messages.begin() + i);
        }
    }

    ImGui::End();
}

void logger::write_text_at(const std::string& message, float x, float y, float r, float g, float b) {
    ImDrawList* draw_list = ImGui::GetBackgroundDrawList();
    draw_list->AddText(ImVec2(x, y), IM_COL32(r, g, b, 255), message.c_str());
}

/*
ImGui Examples
ImGui::Begin("Debug Window");

ImGui::Text("Delta Time: %.3f ms", delta_time * 1000.0);
ImGui::Text("FPS: %d", (int)(1.0 / delta_time));

// You can add checkboxes, sliders, etc. too
// ImGui::Checkbox("Wireframe Mode", &some_debug_bool);

ImGui::End();

// LOG
ImDrawList* draw_list = ImGui::GetBackgroundDrawList();
draw_list->AddText(ImVec2(10, 10), IM_COL32(255, 255, 0, 255), "Hello from the top-left!");
*/