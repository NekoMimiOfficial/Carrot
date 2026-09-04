#pragma once
#include "value.h"
#include <memory>
#include <string>

std::string getType(Value arg);
bool checkArgs(Value arg, std::string type);

std::string getVerString();
std::shared_ptr<NinArray> strSplit(std::string base, std::string denominator);
