#include <filesystem>
#include <pwd.h>
#include <unistd.h>

namespace fs = std::filesystem;

fs::path expand_user_path(const std::string &path_str) {
  if (path_str.empty())
    return {};

  if (path_str[0] == '~') {
    std::string home;

    const char *home_env = std::getenv("HOME");
    if (home_env) {
      home = home_env;
    } else {
      struct passwd *pw = getpwuid(getuid());
      if (pw && pw->pw_dir) {
        home = pw->pw_dir;
      } else {
        throw std::runtime_error("Could not determine home directory.");
      }
    }

    if (path_str.size() == 1) {
      return fs::path(home);
    } else if (path_str[1] == '/') {
      return fs::path(home) / path_str.substr(2);
    }
  }

  return fs::path(path_str);
}
