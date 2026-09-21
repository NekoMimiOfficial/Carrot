#pragma once
#include "value.h"
#include <memory>
#include <string>

std::string getVerString();
std::shared_ptr<NinArray> strSplit(std::string base, std::string denominator);
void strReplace(std::string &base, const std::string &old, const std::string &new_w);
std::string strJoin(std::shared_ptr<NinArray> items, std::string delimiter);

std::string valueToString(const Value &val);
bool isTruthy(const Value &val);
bool isEqual(const Value &a, const Value &b);
bool isInt(double d);
