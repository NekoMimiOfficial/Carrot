#pragma once
#include <cstdint>
#include <string>
#include <variant>
#include <vector>

enum FileType {
  STRING,
  BYTE_ARR,
};

std::variant<std::string, std::vector<uint16_t>> read(std::string file, FileType type);
short write(std::string file, std::variant<std::string, std::vector<uint16_t>>);
