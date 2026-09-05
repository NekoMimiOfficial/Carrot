#pragma once
#include "value.h"
#include <memory>
#include <string>

std::string getVerString();
std::shared_ptr<NinArray> strSplit(std::string base, std::string denominator);

std::string valueToString(const Value &val);
bool isTruthy(const Value &val);
bool isEqual(const Value &a, const Value &b);
bool isInt(double d);
