#include "lexer.h"
#include <stdexcept>

const std::unordered_map<std::string, TokenType> Lexer::keywords = {
    {"let", TokenType::LET},         {"fun", TokenType::FUN},
    {"return", TokenType::RETURN},   {"if", TokenType::IF},
    {"else", TokenType::ELSE},       {"while", TokenType::WHILE},
    {"for", TokenType::FOR},         {"true", TokenType::TRUE_LIT},
    {"false", TokenType::FALSE_LIT}, {"nil", TokenType::NIL},
    {"print", TokenType::PRINT},     {"and", TokenType::AND},
    {"or", TokenType::OR},           {"class", TokenType::CLASS},
    {"new", TokenType::NEW},         {"this", TokenType::THIS},
    {"async", TokenType::ASYNC},     {"coroutine", TokenType::COROUTINE_KW},
    {"await", TokenType::AWAIT},     {"global", TokenType::GLOBAL},
    {"const", TokenType::CONST},     {"free", TokenType::FREE},
};

Lexer::Lexer(const std::string &source) : source(source) {}

std::vector<Token> Lexer::tokenize() {
  while (!isAtEnd()) {
    start = current;
    scanToken();
  }
  tokens.emplace_back(TokenType::EOF_TOKEN, "", line);
  return tokens;
}

void Lexer::scanToken() {
  char c = advance();
  switch (c) {
  case '(':
    addToken(TokenType::LPAREN);
    break;
  case ')':
    addToken(TokenType::RPAREN);
    break;
  case '{':
    addToken(TokenType::LBRACE);
    break;
  case '}':
    addToken(TokenType::RBRACE);
    break;
  case '[':
    addToken(TokenType::LBRACKET);
    break;
  case ']':
    addToken(TokenType::RBRACKET);
    break;
  case ';':
    addToken(TokenType::SEMICOLON);
    break;
  case ',':
    addToken(TokenType::COMMA);
    break;
  case '.':
    addToken(TokenType::DOT);
    break;
  case '+':
    addToken(TokenType::PLUS);
    break;
  case '-':
    addToken(TokenType::MINUS);
    break;
  case '*':
    addToken(TokenType::STAR);
    break;
  case '%':
    addToken(TokenType::PERCENT);
    break;

  case '!':
    addToken(match('=') ? TokenType::BANG_EQUAL : TokenType::BANG);
    break;
  case '=':
    addToken(match('=') ? TokenType::EQUAL_EQUAL : TokenType::EQUAL);
    break;
  case '<':
    addToken(match('=') ? TokenType::LESS_EQUAL : TokenType::LESS);
    break;
  case '>':
    addToken(match('=') ? TokenType::GREATER_EQUAL : TokenType::GREATER);
    break;
  case '&':
    if (match('&'))
      addToken(TokenType::AND);
    else
      throw std::runtime_error("Unexpected character '&' at line " +
                               std::to_string(line) + ". Did you mean '&&'?");
    break;
  case '|':
    if (match('|'))
      addToken(TokenType::OR);
    else
      throw std::runtime_error("Unexpected character '|' at line " +
                               std::to_string(line) + ". Did you mean '||'?");
    break;

  case '/':
    if (match('/')) {

      while (peek() != '\n' && !isAtEnd())
        advance();
    } else if (match('*')) {

      while (!isAtEnd()) {
        if (peek() == '\n')
          line++;
        if (peek() == '*' && peekNext() == '/') {
          advance();
          advance();
          break;
        }
        advance();
      }
    } else {
      addToken(TokenType::SLASH);
    }
    break;

  case ' ':
  case '\r':
  case '\t':
    break;
  case '\n':
    line++;
    break;

  case '"':
    scanString();
    break;

  default:
    if (isDigit(c)) {
      scanNumber();
    } else if (isAlpha(c)) {
      scanIdentifier();
    } else {
      throw std::runtime_error("Unexpected character '" + std::string(1, c) +
                               "' at line " + std::to_string(line));
    }
    break;
  }
}

void Lexer::scanString() {
  while (peek() != '"' && !isAtEnd()) {
    if (peek() == '\n')
      line++;

    if (peek() == '\\') {
      advance();
      if (!isAtEnd())
        advance();
      continue;
    }
    advance();
  }

  if (isAtEnd()) {
    throw std::runtime_error("Unterminated string starting at line " +
                             std::to_string(line));
  }

  advance();

  std::string raw = source.substr(start + 1, current - start - 2);
  std::string value;
  for (size_t i = 0; i < raw.size(); i++) {
    if (raw[i] == '\\' && i + 1 < raw.size()) {
      switch (raw[i + 1]) {
      case 'n':
        value += '\n';
        i++;
        break;
      case 't':
        value += '\t';
        i++;
        break;
      case 'r':
        value += '\r';
        i++;
        break;
      case '"':
        value += '"';
        i++;
        break;
      case '\\':
        value += '\\';
        i++;
        break;
      default:
        value += raw[i];
        break;
      }
    } else {
      value += raw[i];
    }
  }

  addToken(TokenType::STRING, value);
}

void Lexer::scanNumber() {
  while (isDigit(peek()))
    advance();

  if (peek() == '.' && isDigit(peekNext())) {
    advance();
    while (isDigit(peek()))
      advance();
  }

  std::string numStr = source.substr(start, current - start);
  double value = std::stod(numStr);
  addToken(TokenType::NUMBER, value);
}

void Lexer::scanIdentifier() {
  while (isAlphaNumeric(peek()))
    advance();

  std::string text = source.substr(start, current - start);
  auto it = keywords.find(text);
  TokenType type = (it != keywords.end()) ? it->second : TokenType::IDENTIFIER;
  addToken(type);
}

bool Lexer::isAtEnd() { return current >= (int)source.size(); }

char Lexer::advance() { return source[current++]; }

char Lexer::peek() {
  if (isAtEnd())
    return '\0';
  return source[current];
}

char Lexer::peekNext() {
  if (current + 1 >= (int)source.size())
    return '\0';
  return source[current + 1];
}

bool Lexer::match(char expected) {
  if (isAtEnd())
    return false;
  if (source[current] != expected)
    return false;
  current++;
  return true;
}

bool Lexer::isDigit(char c) { return c >= '0' && c <= '9'; }

bool Lexer::isAlpha(char c) {
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

bool Lexer::isAlphaNumeric(char c) { return isAlpha(c) || isDigit(c); }

void Lexer::addToken(TokenType type) {
  std::string text = source.substr(start, current - start);
  tokens.emplace_back(type, text, line);
}

void Lexer::addToken(TokenType type, double number) {
  std::string text = source.substr(start, current - start);
  tokens.emplace_back(type, text, line, number);
}

void Lexer::addToken(TokenType type, std::string str) {
  std::string text = source.substr(start, current - start);
  tokens.emplace_back(type, text, line, std::move(str));
}
