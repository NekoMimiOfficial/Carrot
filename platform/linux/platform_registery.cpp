#include "platform.h"
#include "platform_core_builtins.h"
#include <memory>

void registerPlatformBuiltins(Interpreter *interp) {
  interp->registerBuiltinFn(std::make_shared<LinuxFn>());
  interp->registerBuiltin("version", "1.0.0");
}
