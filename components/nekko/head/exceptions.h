#pragma once
#include "interpreter.h"
#include "value.h"
#include <string>

void registerExceptionTypes(Interpreter *interp);
std::string exceptionText(const Value &v);
int exceptionRet(const Value &v);
Value makeRuntimeError(Interpreter &interp, const std::string &message);
bool isInstanceOf(const Value &v, const std::shared_ptr<NinClass> &klass);
