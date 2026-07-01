#pragma once
#include <string>
#include <variant>

enum class TokenType {

  NUMBER,
  STRING,
  TRUE_LIT,
  FALSE_LIT,
  NIL,

  IDENTIFIER,
  LET,
  FUN,
  RETURN,
  IF,
  ELSE,
  WHILE,
  FOR,
  PRINT,
  AND,
  OR,

  PLUS,
  MINUS,
  STAR,
  SLASH,
  PERCENT,

  EQUAL_EQUAL,
  BANG_EQUAL,
  LESS,
  LESS_EQUAL,
  GREATER,
  GREATER_EQUAL,

  EQUAL,

  BANG,

  LPAREN,
  RPAREN,
  LBRACE,
  RBRACE,
  LBRACKET,
  RBRACKET,
  SEMICOLON,
  COMMA,
  DOT,

  CLASS,
  NEW,
  THIS,
  GLOBAL,
  CONST,

  ASYNC,
  COROUTINE_KW,
  AWAIT,

  EOF_TOKEN
};

struct Token {
  TokenType type;
  std::string lexeme;
  int line;
  std::variant<std::monostate, double, std::string> literal;

  Token(TokenType type, std::string lexeme, int line)
      : type(type), lexeme(std::move(lexeme)), line(line) {}

  Token(TokenType type, std::string lexeme, int line, double num)
      : type(type), lexeme(std::move(lexeme)), line(line), literal(num) {}

  Token(TokenType type, std::string lexeme, int line, std::string str)
      : type(type), lexeme(std::move(lexeme)), line(line),
        literal(std::move(str)) {}
};
