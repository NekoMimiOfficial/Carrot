#include "interpreter.h"
#include "lexer.h"
#include "meta.cpp"
#include "parser.h"
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <readline/history.h>
#include <readline/readline.h>
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
  std::cout << "Carrot " << AppVer.maj << "." << AppVer.min << "." << AppVer.fix
            << " [" << AppVer.codename << "] (main, " << AppVer.date << ") [GCC "
            << AppVer.gccver << "] on linux\n";
  std::cout << "type \"exit\" or hit CTRL-d to exit the REPL.\n\n";

  char *rawInput = nullptr;

  while ((rawInput = readline(">>> ")) != nullptr) {
    std::string input(rawInput);
    free(rawInput);

    if (!input.empty()) {
      if (input == "exit")
        break;

      add_history(input.c_str());
      runSource(input, true);
    }
  }

  std::cout << "See you next time!\n";
}

int main(int argc, char *argv[]) {
  system("mkdir -p ~/.local/lib/carrot/modules");
  system("mkdir -p ~/.local/lib/carrot/CAPI");
  if (argc == 1) {
    runREPL();
  } else if (argc == 2) {
    runFile(argv[1]);
  } else {
    std::cerr
        << "This program takes 0 or 1 arguments\nRun it without an argument to "
           "open the interactive REPL\nPass a Ninjin script to run it.";
    return 1;
  }

  return 0;
}
