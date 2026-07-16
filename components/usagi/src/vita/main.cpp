#include "interpreter.h"
#include "lexer.h"
#include "parser.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

static std::vector<StmtPtr> replAst;
static Interpreter interpreter(".");
static bool hadError = false;

void runSource(const std::string &source) {
  hadError = false;

  try {
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.tokenize();

    Parser parser(std::move(tokens));
    std::vector<StmtPtr> statements = parser.parse();

    interpreter.interpret(statements);
  } catch (const std::runtime_error &e) {
    std::cerr << "[Error] " << e.what() << "\n";
    hadError = true;
  } catch (const std::exception &e) {
    std::cerr << "[Internal Error] " << e.what() << "\n";
    hadError = true;
  }
}

void runFile(const std::string &path) {
  std::ifstream file(path);
  if (!file.is_open()) {
    std::cerr << "Could not open file: " << path << "\n";
    std::exit(1);
  }

  interpreter = Interpreter(".");

  std::ostringstream ss;
  ss << file.rdbuf();
  runSource(ss.str());

  if (hadError)
    std::exit(1);
}

int main(int argc, char *argv[]) {
  runFile("app.nin");

  return 0;
}
