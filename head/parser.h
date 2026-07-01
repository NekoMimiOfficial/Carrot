#pragma once
#include "ast.h"
#include "token.h"
#include <memory>
#include <stdexcept>
#include <vector>

class Parser {
public:
  explicit Parser(std::vector<Token> tokens);
  std::vector<StmtPtr> parse();

private:
  std::vector<Token> tokens;
  int current = 0;

  StmtPtr declaration();
  StmtPtr globalDeclaration();
  StmtPtr varDeclaration();
  StmtPtr funDeclaration();
  StmtPtr statement();
  StmtPtr ifStatement();
  StmtPtr whileStatement();
  StmtPtr forStatement();
  StmtPtr returnStatement();
  StmtPtr printStatement();
  StmtPtr breakStatement();
  StmtPtr continueStatement();
  StmtPtr block();
  StmtPtr expressionStatement();
  StmtPtr classDeclaration();
  StmtPtr asyncFunctionDeclaration();

  ExprPtr expression();
  ExprPtr assignment();
  ExprPtr logicOr();
  ExprPtr logicAnd();
  ExprPtr equality();
  ExprPtr comparison();
  ExprPtr term();
  ExprPtr factor();
  ExprPtr unary();
  ExprPtr call();
  ExprPtr primary();
  ExprPtr newExpression();

  bool check(TokenType type);
  bool insideAsync = false;
  bool match(std::initializer_list<TokenType> types);
  Token advance();
  Token peek();
  Token previous();
  bool isAtEnd();
  Token consume(TokenType type, const std::string &message);
};
