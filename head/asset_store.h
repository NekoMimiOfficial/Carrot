#pragma once
#include <string>
#include <variant>

std::variant<std::string, bool> getAsset(const std::string& rel_path);
