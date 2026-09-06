#pragma once
#include <sstream>
#include <string>
#include <vector>

#define LOC_ID_LIST                                                            \
  X(UDEF_VAR)                                                                  \
  X(ASSIGN_TO_CONST_VAR)                                                       \
  X(ASSIGN_TO_UDEF_VAR)                                                        \
  X(FREE_UDEF_VAR)                                                             \
  X(PREDEFINED_VAR)                                                            \
  X(RET_EXCEPTION)                                                             \
  X(BREAK_EXCEPTION)                                                           \
  X(CONTINUE_EXCEPTION)                                                        \
  X(FN_TOO_MANY_ARGS)                                                          \
  X(FN_MISSING_ARG)                                                            \
  X(FN_UNEXPECTED_KWARG)                                                       \
  X(FN_NOT_VARIADIC)                                                           \
  X(CORO_RUNNING)                                                              \
  X(OP_PLUS_MISMATCH)                                                          \
  X(DIV_ZERO)                                                                  \
  X(EXPECT_FN)                                                                 \
  X(FN_NOT_ENOUGH_ARGS)                                                        \
  X(ARRAY_INDEX_NAN)                                                           \
  X(ARRAY_INDEX_OOB)                                                           \
  X(STRING_INDEX_NAN)                                                          \
  X(STRING_INDEX_OOB)                                                          \
  X(TYPE_UNINDEXABLE)                                                          \
  X(PARSE_TOO_MANY_PARAMS)                                                     \
  X(PARSE_TOO_MANY_ARGS)                                                       \
  X(PARSE_DEFAULT_PARAM_ORDER)                                                 \
  X(PARSE_BREAK_OUTSIDE_LOOP)                                                  \
  X(PARSE_CONTINUE_OUTSIDE_LOOP)                                               \
  X(PARSE_RETURN_OUTSIDE_FUNC)                                                 \
  X(PARSE_AWAIT_OUTSIDE_ASYNC)                                                 \
  X(PARSE_EXPECT_EXPRESSION)                                                   \
  X(PARSE_INVALID_ASSIGN_TARGET)                                               \
  X(ASSIGN_CONST_ARRAY)                                                        \
  X(INDEX_ASSIGN_NOT_ARRAY)                                                    \
  X(MODIFY_CONST_ARRAY)                                                        \
  X(MODULE_NO_MEMBER)                                                          \
  X(UNDEF_PROPERTY)                                                            \
  X(NOT_GETTABLE)                                                              \
  X(NOT_SETTABLE)                                                              \
  X(NEW_NOT_CLASS)                                                             \
  X(INIT_ARG_MISMATCH)                                                         \
  X(NO_INIT_WITH_ARGS)                                                         \
  X(SUPER_OUTSIDE_METHOD)                                                      \
  X(NO_PARENT_CLASS)                                                           \
  X(PARENT_NO_INIT)                                                            \
  X(SUPER_ARG_MISMATCH)                                                        \
  X(CORO_NOT_FN)                                                               \
  X(CORO_NOT_ASYNC)                                                            \
  X(AWAIT_OUTSIDE_CORO)                                                        \
  X(SUPERCLASS_NOT_CLASS)                                                      \
  X(METHOD_NEEDS_OVERRIDE)                                                     \
  X(OVERRIDE_NOT_IN_PARENT)                                                    \
  X(OPERAND_NOT_NUMBER)                                                        \
  X(OPERANDS_NOT_NUMBERS)                                                      \
  X(UNEXPECTED_AMP)                                                            \
  X(UNEXPECTED_PIPE)                                                           \
  X(UNEXPECTED_CHAR)                                                           \
  X(UNTERMINATED_STRING)                                                       \
  X(INVALID_HEX_LITERAL)                                                       \
  X(INVALID_BYTE_SIZE)                                                         \
  X(INVALID_NUMBER_TRAILING_CHAR)                                              \
  X(CONSUME_VAR_NAME_AFTER_GLOBAL)                                             \
  X(CONSUME_EQUAL_AFTER_GLOBAL)                                                \
  X(CONSUME_SEMI_AFTER_GLOBAL)                                                 \
  X(CONSUME_VAR_NAME_AFTER_LET)                                                \
  X(CONSUME_SEMI_AFTER_LET)                                                    \
  X(CONSUME_VAR_NAME_AFTER_CONST)                                              \
  X(CONSUME_EQUAL_AFTER_CONST)                                                 \
  X(CONSUME_SEMI_AFTER_CONST)                                                  \
  X(CONSUME_VAR_NAME_AFTER_MUTEX)                                              \
  X(CONSUME_EQUAL_AFTER_MUTEX)                                                 \
  X(CONSUME_SEMI_AFTER_MUTEX)                                                  \
  X(CONSUME_FN_NAME)                                                           \
  X(CONSUME_LPAREN_AFTER_FN_NAME)                                              \
  X(CONSUME_PARAM_NAME)                                                        \
  X(CONSUME_RPAREN_AFTER_PARAMS)                                               \
  X(CONSUME_LBRACE_BEFORE_FN_BODY)                                             \
  X(CONSUME_SEMI_AFTER_BREAK)                                                  \
  X(CONSUME_SEMI_AFTER_CONTINUE)                                               \
  X(CONSUME_LPAREN_AFTER_IF)                                                   \
  X(CONSUME_RPAREN_AFTER_IF_COND)                                              \
  X(CONSUME_LPAREN_AFTER_WHILE)                                                \
  X(CONSUME_RPAREN_AFTER_WHILE_COND)                                           \
  X(CONSUME_LPAREN_AFTER_FOR)                                                  \
  X(CONSUME_SEMI_AFTER_FOR_COND)                                               \
  X(CONSUME_RPAREN_AFTER_FOR_CLAUSES)                                          \
  X(CONSUME_SEMI_AFTER_RETURN)                                                 \
  X(CONSUME_RBRACE_CLOSE_BLOCK)                                                \
  X(CONSUME_SEMI_AFTER_EXPR)                                                   \
  X(CONSUME_CLASS_NAME)                                                        \
  X(CONSUME_PARENT_CLASS_NAME)                                                 \
  X(CONSUME_LBRACE_BEFORE_CLASS_BODY)                                          \
  X(CONSUME_FUN_AFTER_OVERRIDE)                                                \
  X(CONSUME_FUN_IN_CLASS_BODY)                                                 \
  X(CONSUME_RBRACE_AFTER_CLASS_BODY)                                           \
  X(CONSUME_VAR_NAME_AFTER_FREE)                                               \
  X(CONSUME_SEMI_AFTER_FREE)                                                   \
  X(CONSUME_RPAREN_AFTER_ARGS)                                                 \
  X(CONSUME_RBRACKET_AFTER_INDEX)                                              \
  X(CONSUME_PROP_NAME_AFTER_DOT)                                               \
  X(CONSUME_RBRACKET_AFTER_ARRAY)                                              \
  X(CONSUME_RPAREN_AFTER_EXPR)                                                 \
  X(CONSUME_CLASS_NAME_AFTER_NEW)                                              \
  X(CONSUME_LPAREN_AFTER_CLASS_NAME)                                           \
  X(CONSUME_FN_NAME_AFTER_COROUTINE)                                           \
  X(CONSUME_RPAREN_GENERIC)                                                    \
  X(CONSUME_LPAREN_AFTER_SUPER)                                                \
  X(CONSUME_RPAREN_AFTER_SUPER_ARGS)                                           \
  X(CONSUME_FUN_AFTER_ASYNC)

enum LOC_IDs {
#define X(name) name,
  LOC_ID_LIST
#undef X
      LOC_ID_COUNT
};

void initLocale();

std::string LOC_Internal(int id, const std::vector<std::string> &args);
std::string LOC_Internal(const std::string &key,
                         const std::vector<std::string> &args);

inline std::string LOC_ToString(const std::string &v) { return v; }
inline std::string LOC_ToString(const char *v) { return v; }
inline std::string LOC_ToString(bool v) { return v ? "true" : "false"; }

template <typename T> std::string LOC_ToString(T &&val) {
  std::ostringstream oss;
  oss << val;
  return oss.str();
}

template <typename... Args> std::string LOC(int id, Args &&...args) {
  return LOC_Internal(id, {LOC_ToString(std::forward<Args>(args))...});
}

template <typename... Args>
std::string LOC(const std::string &key, Args &&...args) {
  return LOC_Internal(key, {LOC_ToString(std::forward<Args>(args))...});
}
