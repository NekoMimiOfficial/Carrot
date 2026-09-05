#pragma once
#include "value.h"
#include <string>

std::string getType(Value arg);
bool checkArgs(Value arg, std::string type);
