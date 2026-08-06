#include "interpreter.h"
#include "methods.h"
#include <memory>

void registerHandler(Interpreter *interp) {
  interp->registerBuiltin("version", getVerString());
}
