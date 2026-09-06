#include <fstream>
#include <string>
#include <unordered_map>

// I present to you my work of f-art (fine art, trust me)
// a specialized version of json for locale, thus locale json or lson
// the syntax is simple, {"key" : "value", ...} and nothing else
// no nesting {} no bools or number, just strings and that's it
// it gets the job done so why not :3c

class LsonTranslator {
private:
  std::unordered_map<std::string, std::string> dictionary;
  std::string lastError;

  int getLineNumber(const std::string &text, size_t pos) const {
    int line = 1;
    for (size_t i = 0; i < pos && i < text.size(); ++i) {
      if (text[i] == '\n')
        line++;
    }
    return line;
  }

  void setError(const std::string &lson, size_t pos, const std::string &msg) {
    lastError = "LSON Parse Error (Line " +
                std::to_string(getLineNumber(lson, pos)) + "): " + msg;
  }

  std::string parseLsonString(const std::string &lson, size_t &pos) {
    std::string result;
    pos++;

    while (pos < lson.size()) {
      if (lson[pos] == '\\') {
        pos++;
        if (pos < lson.size()) {
          switch (lson[pos]) {
          case '"':
            result += '"';
            break;
          case '\\':
            result += '\\';
            break;
          case '/':
            result += '/';
            break;
          case 'b':
            result += '\b';
            break;
          case 'f':
            result += '\f';
            break;
          case 'n':
            result += '\n';
            break;
          case 'r':
            result += '\r';
            break;
          case 't':
            result += '\t';
            break;
          default:
            result += lson[pos];
            break;
          }
          pos++;
        }
      } else if (lson[pos] == '"') {
        pos++;
        return result;
      } else {
        result += lson[pos++];
      }
    }

    setError(lson, pos, "Unterminated string");
    return "";
  }

  void skipWhitespace(const std::string &lson, size_t &pos) {
    while (pos < lson.size() && (lson[pos] == ' ' || lson[pos] == '\t' ||
                                 lson[pos] == '\n' || lson[pos] == '\r')) {
      pos++;
    }
  }

public:
  bool loadLanguage(const std::string &filepath) {
    dictionary.clear();
    lastError.clear();

    std::ifstream file(filepath,
                       std::ios::in | std::ios::binary | std::ios::ate);
    if (!file) {
      lastError = "Failed to open file: " + filepath;
      return false;
    }

    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::string lson(size, '\0');
    if (!file.read(&lson[0], size)) {
      lastError = "Failed to read file: " + filepath;
      return false;
    }

    size_t pos = 0;

    if (lson.size() >= 3 && static_cast<unsigned char>(lson[0]) == 0xEF &&
        static_cast<unsigned char>(lson[1]) == 0xBB &&
        static_cast<unsigned char>(lson[2]) == 0xBF) {
      pos = 3;
    }

    skipWhitespace(lson, pos);

    if (pos >= lson.size() || lson[pos] != '{') {
      setError(lson, pos, "Expected '{'");
      return false;
    }
    pos++;

    while (pos < lson.size()) {
      skipWhitespace(lson, pos);

      if (pos < lson.size() && lson[pos] == '}') {
        return true;
      }

      if (pos >= lson.size() || lson[pos] != '"') {
        setError(lson, pos, "Expected '\"' for key");
        return false;
      }
      std::string key = parseLsonString(lson, pos);
      if (!lastError.empty())
        return false;

      skipWhitespace(lson, pos);
      if (pos >= lson.size() || lson[pos] != ':') {
        setError(lson, pos, "Expected ':'");
        return false;
      }
      pos++;

      skipWhitespace(lson, pos);
      if (pos >= lson.size() || lson[pos] != '"') {
        setError(lson, pos, "Expected '\"' for value");
        return false;
      }
      std::string value = parseLsonString(lson, pos);
      if (!lastError.empty())
        return false;

      dictionary[key] = value;

      skipWhitespace(lson, pos);
      if (pos < lson.size() && lson[pos] == ',') {
        pos++;
        skipWhitespace(lson, pos);
        if (pos < lson.size() && lson[pos] == '}') {
          return true;
        }
      } else if (pos < lson.size() && lson[pos] == '}') {
        return true;
      } else {
        setError(lson, pos, "Expected ',' or '}'");
        return false;
      }
    }

    setError(lson, pos, "Missing '}'");
    return false;
  }

  std::string getLastError() const { return lastError; }

  std::string tr(const std::string &key) const {
    auto it = dictionary.find(key);
    if (it != dictionary.end()) {
      return it->second;
    }
    return key;
  }
};
