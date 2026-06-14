#include "interpreter.h"
#include "lexer.h"
#include "parser.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

static std::vector<StmtPtr> replAst;
static Interpreter interpreter(".");
static bool hadError = false;

void runSource(const std::string &source, bool isRepl = false) {
  hadError = false;

  try {
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.tokenize();

    Parser parser(std::move(tokens));
    std::vector<StmtPtr> statements = parser.parse();

    if (isRepl) {
      size_t before = replAst.size();
      for (auto &s : statements)
        replAst.push_back(std::move(s));
      for (size_t i = before; i < replAst.size(); i++)
        interpreter.executeOne(replAst[i].get());
    } else {
      interpreter.interpret(statements);
    }
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

  std::string dir = std::filesystem::path(path).parent_path().string();
  if (dir.empty())
    dir = ".";
  interpreter = Interpreter(dir);

  std::ostringstream ss;
  ss << file.rdbuf();
  runSource(ss.str());

  if (hadError)
    std::exit(1);
}

void runREPL() {
  std::cout
  << "Carrot 1.0.0 (main, Jun 11 2026, 21:58:32) [GCC 13.3.0] on linux\n";
  std::cout << "type \"exit\" or hit CTRL-d to exit the REPL.\n";

  std::string line;
  while (true) {
    std::cout << ">> ";
    std::cout.flush();
    if (!std::getline(std::cin, line)) {
      std::cout << "\n";
      break;
    }
    if (line == "exit" || line == "quit")
      break;
    if (line.empty())
      continue;
    runSource(line, true);
  }

  std::cout << "ikanaide~ qwq!\n";
}

int main(int argc, char *argv[]) {
  if (argc == 1) {
    runREPL();
  } else if (argc == 2) {
    runFile(argv[1]);
  } else {
    std::cerr << "Usage: carrot [script.nin]\n";
    std::cerr << "       carrot              (starts the REPL)\n";
    return 1;
  }

  return 0;
}
