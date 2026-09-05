#include "lang.h"
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
    ss << "[WAT IZ DIS TYPE]";
  }
}

std::string LOC_Internal(int id, std::initializer_list<std::any> args) {
  std::string_view fmt = "";

  switch (id) {
    case UDEF_VAR:
      fmt = "I DUNNO WAT DIS VAR '{}' IZ."; break;
    case ASSIGN_TO_CONST_VAR:
      fmt = "U NO CAN CHANGE CONST '{}'!!"; break;
    case ASSIGN_TO_UDEF_VAR:
      fmt = "U TRYNA GIV CHEEZBURGER TO GHOST VAR '{}'."; break;
    case FREE_UDEF_VAR:
      fmt = "CANNOT LET GO OF NUFFIN '{}'."; break;
    case PREDEFINED_VAR:
      fmt = "'{}' IZ ALREADY HERE MEOOOW."; break;
    case RET_EXCEPTION:
      fmt = "'return' IZ LOST OUTSIDE FUNKSHUN."; break;
    case BREAK_EXCEPTION:
      fmt = "'break' IZ LOST OUTSIDE LOOP."; break;
    case CONTINUE_EXCEPTION:
      fmt = "'continue' IZ OUTSIDE LOOP, HALP."; break;
    case FN_TOO_MANY_ARGS:
      fmt = "'{}' WANTZ MOST {} CHEEZBURGERZ BUT GOT {}."; break;
    case FN_MISSING_ARG:
      fmt = "'{}' MISSING DA CHEEZBURGER '{}'."; break;
    case FN_UNEXPECTED_KWARG:
      fmt = "'{}' DID NOT WANT '{}', Y U GIV?"; break;
    case FN_NOT_VARIADIC:
      fmt = "'{}' NO CAN HAZ KEYWORD ARGZ."; break;
    case CORO_RUNNING:
      fmt = "COROUTINE IZ ALREADY RUNNIN OR SLEEPIN."; break;
    case OP_PLUS_MISMATCH:
      fmt = "NUMBERS OR STRINGZ ONLY FOR '+', SILLY HOOMAN. (line {})"; break;
    case DIV_ZERO:
      fmt = "U DIVIDED BY NUTHIN. OH NOES. (line {})"; break;
    case EXPECT_FN:
      fmt = "U CAN ONLY CALL FUNKSHUNZ. (line {})"; break;
    case FN_NOT_ENOUGH_ARGS:
      fmt = "'{}' WANTZ {} TINGZ BUT GOT {}. (line {})"; break;
    case ARRAY_INDEX_NAN:
      fmt = "ARRAY INDEX NEEDZ TO BE NUMBAR."; break;
    case ARRAY_INDEX_OOB:
      fmt = "ARRAY INDEX {} IS IN DA VOID."; break;
    case STRING_INDEX_NAN:
      fmt = "STRING INDEX NEEDZ TO BE NUMBAR."; break;
    case STRING_INDEX_OOB:
      fmt = "STRING INDEX {} IZ TOO FAR, CANT REACH."; break;
    case TYPE_UNINDEXABLE:
      fmt = "U NO CAN INDEX DIS THING. (line {})"; break;
    case PARSE_TOO_MANY_PARAMS:
      fmt = "WAY TOO MANY PARAMZ, CAT CANT COUNT PAST 255."; break;
    case PARSE_TOO_MANY_ARGS:
      fmt = "WAY TOO MANY ARGZ, CAT CANT COUNT PAST 255."; break;
    case PARSE_DEFAULT_PARAM_ORDER:
      fmt = "PARAM '{}' WIF NO DEFAULT CAN NO FOLLOW ONE DAT HAS."; break;
    case PARSE_BREAK_OUTSIDE_LOOP:
      fmt = "'break' WENT WANDERING OUTSIDE LOOP. (line {})"; break;
    case PARSE_CONTINUE_OUTSIDE_LOOP:
      fmt = "'continue' WENT WANDERING OUTSIDE LOOP. (line {})"; break;
    case PARSE_RETURN_OUTSIDE_FUNC:
      fmt = "'return' IZ NOT IN FUNKSHUN. (line {})"; break;
    case PARSE_AWAIT_OUTSIDE_ASYNC:
      fmt = "'await' ONLY LIVES IN ASYNC FUNKSHUN. (line {})"; break;
    case PARSE_EXPECT_EXPRESSION:
      fmt = "I EXPECTED A THING AT LINE {}, BUT U GAVE ME BAD YARN '{}'."; break;
    case PARSE_INVALID_ASSIGN_TARGET:
      fmt = "BAD PLACE TO PUT CHEEZBURGER AT LINE {}."; break;
    case ASSIGN_CONST_ARRAY:
      fmt = "U NO CAN CHANGE CONST ARRAY '{}'."; break;
    case INDEX_ASSIGN_NOT_ARRAY:
      fmt = "ONLY ARRAYZ GET DIS INDEX MOUSE."; break;
    case MODIFY_CONST_ARRAY:
      fmt = "CAN NO SCRATCH CONST ARRAY."; break;
    case MODULE_NO_MEMBER:
      fmt = "MODULE HIDEZ NO '{}' TO FIND."; break;
    case UNDEF_PROPERTY:
      fmt = "I DUNNO WAT PROPERTY '{}' IZ."; break;
    case NOT_GETTABLE:
      fmt = "ONLY MODULEZ N INSTANCEZ HAVE TOYZ. (line {})"; break;
    case NOT_SETTABLE:
      fmt = "ONLY MODULEZ N INSTANCEZ CAN HAVE TOYZ CHANGED. (line {})"; break;
    case NEW_NOT_CLASS:
      fmt = "DIS NOT A CLASS AFTER 'new'. WAT R U DOIN? (line {})"; break;
    case INIT_ARG_MISMATCH:
      fmt = "'{}.init' WANTZ {} SNACKZ BUT GOT {}."; break;
    case NO_INIT_WITH_ARGS:
      fmt = "CLASS '{}' HAS NO 'init' BUT U GAVE IT TOYZ ANYWAY."; break;
    case SUPER_OUTSIDE_METHOD:
      fmt = "'super' NO IN DA METHOD, Y U DO DIS?"; break;
    case NO_PARENT_CLASS:
      fmt = "CLASS '{}' NO HAVE MOMMA OR POPPA."; break;
    case PARENT_NO_INIT:
      fmt = "MOMMA CLASS NO GOTS 'init'."; break;
    case SUPER_ARG_MISMATCH:
      fmt = "super() WANTZ {} NOMZ BUT GOT {}."; break;
    case CORO_NOT_FN:
      fmt = "COROUTINE: '{}' IZ NOT A FUNKSHUN, HOOMAN."; break;
    case CORO_NOT_ASYNC:
      fmt = "COROUTINE: '{}' IZ NOT AN ASYNC FUNKSHUN, MEOW."; break;
    case AWAIT_OUTSIDE_CORO:
      fmt = "'await' IS LOST FROM DA COROUTINE. (line {})"; break;
    case SUPERCLASS_NOT_CLASS:
      fmt = "'{}' IZ NOT A CLASS."; break;
    case METHOD_NEEDS_OVERRIDE:
      fmt = "MOMMA CLASS ALREADY HAZ '{}'. U MUST 'override' IT WIF CLAWZ."; break;
    case OVERRIDE_NOT_IN_PARENT:
      fmt = "CAN NO 'override' '{}': MOMMA DO NOT HAVE IT."; break;
    case OPERAND_NOT_NUMBER:
      fmt = "THING OF '{}' MUST BE A NUMBAR. (line {})"; break;
    case OPERANDS_NOT_NUMBERS:
      fmt = "ALL TINGS OF '{}' MUST BE NUMBARZ. (line {})"; break;
    case UNEXPECTED_AMP:
      fmt = "WAT IS DIS '&' AT LINE {}? U WANT '&&'?"; break;
    case UNEXPECTED_PIPE:
      fmt = "WAT IS DIS '|' AT LINE {}? U WANT '||'?"; break;
    case UNEXPECTED_CHAR:
      fmt = "U SCARED ME WIF '{}' AT LINE {}."; break;
    case UNTERMINATED_STRING:
      fmt = "STRING NO ENDZ AT LINE {}. FOREVER YARN."; break;
    case INVALID_HEX_LITERAL:
      fmt = "DIS HEX IS BROKEN, NEED MOAR DIGITZ AFTER '0x' AT LINE: {}."; break;
    case INVALID_BYTE_SIZE:
      fmt = "BAD BYTE SIZE, BYTES IZ 2 LETTERZ LONG, GOT: {} AT LINE: {}."; break;
    case INVALID_NUMBER_TRAILING_CHAR:
      fmt = "BAD LETTER '{}' ON DA END OF NUMBAR AT LINE: {}."; break;
    case CONSUME_VAR_NAME_AFTER_GLOBAL:
      fmt = "I NEEDZ VAR NAME AFTER 'global' PLZ."; break;
    case CONSUME_EQUAL_AFTER_GLOBAL:
      fmt = "I NEEDZ '=' AFTER GLOBAL VAR PLZ."; break;
    case CONSUME_SEMI_AFTER_GLOBAL:
      fmt = "I NEEDZ ';' AFTER GLOBAL TING PLZ."; break;
    case CONSUME_VAR_NAME_AFTER_LET:
      fmt = "I NEEDZ VAR NAME AFTER 'let' PLZ."; break;
    case CONSUME_SEMI_AFTER_LET:
      fmt = "I NEEDZ ';' AFTER VAR TING PLZ."; break;
    case CONSUME_VAR_NAME_AFTER_CONST:
      fmt = "I NEEDZ VAR NAME AFTER 'const' PLZ."; break;
    case CONSUME_EQUAL_AFTER_CONST:
      fmt = "I NEEDZ '=' AFTER CONST VAR PLZ."; break;
    case CONSUME_SEMI_AFTER_CONST:
      fmt = "I NEEDZ ';' AFTER CONST TING PLZ."; break;
    case CONSUME_VAR_NAME_AFTER_MUTEX:
      fmt = "I NEEDZ VAR NAME AFTER 'mutex' PLZ."; break;
    case CONSUME_EQUAL_AFTER_MUTEX:
      fmt = "I NEEDZ '=' AFTER MUTEX VAR PLZ."; break;
    case CONSUME_SEMI_AFTER_MUTEX:
      fmt = "I NEEDZ ';' AFTER MUTEX TING PLZ."; break;
    case CONSUME_FN_NAME:
      fmt = "WHERE IZ FUNKSHUN NAME?"; break;
    case CONSUME_LPAREN_AFTER_FN_NAME:
      fmt = "NEEDZ '(' AFTER FUNKSHUN NAME, KTHX."; break;
    case CONSUME_PARAM_NAME:
      fmt = "NEEDZ PARAM NAME PLZ."; break;
    case CONSUME_RPAREN_AFTER_PARAMS:
      fmt = "NEEDZ ')' AFTER PARAMS PLZ."; break;
    case CONSUME_LBRACE_BEFORE_FN_BODY:
      fmt = "NEEDZ '{' B4 FUNKSHUN BELLY."; break;
    case CONSUME_SEMI_AFTER_BREAK:
      fmt = "NEEDZ ';' AFTER 'break'."; break;
    case CONSUME_SEMI_AFTER_CONTINUE:
      fmt = "NEEDZ ';' AFTER 'continue'."; break;
    case CONSUME_LPAREN_AFTER_IF:
      fmt = "NEEDZ '(' AFTER 'if'."; break;
    case CONSUME_RPAREN_AFTER_IF_COND:
      fmt = "NEEDZ ')' AFTER IF THINGY."; break;
    case CONSUME_LPAREN_AFTER_WHILE:
      fmt = "NEEDZ '(' AFTER 'while'."; break;
    case CONSUME_RPAREN_AFTER_WHILE_COND:
      fmt = "NEEDZ ')' AFTER WHILE THINGY."; break;
    case CONSUME_LPAREN_AFTER_FOR:
      fmt = "NEEDZ '(' AFTER 'for'."; break;
    case CONSUME_SEMI_AFTER_FOR_COND:
      fmt = "NEEDZ ';' AFTER FOR LOOP THINGY."; break;
    case CONSUME_RPAREN_AFTER_FOR_CLAUSES:
      fmt = "NEEDZ ')' AFTER FOR BITZ."; break;
    case CONSUME_SEMI_AFTER_RETURN:
      fmt = "NEEDZ ';' AFTER RETURN TING."; break;
    case CONSUME_RBRACE_CLOSE_BLOCK:
      fmt = "NEEDZ '}' TO CLOSE DA BOX."; break;
    case CONSUME_SEMI_AFTER_EXPR:
      fmt = "NEEDZ ';' AFTER EXPRESSHUN."; break;
    case CONSUME_CLASS_NAME:
      fmt = "WHERE IZ CLASS NAME?"; break;
    case CONSUME_PARENT_CLASS_NAME:
      fmt = "NEEDZ MOMMA CLASS NAME AFTER ':'."; break;
    case CONSUME_LBRACE_BEFORE_CLASS_BODY:
      fmt = "NEEDZ '{' B4 CLASS BELLY."; break;
    case CONSUME_FUN_AFTER_OVERRIDE:
      fmt = "NEEDZ 'fun' AFTER 'override'."; break;
    case CONSUME_FUN_IN_CLASS_BODY:
      fmt = "NEEDZ 'fun' IN CLASS BELLY."; break;
    case CONSUME_RBRACE_AFTER_CLASS_BODY:
      fmt = "NEEDZ '}' AFTER CLASS BELLY."; break;
    case CONSUME_VAR_NAME_AFTER_FREE:
      fmt = "NEEDZ VAR NAME AFTER 'free'."; break;
    case CONSUME_SEMI_AFTER_FREE:
      fmt = "NEEDZ ';' AFTER FREE TING."; break;
    case CONSUME_RPAREN_AFTER_ARGS:
      fmt = "NEEDZ ')' AFTER ARGZ."; break;
    case CONSUME_RBRACKET_AFTER_INDEX:
      fmt = "NEEDZ ']' AFTER INDEX TING."; break;
    case CONSUME_PROP_NAME_AFTER_DOT:
      fmt = "NEEDZ TOY NAME AFTER '.'."; break;
    case CONSUME_RBRACKET_AFTER_ARRAY:
      fmt = "NEEDZ ']' AFTER ARRAY NOMZ."; break;
    case CONSUME_RPAREN_AFTER_EXPR:
      fmt = "NEEDZ ')' AFTER EXPRESSHUN."; break;
    case CONSUME_CLASS_NAME_AFTER_NEW:
      fmt = "NEEDZ CLASS NAME AFTER 'new'."; break;
    case CONSUME_LPAREN_AFTER_CLASS_NAME:
      fmt = "NEEDZ '(' AFTER CLASS NAME."; break;
    case CONSUME_FN_NAME_AFTER_COROUTINE:
      fmt = "NEEDZ FUNKSHUN NAME AFTER 'coroutine'."; break;
    case CONSUME_RPAREN_GENERIC:
      fmt = "GIMME ')', PLZ."; break;
    case CONSUME_LPAREN_AFTER_SUPER:
      fmt = "NEEDZ '(' AFTER 'super'."; break;
    case CONSUME_RPAREN_AFTER_SUPER_ARGS:
      fmt = "NEEDZ ')' AFTER SUPER NOMZ."; break;
    case CONSUME_FUN_AFTER_ASYNC:
      fmt = "NEEDZ 'fun' AFTER 'async'."; break;

    default:
      fmt = "I DUNNO DIS ERROR. MEOW.";
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
