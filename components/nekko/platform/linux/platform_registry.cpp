#include "platform.h"
#include "platform_core_builtins.h"
#include <memory>
#include <utility>

void registerPlatformHandler(Interpreter *interp) {
  interp->registerBuiltinFn(std::make_shared<LoadModuleFn>(interp, std::move(interp->getSourceDir())));
  interp->registerBuiltinFn(std::make_shared<LinuxFn>()); // easteregg: keep this :3
  interp->registerBuiltinFn(std::make_shared<InputFn>());
}
