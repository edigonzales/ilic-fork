#pragma once

#include <string>
#include <string_view>

namespace util {

// Returns a stable public code for legacy semantic diagnostics that have not
// yet been migrated to an explicit source-site code.
std::string diagnosticCodeForMessage(std::string_view message);

}
