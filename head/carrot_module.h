#pragma once
#include "value.h"

extern "C" { void carrot_module_init(std::unordered_map<std::string, Value> *out); }
