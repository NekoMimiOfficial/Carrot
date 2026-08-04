#pragma once
#include "token.h"
#include <string>
#include <unordered_map>
#include <vector>

class Lexer {
public:
  explicit Lexer(const std::string &source);
  std::vector<Token> tokenize();

private:
  std::string source;
  int start = 0;
  int current = 0;
  int line = 1;
  std::vector<Token> tokens;

  static const std::unordered_map<std::string, TokenType> keywords;

  bool isAtEnd();
  bool isDigit(char c);
  bool isAlpha(char c);
  bool isAlphaNumeric(char c);

  char advance();
  char peek();
  char peekNext();
  bool match(char expected);

  void scanToken();
  void scanString();
  void scanNumber();
  void scanIdentifier();

  void addToken(TokenType type);
  void addToken(TokenType type, double number);
  void addToken(TokenType type, std::string str);
};
