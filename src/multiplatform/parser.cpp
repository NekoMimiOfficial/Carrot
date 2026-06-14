#include "parser.h"

Parser::Parser(std::vector<Token> tokens) : tokens(std::move(tokens)) {}

std::vector<StmtPtr> Parser::parse() {
  std::vector<StmtPtr> statements;
  while (!isAtEnd()) {
    statements.push_back(declaration());
  }
  return statements;
}

StmtPtr Parser::declaration() {
  if (match({TokenType::LET}))
    return varDeclaration();
  if (match({TokenType::FUN}))
    return funDeclaration();
  if (match({TokenType::CLASS}))
    return classDeclaration();
  return statement();
}

StmtPtr Parser::varDeclaration() {
  Token name =
      consume(TokenType::IDENTIFIER, "Expected variable name after 'let'.");

  ExprPtr initializer = nullptr;
  if (match({TokenType::EQUAL})) {
    initializer = expression();
  }

  consume(TokenType::SEMICOLON, "Expected ';' after variable declaration.");
  return std::make_unique<VarDecl>(std::move(name), std::move(initializer));
}

StmtPtr Parser::funDeclaration() {
  Token name = consume(TokenType::IDENTIFIER, "Expected function name.");
  consume(TokenType::LPAREN, "Expected '(' after function name.");

  std::vector<Token> params;
  if (!check(TokenType::RPAREN)) {
    do {
      if (params.size() >= 255) {
        throw std::runtime_error("Cannot have more than 255 parameters.");
      }
      params.push_back(
          consume(TokenType::IDENTIFIER, "Expected parameter name."));
    } while (match({TokenType::COMMA}));
  }
  consume(TokenType::RPAREN, "Expected ')' after parameters.");

  consume(TokenType::LBRACE, "Expected '{' before function body.");
  auto bodyBlock =
      std::unique_ptr<BlockStmt>(static_cast<BlockStmt *>(block().release()));

  return std::make_unique<FunctionStmt>(std::move(name), std::move(params),
                                        std::move(bodyBlock->statements));
}

StmtPtr Parser::statement() {
  if (match({TokenType::IF}))
    return ifStatement();
  if (match({TokenType::WHILE}))
    return whileStatement();
  if (match({TokenType::FOR}))
    return forStatement();
  if (match({TokenType::RETURN}))
    return returnStatement();
  if (match({TokenType::PRINT}))
    return printStatement();
  if (match({TokenType::LBRACE}))
    return block();

  if (peek().lexeme == "break") {
    advance();
    consume(TokenType::SEMICOLON, "Expected ';' after 'break'.");
    return std::make_unique<BreakStmt>();
  }
  if (peek().lexeme == "continue") {
    advance();
    consume(TokenType::SEMICOLON, "Expected ';' after 'continue'.");
    return std::make_unique<ContinueStmt>();
  }

  return expressionStatement();
}

StmtPtr Parser::ifStatement() {
  consume(TokenType::LPAREN, "Expected '(' after 'if'.");
  ExprPtr condition = expression();
  consume(TokenType::RPAREN, "Expected ')' after if condition.");

  StmtPtr thenBranch = statement();
  StmtPtr elseBranch = nullptr;
  if (match({TokenType::ELSE})) {
    elseBranch = statement();
  }

  return std::make_unique<IfStmt>(std::move(condition), std::move(thenBranch),
                                  std::move(elseBranch));
}

StmtPtr Parser::whileStatement() {
  consume(TokenType::LPAREN, "Expected '(' after 'while'.");
  ExprPtr condition = expression();
  consume(TokenType::RPAREN, "Expected ')' after while condition.");
  StmtPtr body = statement();
  return std::make_unique<WhileStmt>(std::move(condition), std::move(body));
}

StmtPtr Parser::forStatement() {
  consume(TokenType::LPAREN, "Expected '(' after 'for'.");

  StmtPtr initializer = nullptr;
  if (match({TokenType::SEMICOLON})) {
    initializer = nullptr;
  } else if (match({TokenType::LET})) {
    initializer = varDeclaration();
  } else {
    initializer = expressionStatement();
  }

  ExprPtr condition = nullptr;
  if (!check(TokenType::SEMICOLON)) {
    condition = expression();
  }
  consume(TokenType::SEMICOLON, "Expected ';' after for-loop condition.");

  ExprPtr increment = nullptr;
  if (!check(TokenType::RPAREN)) {
    increment = expression();
  }
  consume(TokenType::RPAREN, "Expected ')' after for clauses.");

  StmtPtr body = statement();

  return std::make_unique<ForStmt>(std::move(initializer), std::move(condition),
                                   std::move(increment), std::move(body));
}

StmtPtr Parser::returnStatement() {
  Token keyword = previous();
  ExprPtr value = nullptr;
  if (!check(TokenType::SEMICOLON)) {
    value = expression();
  }
  consume(TokenType::SEMICOLON, "Expected ';' after return value.");
  return std::make_unique<ReturnStmt>(std::move(keyword), std::move(value));
}

StmtPtr Parser::printStatement() {
  consume(TokenType::LPAREN, "Expected '(' after 'print'.");
  ExprPtr value = expression();
  consume(TokenType::RPAREN, "Expected ')' after print argument.");
  consume(TokenType::SEMICOLON, "Expected ';' after print statement.");
  return std::make_unique<PrintStmt>(std::move(value));
}

StmtPtr Parser::block() {
  std::vector<StmtPtr> statements;
  while (!check(TokenType::RBRACE) && !isAtEnd()) {
    statements.push_back(declaration());
  }
  consume(TokenType::RBRACE, "Expected '}' to close block.");
  return std::make_unique<BlockStmt>(std::move(statements));
}

StmtPtr Parser::expressionStatement() {
  ExprPtr expr = expression();
  consume(TokenType::SEMICOLON, "Expected ';' after expression.");
  return std::make_unique<ExprStmt>(std::move(expr));
}

StmtPtr Parser::classDeclaration() {
  Token name = consume(TokenType::IDENTIFIER, "Expected class name.");
  consume(TokenType::LBRACE, "Expected '{' before class body.");

  std::vector<std::unique_ptr<FunctionStmt>> methods;
  while (!check(TokenType::RBRACE) && !isAtEnd()) {
    consume(TokenType::FUN, "Expected 'fun' in class body.");
    auto fn = std::unique_ptr<FunctionStmt>(
        static_cast<FunctionStmt *>(funDeclaration().release()));
    methods.push_back(std::move(fn));
  }
  consume(TokenType::RBRACE, "Expected '}' after class body.");
  return std::make_unique<ClassStmt>(std::move(name), std::move(methods));
}

ExprPtr Parser::expression() { return assignment(); }

ExprPtr Parser::assignment() {
  ExprPtr expr = logicOr();

  if (match({TokenType::EQUAL})) {
    ExprPtr value = assignment();

    if (auto *varExpr = dynamic_cast<VariableExpr *>(expr.get())) {
      return std::make_unique<AssignExpr>(varExpr->name, std::move(value));
    }

    if (auto *idxExpr = dynamic_cast<IndexExpr *>(expr.get())) {

      return std::make_unique<IndexAssignExpr>(
          std::move(idxExpr->object), std::move(idxExpr->index),
          std::move(value), idxExpr->bracket);
    }

    if (auto *get = dynamic_cast<GetExpr *>(expr.get())) {
      return std::make_unique<SetExpr>(std::move(get->object), get->name,
                                       std::move(value));
    }

    throw std::runtime_error("Invalid assignment target at line " +
                             std::to_string(previous().line) + ".");
  }

  return expr;
}

ExprPtr Parser::logicOr() {
  ExprPtr expr = logicAnd();
  while (match({TokenType::OR})) {
    Token op = previous();
    ExprPtr right = logicAnd();
    expr = std::make_unique<LogicalExpr>(std::move(expr), std::move(op),
                                         std::move(right));
  }
  return expr;
}

ExprPtr Parser::logicAnd() {
  ExprPtr expr = equality();
  while (match({TokenType::AND})) {
    Token op = previous();
    ExprPtr right = equality();
    expr = std::make_unique<LogicalExpr>(std::move(expr), std::move(op),
                                         std::move(right));
  }
  return expr;
}

ExprPtr Parser::equality() {
  ExprPtr expr = comparison();
  while (match({TokenType::EQUAL_EQUAL, TokenType::BANG_EQUAL})) {
    Token op = previous();
    ExprPtr right = comparison();
    expr = std::make_unique<BinaryExpr>(std::move(expr), std::move(op),
                                        std::move(right));
  }
  return expr;
}

ExprPtr Parser::comparison() {
  ExprPtr expr = term();
  while (match({TokenType::LESS, TokenType::LESS_EQUAL, TokenType::GREATER,
                TokenType::GREATER_EQUAL})) {
    Token op = previous();
    ExprPtr right = term();
    expr = std::make_unique<BinaryExpr>(std::move(expr), std::move(op),
                                        std::move(right));
  }
  return expr;
}

ExprPtr Parser::term() {
  ExprPtr expr = factor();
  while (match({TokenType::PLUS, TokenType::MINUS})) {
    Token op = previous();
    ExprPtr right = factor();
    expr = std::make_unique<BinaryExpr>(std::move(expr), std::move(op),
                                        std::move(right));
  }
  return expr;
}

ExprPtr Parser::factor() {
  ExprPtr expr = unary();
  while (match({TokenType::STAR, TokenType::SLASH, TokenType::PERCENT})) {
    Token op = previous();
    ExprPtr right = unary();
    expr = std::make_unique<BinaryExpr>(std::move(expr), std::move(op),
                                        std::move(right));
  }
  return expr;
}

ExprPtr Parser::unary() {
  if (match({TokenType::BANG, TokenType::MINUS})) {
    Token op = previous();
    ExprPtr right = unary();
    return std::make_unique<UnaryExpr>(std::move(op), std::move(right));
  }
  return call();
}

ExprPtr Parser::call() {
  ExprPtr expr = primary();

  while (true) {
    if (match({TokenType::LPAREN})) {
      Token paren = previous();
      std::vector<ExprPtr> args;
      if (!check(TokenType::RPAREN)) {
        do {
          if (args.size() >= 255)
            throw std::runtime_error("Cannot have more than 255 arguments.");
          args.push_back(expression());
        } while (match({TokenType::COMMA}));
      }
      consume(TokenType::RPAREN, "Expected ')' after arguments.");
      expr = std::make_unique<CallExpr>(std::move(expr), std::move(paren),
                                        std::move(args));
    } else if (match({TokenType::LBRACKET})) {
      Token bracket = previous();
      ExprPtr index = expression();
      consume(TokenType::RBRACKET, "Expected ']' after index.");
      expr = std::make_unique<IndexExpr>(std::move(expr), std::move(index),
                                         std::move(bracket));
    } else if (match({TokenType::DOT})) {
      Token propName =
          consume(TokenType::IDENTIFIER, "Expected property name after '.'.");
      expr = std::make_unique<GetExpr>(std::move(expr), std::move(propName));
    } else {
      break;
    }
  }

  return expr;
}

ExprPtr Parser::primary() {
  if (match({TokenType::TRUE_LIT}))
    return std::make_unique<LiteralExpr>(true);
  if (match({TokenType::FALSE_LIT}))
    return std::make_unique<LiteralExpr>(false);
  if (match({TokenType::NIL}))
    return std::make_unique<LiteralExpr>();

  if (match({TokenType::NUMBER})) {
    double val = std::get<double>(previous().literal);
    return std::make_unique<LiteralExpr>(val);
  }

  if (match({TokenType::STRING})) {
    std::string val = std::get<std::string>(previous().literal);
    return std::make_unique<LiteralExpr>(std::move(val));
  }

  if (match({TokenType::IDENTIFIER})) {
    return std::make_unique<VariableExpr>(previous());
  }

  if (match({TokenType::LBRACKET})) {
    std::vector<ExprPtr> elements;
    if (!check(TokenType::RBRACKET)) {
      do {
        elements.push_back(expression());
      } while (match({TokenType::COMMA}));
    }
    consume(TokenType::RBRACKET, "Expected ']' after array elements.");
    return std::make_unique<ArrayExpr>(std::move(elements));
  }

  if (match({TokenType::LPAREN})) {
    ExprPtr expr = expression();
    consume(TokenType::RPAREN, "Expected ')' after expression.");
    return expr;
  }

  if (match({TokenType::NEW})) {
    Token kw = previous();
    Token cls =
        consume(TokenType::IDENTIFIER, "Expected class name after 'new'.");
    consume(TokenType::LPAREN, "Expected '(' after class name.");
    std::vector<ExprPtr> args;
    if (!check(TokenType::RPAREN)) {
      do {
        args.push_back(expression());
      } while (match({TokenType::COMMA}));
    }
    consume(TokenType::RPAREN, "Expected ')' after arguments.");
    return std::make_unique<NewExpr>(std::move(kw), std::move(cls),
                                     std::move(args));
  }

  if (match({TokenType::THIS}))
    return std::make_unique<ThisExpr>(previous());

  throw std::runtime_error("Expected expression at line " +
                           std::to_string(peek().line) +
                           ", got unexpected token '" + peek().lexeme + "'.");
}

bool Parser::check(TokenType type) {
  if (isAtEnd())
    return false;
  return peek().type == type;
}

bool Parser::match(std::initializer_list<TokenType> types) {
  for (TokenType type : types) {
    if (check(type)) {
      advance();
      return true;
    }
  }
  return false;
}

Token Parser::advance() {
  if (!isAtEnd())
    current++;
  return previous();
}

Token Parser::peek() { return tokens[current]; }

Token Parser::previous() { return tokens[current - 1]; }

bool Parser::isAtEnd() { return peek().type == TokenType::EOF_TOKEN; }

Token Parser::consume(TokenType type, const std::string &message) {
  if (check(type))
    return advance();
  throw std::runtime_error(message + " (line " + std::to_string(peek().line) +
                           ")");
}
