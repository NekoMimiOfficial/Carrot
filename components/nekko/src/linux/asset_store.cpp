#include "asset_store.h"
#include <cstdlib>
#include <filesystem>
#include <vector>

namespace fs = std::filesystem;

std::variant<std::string, bool> getAsset(const std::string &rel_path) {
  std::vector<fs::path> search_paths;

  const char *home = std::getenv("HOME");
  if (home) {
    search_paths.push_back(fs::path(home) / ".local" / "lib" / "carrot");
  }

  search_paths.push_back(fs::path("/usr/lib/carrot"));

  for (const auto &base_dir : search_paths) {
    fs::path full_path = base_dir / rel_path;

    if (fs::exists(full_path) && fs::is_regular_file(full_path)) {
      return full_path.string();
    }
  }

  return false;
}
