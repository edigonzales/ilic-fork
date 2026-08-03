#pragma once

#include "../../include/ilic/Compiler.h"
#include "Json.h"

namespace ilic::capi {

CompilationRequest decodeCompilationRequest(const json::Value &json);

} // namespace ilic::capi
