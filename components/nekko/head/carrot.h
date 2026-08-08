#pragma once
#include "value.h"
#include <string>

Value readValue(std::string var);
short writeValue(Value val, std::string var);
short initVar(std::string var);
short freeVar(std::string var);
