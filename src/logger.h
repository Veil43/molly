#pragma once

#include <string>

namespace logger 
{

enum class eLoggingLevel {
    kInfo,
    kWarning,
    kError,
};

void log_debug(const std::string& message, eLoggingLevel level = eLoggingLevel::kError, double duration = 1.0);
void print_messages(float delta_time);
void write_text_at(const std::string& message, float x, float y);
};