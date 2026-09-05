#include "locale.h"
#include <any>
#include <sstream>
#include <string>
#include <string_view>
#include <utility>

void PrintAnyToStream(std::stringstream &ss, const std::any &val) {
  if (auto p = std::any_cast<const char *>(&val)) {
    ss << *p;
  } else if (auto p = std::any_cast<std::string>(&val)) {
    ss << *p;
  } else if (auto p = std::any_cast<std::string_view>(&val)) {
    ss << *p;
  } else if (auto p = std::any_cast<int>(&val)) {
    ss << *p;
  } else if (auto p = std::any_cast<double>(&val)) {
    ss << *p;
  } else if (auto p = std::any_cast<float>(&val)) {
    ss << *p;
  } else {
    ss << "[Unsupported Type]";
  }
}

std::string LOC_Internal(int id, std::initializer_list<std::any> args) {
  std::string_view fmt = "";

  switch (id) {
    case UDEF_VAR:
      fmt = "Undefined variable '{}'."; break;
    case ASSIGN_TO_CONST_VAR:
      fmt = "Cannot assign to const '{}'."; break;
    case ASSIGN_TO_UDEF_VAR:
      fmt = "Cannot assign to undefined variable '{}'."; break;
    case FREE_UDEF_VAR:
      fmt = "Cannot free undefined variable '{}'."; break;
    case PREDEFINED_VAR:
      fmt = "'{}' is already defined."; break;
    case RET_EXCEPTION:
      fmt = "'return' used outside of a function."; break;
    case BREAK_EXCEPTION:
      fmt = "'break' used outside of a loop."; break;
    case CONTINUE_EXCEPTION:
      fmt = "'continue' used outside of a loop."; break;
    case FN_TOO_MANY_ARGS:
      fmt = "'{}' expects at most {} argument(s) but got {}."; break;
    case FN_MISSING_ARG:
      fmt = "'{}' missing required argument '{}'."; break;
    case FN_UNEXPECTED_KWARG:
      fmt = "'{}' got unexpected keyword argument '{}'."; break;
    case FN_NOT_VARIADIC:
      fmt = "'{}' does not accept keyword arguments."; break;
    case CORO_RUNNING:
      fmt = "coroutine is already running or finished."; break;
    case OP_PLUS_MISMATCH:
      fmt = "Operands of '+' must be numbers or strings. (line {})"; break;
    case DIV_ZERO:
      fmt = "Division by zero. (line {})"; break;
    case EXPECT_FN:
      fmt = "Can only call functions. (line {})"; break;
    case FN_NOT_ENOUGH_ARGS:
      fmt = "'{}' expects {} argument(s) but got {}. (line {})"; break;
    case ARRAY_INDEX_NAN:
      fmt = "Array index must be a number."; break;
    case ARRAY_INDEX_OOB:
      fmt = "Array index {} out of bounds."; break;
    case STRING_INDEX_NAN:
      fmt = "String index must be a number."; break;
    case STRING_INDEX_OOB:
      fmt = "String index {} out of bounds."; break;
    case TYPE_UNINDEXABLE:
      fmt = "Cannot index into this type. (line {})"; break;
    case PARSE_TOO_MANY_PARAMS:
      fmt = "Cannot have more than 255 parameters."; break;
    case PARSE_TOO_MANY_ARGS:
      fmt = "Cannot have more than 255 arguments."; break;
    case PARSE_DEFAULT_PARAM_ORDER:
      fmt = "Parameter '{}' without a default cannot follow one that has a default."; break;
    case PARSE_BREAK_OUTSIDE_LOOP:
      fmt = "'break' used outside of a loop. (line {})"; break;
    case PARSE_CONTINUE_OUTSIDE_LOOP:
      fmt = "'continue' used outside of a loop. (line {})"; break;
    case PARSE_RETURN_OUTSIDE_FUNC:
      fmt = "'return' used outside of a function. (line {})"; break;
    case PARSE_AWAIT_OUTSIDE_ASYNC:
      fmt = "'await' used outside an async function. (line {})"; break;
    case PARSE_EXPECT_EXPRESSION:
      fmt = "Expected expression at line {}, got unexpected token '{}'."; break;
    case PARSE_INVALID_ASSIGN_TARGET:
      fmt = "Invalid assignment target at line {}."; break;
    case ASSIGN_CONST_ARRAY:
      fmt = "Cannot modify const array '{}'."; break;
    case INDEX_ASSIGN_NOT_ARRAY:
      fmt = "Can only index-assign into arrays."; break;
    case MODIFY_CONST_ARRAY:
      fmt = "Cannot modify a const array."; break;
    case MODULE_NO_MEMBER:
      fmt = "Module has no member '{}'."; break;
    case UNDEF_PROPERTY:
      fmt = "Undefined property '{}'."; break;
    case NOT_GETTABLE:
      fmt = "Only modules and instances have properties. (line {})"; break;
    case NOT_SETTABLE:
      fmt = "Only modules and instances have settable properties. (line {})"; break;
    case NEW_NOT_CLASS:
      fmt = "Expression after 'new' is not a class. (line {})"; break;
    case INIT_ARG_MISMATCH:
      fmt = "'{}.init' expects {} argument(s) but got {}."; break;
    case NO_INIT_WITH_ARGS:
      fmt = "Class '{}' has no 'init' but was called with arguments."; break;
    case SUPER_OUTSIDE_METHOD:
      fmt = "'super' used outside a class method."; break;
    case NO_PARENT_CLASS:
      fmt = "Class '{}' has no parent class."; break;
    case PARENT_NO_INIT:
      fmt = "Parent class has no 'init' method."; break;
    case SUPER_ARG_MISMATCH:
      fmt = "super() expects {} argument(s) but got {}."; break;
    case CORO_NOT_FN:
      fmt = "coroutine: '{}' is not a function."; break;
    case CORO_NOT_ASYNC:
      fmt = "coroutine: '{}' is not an async function."; break;
    case AWAIT_OUTSIDE_CORO:
      fmt = "'await' used outside a running coroutine. (line {})"; break;
    case SUPERCLASS_NOT_CLASS:
      fmt = "'{}' is not a class."; break;
    case METHOD_NEEDS_OVERRIDE:
      fmt = "Method '{}' exists in parent class. Use 'override' to replace it."; break;
    case OVERRIDE_NOT_IN_PARENT:
      fmt = "Cannot override '{}': not defined in parent class."; break;
    case OPERAND_NOT_NUMBER:
      fmt = "Operand of '{}' must be a number. (line {})"; break;
    case OPERANDS_NOT_NUMBERS:
      fmt = "Both operands of '{}' must be numbers. (line {})"; break;
    case UNEXPECTED_AMP:
      fmt = "Unexpected character '&' at line {}. Did you mean '&&'?"; break;
    case UNEXPECTED_PIPE:
      fmt = "Unexpected character '|' at line {}. Did you mean '||'?"; break;
    case UNEXPECTED_CHAR:
      fmt = "Unexpected character '{}' at line {}."; break;
    case UNTERMINATED_STRING:
      fmt = "Unterminated string starting at line {}."; break;
    case INVALID_HEX_LITERAL:
      fmt = "Invalid hex literal, expected hex digits after '0x' at line: {}."; break;
    case INVALID_BYTE_SIZE:
      fmt = "Incorrect byte size, bytes are 2 characters long, got: {} at line: {}."; break;
    case INVALID_NUMBER_TRAILING_CHAR:
      fmt = "Invalid character '{}' trailing number literal at line: {}."; break;
    case CONSUME_VAR_NAME_AFTER_GLOBAL:
      fmt = "Expected variable name after 'global'."; break;
    case CONSUME_EQUAL_AFTER_GLOBAL:
      fmt = "Expected '=' after global variable name."; break;
    case CONSUME_SEMI_AFTER_GLOBAL:
      fmt = "Expected ';' after global declaration."; break;
    case CONSUME_VAR_NAME_AFTER_LET:
      fmt = "Expected variable name after 'let'."; break;
    case CONSUME_SEMI_AFTER_LET:
      fmt = "Expected ';' after variable declaration."; break;
    case CONSUME_VAR_NAME_AFTER_CONST:
      fmt = "Expected variable name after 'const'."; break;
    case CONSUME_EQUAL_AFTER_CONST:
      fmt = "Expected '=' after const variable name."; break;
    case CONSUME_SEMI_AFTER_CONST:
      fmt = "Expected ';' after const declaration."; break;
    case CONSUME_VAR_NAME_AFTER_MUTEX:
      fmt = "Expected variable name after 'mutex'."; break;
    case CONSUME_EQUAL_AFTER_MUTEX:
      fmt = "Expected '=' after mutex variable name."; break;
    case CONSUME_SEMI_AFTER_MUTEX:
      fmt = "Expected ';' after mutex declaration."; break;
    case CONSUME_FN_NAME:
      fmt = "Expected function name."; break;
    case CONSUME_LPAREN_AFTER_FN_NAME:
      fmt = "Expected '(' after function name."; break;
    case CONSUME_PARAM_NAME:
      fmt = "Expected parameter name."; break;
    case CONSUME_RPAREN_AFTER_PARAMS:
      fmt = "Expected ')' after parameters."; break;
    case CONSUME_LBRACE_BEFORE_FN_BODY:
      fmt = "Expected '{' before function body."; break;
    case CONSUME_SEMI_AFTER_BREAK:
      fmt = "Expected ';' after 'break'."; break;
    case CONSUME_SEMI_AFTER_CONTINUE:
      fmt = "Expected ';' after 'continue'."; break;
    case CONSUME_LPAREN_AFTER_IF:
      fmt = "Expected '(' after 'if'."; break;
    case CONSUME_RPAREN_AFTER_IF_COND:
      fmt = "Expected ')' after if condition."; break;
    case CONSUME_LPAREN_AFTER_WHILE:
      fmt = "Expected '(' after 'while'."; break;
    case CONSUME_RPAREN_AFTER_WHILE_COND:
      fmt = "Expected ')' after while condition."; break;
    case CONSUME_LPAREN_AFTER_FOR:
      fmt = "Expected '(' after 'for'."; break;
    case CONSUME_SEMI_AFTER_FOR_COND:
      fmt = "Expected ';' after for-loop condition."; break;
    case CONSUME_RPAREN_AFTER_FOR_CLAUSES:
      fmt = "Expected ')' after for clauses."; break;
    case CONSUME_SEMI_AFTER_RETURN:
      fmt = "Expected ';' after return value."; break;
    case CONSUME_RBRACE_CLOSE_BLOCK:
      fmt = "Expected '}' to close block."; break;
    case CONSUME_SEMI_AFTER_EXPR:
      fmt = "Expected ';' after expression."; break;
    case CONSUME_CLASS_NAME:
      fmt = "Expected class name."; break;
    case CONSUME_PARENT_CLASS_NAME:
      fmt = "Expected parent class name after ':'."; break;
    case CONSUME_LBRACE_BEFORE_CLASS_BODY:
      fmt = "Expected '{' before class body."; break;
    case CONSUME_FUN_AFTER_OVERRIDE:
      fmt = "Expected 'fun' after 'override'."; break;
    case CONSUME_FUN_IN_CLASS_BODY:
      fmt = "Expected 'fun' in class body."; break;
    case CONSUME_RBRACE_AFTER_CLASS_BODY:
      fmt = "Expected '}' after class body."; break;
    case CONSUME_VAR_NAME_AFTER_FREE:
      fmt = "Expected variable name after 'free'."; break;
    case CONSUME_SEMI_AFTER_FREE:
      fmt = "Expected ';' after free statement."; break;
    case CONSUME_RPAREN_AFTER_ARGS:
      fmt = "Expected ')' after arguments."; break;
    case CONSUME_RBRACKET_AFTER_INDEX:
      fmt = "Expected ']' after index."; break;
    case CONSUME_PROP_NAME_AFTER_DOT:
      fmt = "Expected property name after '.'."; break;
    case CONSUME_RBRACKET_AFTER_ARRAY:
      fmt = "Expected ']' after array elements."; break;
    case CONSUME_RPAREN_AFTER_EXPR:
      fmt = "Expected ')' after expression."; break;
    case CONSUME_CLASS_NAME_AFTER_NEW:
      fmt = "Expected class name after 'new'."; break;
    case CONSUME_LPAREN_AFTER_CLASS_NAME:
      fmt = "Expected '(' after class name."; break;
    case CONSUME_FN_NAME_AFTER_COROUTINE:
      fmt = "Expected function name after 'coroutine'."; break;
    case CONSUME_RPAREN_GENERIC:
      fmt = "Expected ')'."; break;
    case CONSUME_LPAREN_AFTER_SUPER:
      fmt = "Expected '(' after 'super'."; break;
    case CONSUME_RPAREN_AFTER_SUPER_ARGS:
      fmt = "Expected ')' after super arguments."; break;
    case CONSUME_FUN_AFTER_ASYNC:
      fmt = "Expected 'fun' after 'async'."; break;

    default:
      fmt = "Unknown Message ID";
      break;
  }

  std::stringstream ss;
  size_t current_pos = 0;
  auto arg_it = args.begin();

  while (true) {
    size_t next_pos = fmt.find("{}", current_pos);
    if (next_pos == std::string_view::npos || arg_it == args.end()) {
      ss << fmt.substr(current_pos);
      break;
    }

    ss << fmt.substr(current_pos, next_pos - current_pos);
    PrintAnyToStream(ss, *arg_it);

    current_pos = next_pos + 2;
    ++arg_it;
  }

  return ss.str();
}
