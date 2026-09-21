#include "interpreter.h"
#include "builtin.h"
#include "builtins/cstrtools.h"
#include <memory>

void registerHandler(Interpreter *interp) {
  interp->registerBuiltin("version", getVerString());
  interp->registerBuiltinFn(std::make_shared<ImportFn>(interp, std::move(interp->getSourceDir())));

  interp->registerBuiltinFn(std::make_shared<SystemFn>());
  interp->registerBuiltinFn(std::make_shared<ClockFn>());
  interp->registerBuiltinFn(std::make_shared<ArgvFn>(interp->getArgV()));
  interp->registerBuiltinFn(std::make_shared<ExitFn>());
  interp->registerBuiltinFn(std::make_shared<SleepFn>());

  interp->registerBuiltinFn(std::make_shared<NumFn>());
  interp->registerBuiltinFn(std::make_shared<TypeFn>());
  interp->registerBuiltinFn(std::make_shared<LenFn>());
  interp->registerBuiltinFn(std::make_shared<PushFn>());
  interp->registerBuiltinFn(std::make_shared<PopFn>());
  interp->registerBuiltin("string", StringTools());
}
