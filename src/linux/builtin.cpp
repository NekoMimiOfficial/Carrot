#include "builtin.h"
#include "value.h"
#include "fs.h"
#include <cstdlib>
#include <dlfcn.h>
#include <iterator>
#include <stdexcept>
#include <string>
#include <filesystem>
#include <sys/types.h>

using InitFn = void (*)(std::unordered_map<std::string, Value> *);

const char *global_capi_dir = "~/.local/lib/carrot/CAPI/";
const char *global_module_dir = "~/.local/lib/carrot/modules/";

Value ImportFn::call(std::vector<Value> args) {
  if (!std::holds_alternative<std::string>(args[0]))
    throw std::runtime_error("import(): argument must be a string path.");

  std::filesystem::path path;
  std::string rel = std::get<std::string>(args[0]);
  if (!rel.empty() and rel.front() == '@') {
    path = expand_user_path(global_module_dir).string() + rel.substr(1);
  } else {
    path = std::filesystem::path(callerDir) / rel;
  }

  std::ifstream file(path);
  if (!file.is_open())
    throw std::runtime_error("import(): cannot open '" + path.string() +
    "'.");

  std::ostringstream ss;
  ss << file.rdbuf();
  std::string source = ss.str();

  Lexer modLex(source);
  auto tokens = modLex.tokenize();
  Parser modParser(std::move(tokens));
  auto stmts = modParser.parse();

  auto mod = std::make_shared<NinModule>();
  mod->sourcePath = path.string();
  mod->ast = std::move(stmts);

  auto modEnv = std::make_shared<Environment>(interp->globals);
  interp->executeBlock(mod->ast, modEnv);
  mod->members = modEnv->exportAll();
  return mod;
}

Value LoadModuleFn::call(std::vector<Value> args) {
  if (!std::holds_alternative<std::string>(args[0]))
    throw std::runtime_error("loadmodule(): argument must be a string path.");

  std::string get_path = std::get<std::string>(args[0]);
  std::string path;
  path = get_path;
  if (!get_path.empty() and get_path.front() == '@')
    path = expand_user_path(global_capi_dir).string() + get_path.substr(1);

  void *handle = dlopen(path.c_str(), RTLD_LAZY);
  if (!handle)
    throw std::runtime_error("loadmodule(): cannot open '" + path +
                             "': " + dlerror());

  auto init = (InitFn)dlsym(handle, "carrot_module_init");
  if (!init) {
    dlclose(handle);
    throw std::runtime_error("loadmodule(): '" + path +
                             "' has no carrot_module_init symbol.");
  }

  auto mod = std::shared_ptr<NinModule>(new NinModule(), [](NinModule *m) {
    if (m->handle)
      dlclose(m->handle);
    delete m;
  });

  mod->sourcePath = path;
  mod->handle = handle;
  init(&mod->members);

  return mod;
}
