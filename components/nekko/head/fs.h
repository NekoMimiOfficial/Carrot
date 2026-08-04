#pragma once
#include <filesystem>
#include <string>

std::filesystem::path expand_user_path(const std::string &path_str);
