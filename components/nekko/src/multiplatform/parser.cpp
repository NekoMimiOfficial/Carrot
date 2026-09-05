#include "parser.h"
#include "lang.h"
#include <cstdint>

Parser::Parser(std::vector<Token> tokens) : tokens(std::move(tokens)) {}

std::vector<StmtPtr> Parser::parse() {
  std::vector<StmtPtr> statements;
  while (!isAtEnd()) {
    statements.push_back(declaration());
  }
  return statements;
}

StmtPtr Parser::declaration() {
  if (match({TokenType::GLOBAL}))
    return globalDeclaration();
  if (match({TokenType::LET}))
    return varDeclaration();
  if (match({TokenType::CONST}))
    return constDeclaration();
  if (match({TokenType::MUTEX_KW}))
    return mutexDeclaration();
  if (match({TokenType::ASYNC})) {
    consume(TokenType::FUN, LOC(CONSUME_FUN_AFTER_ASYNC));
    return asyncFunctionDeclaration();
  }
  if (match({TokenType::FUN}))
    return funDeclaration();
  if (match({TokenType::CLASS}))
    return classDeclaration();
  return statement();
}

StmtPtr Parser::globalDeclaration() {
  Token name =
      consume(TokenType::IDENTIFIER, LOC(CONSUME_VAR_NAME_AFTER_GLOBAL));
  consume(TokenType::EQUAL, LOC(CONSUME_EQUAL_AFTER_GLOBAL));
  ExprPtr init = expression();
  consume(TokenType::SEMICOLON, LOC(CONSUME_SEMI_AFTER_GLOBAL));
  return std::make_unique<GlobalDecl>(std::move(name), std::move(init));
}

StmtPtr Parser::varDeclaration() {
  Token name =
      consume(TokenType::IDENTIFIER, LOC(CONSUME_VAR_NAME_AFTER_LET));

  ExprPtr initializer = nullptr;
  if (match({TokenType::EQUAL})) {
    initializer = expression();
  }

  consume(TokenType::SEMICOLON, LOC(CONSUME_SEMI_AFTER_LET));
  return std::make_unique<VarDecl>(std::move(name), std::move(initializer));
}

StmtPtr Parser::constDeclaration() {
  Token name =
      consume(TokenType::IDENTIFIER, LOC(CONSUME_VAR_NAME_AFTER_CONST));
  consume(TokenType::EQUAL, LOC(CONSUME_EQUAL_AFTER_CONST));
  ExprPtr init = expression();
  consume(TokenType::SEMICOLON, LOC(CONSUME_SEMI_AFTER_CONST));
  return std::make_unique<ConstDecl>(std::move(name), std::move(init));
}

StmtPtr Parser::mutexDeclaration() {
  Token name =
      consume(TokenType::IDENTIFIER, LOC(CONSUME_VAR_NAME_AFTER_MUTEX));
  consume(TokenType::EQUAL, LOC(CONSUME_EQUAL_AFTER_MUTEX));
  ExprPtr init = expression();
  consume(TokenType::SEMICOLON, LOC(CONSUME_SEMI_AFTER_MUTEX));
  return std::make_unique<MutexDecl>(std::move(name), std::move(init));
}

StmtPtr Parser::funDeclaration() {
  Token name = consume(TokenType::IDENTIFIER, LOC(CONSUME_FN_NAME));
  consume(TokenType::LPAREN, LOC(CONSUME_LPAREN_AFTER_FN_NAME));

  std::vector<Token> params;
  std::vector<ExprPtr> defaults;
  bool seenDefault = false;
  if (!check(TokenType::RPAREN)) {
    do {
      if (params.size() >= 255)
        throw std::runtime_error(LOC(PARSE_TOO_MANY_PARAMS));

      Token param = consume(TokenType::IDENTIFIER, LOC(CONSUME_PARAM_NAME));
      params.push_back(param);

      if (match({TokenType::EQUAL})) {
        defaults.push_back(expression());
        seenDefault = true;
      } else {
        if (seenDefault)
          throw std::runtime_error(LOC(PARSE_DEFAULT_PARAM_ORDER, param.lexeme));
        defaults.push_back(nullptr);
      }
    } while (match({TokenType::COMMA}));
  }
  consume(TokenType::RPAREN, LOC(CONSUME_RPAREN_AFTER_PARAMS));
  consume(TokenType::LBRACE, LOC(CONSUME_LBRACE_BEFORE_FN_BODY));

  funcDepth++;
  int prevLoopDepth = loopDepth;
  loopDepth = 0;
  auto bodyBlock =
      std::unique_ptr<BlockStmt>(static_cast<BlockStmt *>(block().release()));
  loopDepth = prevLoopDepth;
  funcDepth--;

  return std::make_unique<FunctionStmt>(std::move(name), std::move(params),
                                        std::move(defaults),
                                        std::move(bodyBlock->statements));
}

StmtPtr Parser::asyncFunctionDeclaration() {
  Token name = consume(TokenType::IDENTIFIER, LOC(CONSUME_FN_NAME));
  consume(TokenType::LPAREN, LOC(CONSUME_LPAREN_AFTER_FN_NAME));

  std::vector<Token> params;
  std::vector<ExprPtr> defaults;
  bool seenDefault = false;
  if (!check(TokenType::RPAREN)) {
    do {
      Token param = consume(TokenType::IDENTIFIER, LOC(CONSUME_PARAM_NAME));
      params.push_back(param);

      if (match({TokenType::EQUAL})) {
        defaults.push_back(expression());
        seenDefault = true;
      } else {
        if (seenDefault)
          throw std::runtime_error(LOC(PARSE_DEFAULT_PARAM_ORDER, param.lexeme));
        defaults.push_back(nullptr);
      }
    } while (match({TokenType::COMMA}));
  }
  consume(TokenType::RPAREN, LOC(CONSUME_RPAREN_AFTER_PARAMS));
  consume(TokenType::LBRACE, LOC(CONSUME_LBRACE_BEFORE_FN_BODY));

  funcDepth++;
  int prevLoopDepth = loopDepth;
  loopDepth = 0;
  bool prev = insideAsync;
  insideAsync = true;
  auto bodyBlock =
      std::unique_ptr<BlockStmt>(static_cast<BlockStmt *>(block().release()));
  insideAsync = prev;
  loopDepth = prevLoopDepth;
  funcDepth--;

  return std::make_unique<AsyncFunctionStmt>(std::move(name), std::move(params),
                                             std::move(defaults),
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
  if (match({TokenType::LBRACE}))
    return block();
  if (match({TokenType::FREE}))
    return freeStatement();

  if (peek().lexeme == "break") {
    if (loopDepth == 0)
      throw std::runtime_error(LOC(PARSE_BREAK_OUTSIDE_LOOP, std::to_string(peek().line)));
    advance();
    consume(TokenType::SEMICOLON, LOC(CONSUME_SEMI_AFTER_BREAK));
    return std::make_unique<BreakStmt>();
  }
  if (peek().lexeme == "continue") {
    if (loopDepth == 0)
      throw std::runtime_error(LOC(PARSE_CONTINUE_OUTSIDE_LOOP, std::to_string(peek().line)));
    advance();
    consume(TokenType::SEMICOLON, LOC(CONSUME_SEMI_AFTER_CONTINUE));
    return std::make_unique<ContinueStmt>();
  }

  return expressionStatement();
}

StmtPtr Parser::ifStatement() {
  consume(TokenType::LPAREN, LOC(CONSUME_LPAREN_AFTER_IF));
  ExprPtr condition = expression();
  consume(TokenType::RPAREN, LOC(CONSUME_RPAREN_AFTER_IF_COND));

  StmtPtr thenBranch = statement();
  StmtPtr elseBranch = nullptr;
  if (match({TokenType::ELSE})) {
    elseBranch = statement();
  }

  return std::make_unique<IfStmt>(std::move(condition), std::move(thenBranch),
                                  std::move(elseBranch));
}

StmtPtr Parser::whileStatement() {
  consume(TokenType::LPAREN, LOC(CONSUME_LPAREN_AFTER_WHILE));
  ExprPtr condition = expression();
  consume(TokenType::RPAREN, LOC(CONSUME_RPAREN_AFTER_WHILE_COND));
  loopDepth++;
  StmtPtr body = statement();
  loopDepth--;
  return std::make_unique<WhileStmt>(std::move(condition), std::move(body));
}

StmtPtr Parser::forStatement() {
  consume(TokenType::LPAREN, LOC(CONSUME_LPAREN_AFTER_FOR));

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
  consume(TokenType::SEMICOLON, LOC(CONSUME_SEMI_AFTER_FOR_COND));

  ExprPtr increment = nullptr;
  if (!check(TokenType::RPAREN)) {
    increment = expression();
  }
  consume(TokenType::RPAREN, LOC(CONSUME_RPAREN_AFTER_FOR_CLAUSES));

  loopDepth++;
  StmtPtr body = statement();
  loopDepth--;

  return std::make_unique<ForStmt>(std::move(initializer), std::move(condition),
                                   std::move(increment), std::move(body));
}

StmtPtr Parser::returnStatement() {
  Token keyword = previous();
  if (funcDepth == 0)
    throw std::runtime_error(LOC(PARSE_RETURN_OUTSIDE_FUNC, std::to_string(keyword.line)));
  ExprPtr value = nullptr;
  if (!check(TokenType::SEMICOLON)) {
    value = expression();
  }
  consume(TokenType::SEMICOLON, LOC(CONSUME_SEMI_AFTER_RETURN));
  return std::make_unique<ReturnStmt>(std::move(keyword), std::move(value));
}

StmtPtr Parser::block() {
  std::vector<StmtPtr> statements;
  while (!check(TokenType::RBRACE) && !isAtEnd()) {
    statements.push_back(declaration());
  }
  consume(TokenType::RBRACE, LOC(CONSUME_RBRACE_CLOSE_BLOCK));
  return std::make_unique<BlockStmt>(std::move(statements));
}

StmtPtr Parser::expressionStatement() {
  ExprPtr expr = expression();
  consume(TokenType::SEMICOLON, LOC(CONSUME_SEMI_AFTER_EXPR));
  return std::make_unique<ExprStmt>(std::move(expr));
}

StmtPtr Parser::classDeclaration() {
  Token name = consume(TokenType::IDENTIFIER, LOC(CONSUME_CLASS_NAME));

  std::unique_ptr<Token> superclass = nullptr;
  if (match({TokenType::COLON})) {
    Token parentName =
        consume(TokenType::IDENTIFIER, LOC(CONSUME_PARENT_CLASS_NAME));
    superclass = std::make_unique<Token>(std::move(parentName));
  }

  consume(TokenType::LBRACE, LOC(CONSUME_LBRACE_BEFORE_CLASS_BODY));

  std::vector<std::unique_ptr<FunctionStmt>> methods;
  std::vector<std::unique_ptr<FunctionStmt>> overrides;

  while (!check(TokenType::RBRACE) && !isAtEnd()) {
    if (match({TokenType::OVERRIDE})) {
      consume(TokenType::FUN, LOC(CONSUME_FUN_AFTER_OVERRIDE));
      auto fn = std::unique_ptr<FunctionStmt>(
          static_cast<FunctionStmt *>(funDeclaration().release()));
      overrides.push_back(std::move(fn));
    } else {
      consume(TokenType::FUN, LOC(CONSUME_FUN_IN_CLASS_BODY));
      auto fn = std::unique_ptr<FunctionStmt>(
          static_cast<FunctionStmt *>(funDeclaration().release()));
      methods.push_back(std::move(fn));
    }
  }

  consume(TokenType::RBRACE, LOC(CONSUME_RBRACE_AFTER_CLASS_BODY));
  return std::make_unique<ClassStmt>(std::move(name), std::move(methods),
                                     std::move(overrides),
                                     std::move(superclass));
}

StmtPtr Parser::freeStatement() {
  Token name =
      consume(TokenType::IDENTIFIER, LOC(CONSUME_VAR_NAME_AFTER_FREE));
  consume(TokenType::SEMICOLON, LOC(CONSUME_SEMI_AFTER_FREE));
  return std::make_unique<FreeStmt>(std::move(name));
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

    throw std::runtime_error(LOC(PARSE_INVALID_ASSIGN_TARGET, std::to_string(previous().line)));
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
      std::vector<std::pair<std::string, ExprPtr>> kwargs;
      if (!check(TokenType::RPAREN)) {
        do {
          if (args.size() + kwargs.size() >= 255)
            throw std::runtime_error(LOC(PARSE_TOO_MANY_ARGS));

          if (check(TokenType::IDENTIFIER) && checkNext(TokenType::EQUAL)) {
            Token kwName = advance();
            advance();
            kwargs.emplace_back(kwName.lexeme, expression());
          } else {
            args.push_back(expression());
          }
        } while (match({TokenType::COMMA}));
      }
      consume(TokenType::RPAREN, LOC(CONSUME_RPAREN_AFTER_ARGS));
      expr = std::make_unique<CallExpr>(std::move(expr), std::move(paren),
                                        std::move(args), std::move(kwargs));
    } else if (match({TokenType::LBRACKET})) {
      Token bracket = previous();
      ExprPtr index = expression();
      consume(TokenType::RBRACKET, LOC(CONSUME_RBRACKET_AFTER_INDEX));
      expr = std::make_unique<IndexExpr>(std::move(expr), std::move(index),
                                         std::move(bracket));
    } else if (match({TokenType::DOT})) {
      Token propName =
          consume(TokenType::IDENTIFIER, LOC(CONSUME_PROP_NAME_AFTER_DOT));
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

  if (match({TokenType::BYTE})) {
    uint8_t val = std::get<uint8_t>(previous().literal);
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
    consume(TokenType::RBRACKET, LOC(CONSUME_RBRACKET_AFTER_ARRAY));
    return std::make_unique<ArrayExpr>(std::move(elements));
  }

  if (match({TokenType::LPAREN})) {
    ExprPtr expr = expression();
    consume(TokenType::RPAREN, LOC(CONSUME_RPAREN_AFTER_EXPR));
    return expr;
  }

  if (match({TokenType::NEW})) {
    Token kw = previous();

    ExprPtr classRef = std::make_unique<VariableExpr>(
        consume(TokenType::IDENTIFIER, LOC(CONSUME_CLASS_NAME_AFTER_NEW)));

    while (match({TokenType::DOT})) {
      Token propName =
          consume(TokenType::IDENTIFIER, LOC(CONSUME_PROP_NAME_AFTER_DOT));
      classRef =
          std::make_unique<GetExpr>(std::move(classRef), std::move(propName));
    }

    consume(TokenType::LPAREN, LOC(CONSUME_LPAREN_AFTER_CLASS_NAME));
    std::vector<ExprPtr> args;
    if (!check(TokenType::RPAREN)) {
      do {
        args.push_back(expression());
      } while (match({TokenType::COMMA}));
    }
    consume(TokenType::RPAREN, LOC(CONSUME_RPAREN_AFTER_ARGS));

    return std::make_unique<NewExpr>(std::move(kw), std::move(classRef),
                                     std::move(args));
  }

  if (match({TokenType::THIS}))
    return std::make_unique<ThisExpr>(previous());

  if (match({TokenType::COROUTINE_KW})) {
    Token kw = previous();
    Token fnName = consume(TokenType::IDENTIFIER, LOC(CONSUME_FN_NAME_AFTER_COROUTINE));
    std::vector<ExprPtr> args;
    std::vector<std::pair<std::string, ExprPtr>> kwargs;
    if (!check(TokenType::RPAREN)) {
      do {
        if (check(TokenType::IDENTIFIER) && checkNext(TokenType::EQUAL)) {
          Token kwName = advance();
          advance();
          kwargs.emplace_back(kwName.lexeme, expression());
        } else {
          args.push_back(expression());
        }
      } while (match({TokenType::COMMA}));
    }
    consume(TokenType::RPAREN, LOC(CONSUME_RPAREN_GENERIC));
    return std::make_unique<CoroutineExpr>(std::move(kw), std::move(fnName),
                                           std::move(args), std::move(kwargs));
  }

  if (match({TokenType::AWAIT})) {
    if (!insideAsync)
      throw std::runtime_error(LOC(PARSE_AWAIT_OUTSIDE_ASYNC, std::to_string(previous().line)));
    Token kw = previous();
    ExprPtr val = call();
    return std::make_unique<AwaitExpr>(std::move(kw), std::move(val));
  }

  if (match({TokenType::SUPER})) {
    Token kw = previous();
    consume(TokenType::LPAREN, LOC(CONSUME_LPAREN_AFTER_SUPER));
    std::vector<ExprPtr> args;
    if (!check(TokenType::RPAREN)) {
      do {
        args.push_back(expression());
      } while (match({TokenType::COMMA}));
    }
    consume(TokenType::RPAREN, LOC(CONSUME_RPAREN_AFTER_SUPER_ARGS));
    return std::make_unique<SuperExpr>(std::move(kw), std::move(args));
  }

  throw std::runtime_error(LOC(PARSE_EXPECT_EXPRESSION, std::to_string(peek().line), peek().lexeme));
}

bool Parser::check(TokenType type) {
  if (isAtEnd())
    return false;
  return peek().type == type;
}

bool Parser::checkNext(TokenType type) {
  if (isAtEnd())
    return false;
  if (current + 1 >= (int)tokens.size())
    return false;
  return tokens[current + 1].type == type;
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
