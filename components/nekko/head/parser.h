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
  int loopDepth = 0;
  int funcDepth = 0;

  StmtPtr declaration();
  StmtPtr globalDeclaration();
  StmtPtr constDeclaration();
  StmtPtr mutexDeclaration();
  StmtPtr varDeclaration();
  StmtPtr funDeclaration();
  StmtPtr freeStatement();
  StmtPtr statement();
  StmtPtr ifStatement();
  StmtPtr whileStatement();
  StmtPtr forStatement();
  StmtPtr returnStatement();
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
  bool checkNext(TokenType type);
  Token advance();
  Token peek();
  Token previous();
  bool isAtEnd();
  Token consume(TokenType type, const std::string &message);
};
