#include "lang.h"
#include "lson.h"
#include <cstdlib>
#include <filesystem>
#include <sstream>
#include <string>

static LsonTranslator g_translator;
static bool g_localeLoaded = false;
static std::string err_msg;

static const char *DEFAULT_LOCALE = "carrie";

static const char *LOC_ID_NAMES[] = {
#define X(name) #name,
    LOC_ID_LIST
#undef X
};

static bool tryLoadLocale(const std::string &name) {
  namespace fs = std::filesystem;

  const char *home = std::getenv("HOME");
  if (home) {
    fs::path localPath =
        fs::path(home) / ".local/lib/carrot/locale" / (name + ".json");
    if (fs::exists(localPath) && g_translator.loadLanguage(localPath.string()))
      return true;
  }

  const char *appdir_env = std::getenv("APPDIR");
  if (appdir_env != nullptr) {
    fs::path appimgPath =
        fs::path(appdir_env) / "usr/lib/carrot/locale" / (name + ".json");
    if (fs::exists(appimgPath) &&
        g_translator.loadLanguage(appimgPath.string()))
      return true;
  }

  fs::path globalPath = fs::path("/usr/lib/carrot/locale") / (name + ".json");
  if (fs::exists(globalPath) && g_translator.loadLanguage(globalPath.string()))
    return true;

  return false;
}

void initLocale() {
  const char *envLocale = std::getenv("CARROT_LOCALE");

  if (envLocale && tryLoadLocale(envLocale)) {
    g_localeLoaded = true;
    return;
  }

  if (tryLoadLocale(DEFAULT_LOCALE)) {
    g_localeLoaded = true;
    return;
  }

  std::stringstream ss;
  ss << "Could not load a locale file (tried '"
     << (envLocale ? envLocale : DEFAULT_LOCALE) << "' and fallback '"
     << DEFAULT_LOCALE
     << "' under ~/.local/lib/carrot/locale and /usr/lib/carrot/locale). "
     << "All messages will be empty until this is fixed.\n";

  err_msg = ss.str();
  g_localeLoaded = false;
}

static std::string substitute(const std::string &fmt,
                              const std::vector<std::string> &args) {
  std::string result;
  result.reserve(fmt.size());

  for (size_t i = 0; i < fmt.size();) {
    if (fmt[i] == '{') {
      size_t j = i + 1;
      size_t numStart = j;
      while (j < fmt.size() && isdigit((unsigned char)fmt[j]))
        j++;
      if (j > numStart && j < fmt.size() && fmt[j] == '}') {
        int idx = std::stoi(fmt.substr(numStart, j - numStart));
        if (idx >= 0 && (size_t)idx < args.size())
          result += args[idx];
        i = j + 1;
        continue;
      }
    }
    result += fmt[i++];
  }
  return result;
}

std::string LOC_Internal(int id, const std::vector<std::string> &args) {
  if (!g_localeLoaded || id < 0 || id >= LOC_ID_COUNT)
    return err_msg;
  return substitute(g_translator.tr(LOC_ID_NAMES[id]), args);
}

std::string LOC_Internal(const std::string &key,
                         const std::vector<std::string> &args) {
  if (!g_localeLoaded)
    return err_msg;
  return substitute(g_translator.tr(key), args);
}
