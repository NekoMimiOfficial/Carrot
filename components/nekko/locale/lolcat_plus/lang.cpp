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
    ss << "[da wat? wut yuu gib mii?]";
  }
}

std::string LOC_Internal(int id, std::initializer_list<std::any> args) {
  std::string_view fmt = "";

  switch (id) {
    case UDEF_VAR:
      fmt = "no can find '{}', wut dat iz?"; break;
    case ASSIGN_TO_CONST_VAR:
      fmt = "'{}' iz const, u no can touch."; break;
    case ASSIGN_TO_UDEF_VAR:
      fmt = "no can put in '{}', it no exist yet, maek wif 'let' first plz."; break;
    case FREE_UDEF_VAR:
      fmt = "no can free '{}', it wuz nevr caught."; break;
    case PREDEFINED_VAR:
      fmt = "'{}' iz already a thing, no can maek twice."; break;
    case RET_EXCEPTION:
      fmt = "'return' outside fun?? dis not fun, dis confusion."; break;
    case BREAK_EXCEPTION:
      fmt = "'break' but no loop 2 breek, sad."; break;
    case CONTINUE_EXCEPTION:
      fmt = "'continue' but no loop 2 continue in, confuzzled."; break;
    case FN_TOO_MANY_ARGS:
      fmt = "'{}' wants only {} thing(s) but u gaved it {}, tuu much cheezburger."; break;
    case FN_MISSING_ARG:
      fmt = "'{}' iz hungry, it needs '{}' plz feed it."; break;
    case FN_UNEXPECTED_KWARG:
      fmt = "'{}' no ordered '{}', dat not on teh menu."; break;
    case FN_NOT_VARIADIC:
      fmt = "'{}' dusnt do namey-args, jus give normal ones kthx."; break;
    case CORO_RUNNING:
      fmt = "coroutine already runnin or allready done runnin, cant do it agin."; break;
    case OP_PLUS_MISMATCH:
      fmt = "'+' wants numbrz or wordz, not dis. (line {})"; break;
    case DIV_ZERO:
      fmt = "cant div by zero, math brok. (line {})"; break;
    case EXPECT_FN:
      fmt = "only can call funs, dis not fun. (line {})"; break;
    case FN_NOT_ENOUGH_ARGS:
      fmt = "'{}' wants {} thing(s) but only gots {}. (line {})"; break;
    case ARRAY_INDEX_NAN:
      fmt = "array index gotta b numbr, not dis."; break;
    case ARRAY_INDEX_OOB:
      fmt = "index {} iz outside teh array, fell off teh edge."; break;
    case STRING_INDEX_NAN:
      fmt = "string index gotta b numbr too."; break;
    case STRING_INDEX_OOB:
      fmt = "index {} iz outside teh word, oopsie."; break;
    case TYPE_UNINDEXABLE:
      fmt = "cant [] into dis kind of thing. (line {})"; break;
    case PARSE_TOO_MANY_PARAMS:
      fmt = "255 paramz iz teh max, u haz tuu many, calm down."; break;
    case PARSE_TOO_MANY_ARGS:
      fmt = "255 argz iz teh max, dat enuff cheezburgers."; break;
    case PARSE_DEFAULT_PARAM_ORDER:
      fmt = "'{}' gots no default but comes after one dat duz, put it first plz."; break;
    case PARSE_BREAK_OUTSIDE_LOOP:
      fmt = "'break' but no loop here 2 breek. (line {})"; break;
    case PARSE_CONTINUE_OUTSIDE_LOOP:
      fmt = "'continue' but no loop here 2 continue. (line {})"; break;
    case PARSE_RETURN_OUTSIDE_FUNC:
      fmt = "'return' but no fun 2 return frum. (line {})"; break;
    case PARSE_AWAIT_OUTSIDE_ASYNC:
      fmt = "'await' but dis fun not async, cant wait here. (line {})"; break;
    case PARSE_EXPECT_EXPRESSION:
      fmt = "wuz expectin sumthing at line {}, but got dis weird token '{}' insted."; break;
    case PARSE_INVALID_ASSIGN_TARGET:
      fmt = "cant put stuff in dat at line {}, it no haz a slot."; break;
    case ASSIGN_CONST_ARRAY:
      fmt = "array '{}' iz const, hands off."; break;
    case INDEX_ASSIGN_NOT_ARRAY:
      fmt = "only can put stuff into arrays wif [], dis no array."; break;
    case MODIFY_CONST_ARRAY:
      fmt = "dis array iz const, no touchy."; break;
    case MODULE_NO_MEMBER:
      fmt = "module dusnt haz '{}' inside, checked evrywhere."; break;
    case UNDEF_PROPERTY:
      fmt = "no property called '{}' on dis, sowwy."; break;
    case NOT_GETTABLE:
      fmt = "only modules n instances gots propertys 2 get. (line {})"; break;
    case NOT_SETTABLE:
      fmt = "only modules n instances let u set propertys. (line {})"; break;
    case NEW_NOT_CLASS:
      fmt = "u used 'new' on sumthing dat no iz a class. (line {})"; break;
    case INIT_ARG_MISMATCH:
      fmt = "'{}.init' wants {} thing(s) but got {}, oof."; break;
    case NO_INIT_WITH_ARGS:
      fmt = "class '{}' gots no init, y u give it argz."; break;
    case SUPER_OUTSIDE_METHOD:
      fmt = "'super' outside a class method?? dat no make sense."; break;
    case NO_PARENT_CLASS:
      fmt = "class '{}' gots no parent, iz an orphan class."; break;
    case PARENT_NO_INIT:
      fmt = "parent class gots no init 2 call, nuthin happen."; break;
    case SUPER_ARG_MISMATCH:
      fmt = "super() wants {} thing(s) but got {}."; break;
    case CORO_NOT_FN:
      fmt = "coroutine: '{}' iz not even a fun."; break;
    case CORO_NOT_ASYNC:
      fmt = "coroutine: '{}' iz a fun but not an async fun, cant coroutine dat."; break;
    case AWAIT_OUTSIDE_CORO:
      fmt = "'await' but no coroutine runnin rite now. (line {})"; break;
    case SUPERCLASS_NOT_CLASS:
      fmt = "'{}' iz not a class, cant b a parent."; break;
    case METHOD_NEEDS_OVERRIDE:
      fmt = "method '{}' allready in parent class, u gotta say 'override' if u wanna replace it."; break;
    case OVERRIDE_NOT_IN_PARENT:
      fmt = "cant override '{}', parent class dusnt haz dat 2 override."; break;
    case OPERAND_NOT_NUMBER:
      fmt = "'{}' needs a numbr, dis no numbr. (line {})"; break;
    case OPERANDS_NOT_NUMBERS:
      fmt = "'{}' needs TWO numbrz, one or both no numbr. (line {})"; break;
    case UNEXPECTED_AMP:
      fmt = "wut iz dis lonely '&' at line {}, u meen '&&'?"; break;
    case UNEXPECTED_PIPE:
      fmt = "wut iz dis lonely '|' at line {}, u meen '||'?"; break;
    case UNEXPECTED_CHAR:
      fmt = "wut iz dis '{}' at line {}, nevr seen dat b4."; break;
    case UNTERMINATED_STRING:
      fmt = "dis string startin at line {} nevr closed, where it go."; break;
    case INVALID_HEX_LITERAL:
      fmt = "0x needs hex digitz after it at line {}, dis gots none."; break;
    case INVALID_BYTE_SIZE:
      fmt = "bytes gotta b 2 characters, u gaved {} at line {}, too smol or too big."; break;
    case INVALID_NUMBER_TRAILING_CHAR:
      fmt = "'{}' stuck onto teh end of a numbr at line {}, dat no belong there."; break;
    case CONSUME_VAR_NAME_AFTER_GLOBAL:
      fmt = "needs a name after 'global', wut we callin dis."; break;
    case CONSUME_EQUAL_AFTER_GLOBAL:
      fmt = "needs '=' after teh global name."; break;
    case CONSUME_SEMI_AFTER_GLOBAL:
      fmt = "needs ';' after teh global thingy."; break;
    case CONSUME_VAR_NAME_AFTER_LET:
      fmt = "needs a name after 'let'."; break;
    case CONSUME_SEMI_AFTER_LET:
      fmt = "needs ';' after teh let thingy."; break;
    case CONSUME_VAR_NAME_AFTER_CONST:
      fmt = "needs a name after 'const'."; break;
    case CONSUME_EQUAL_AFTER_CONST:
      fmt = "needs '=' after teh const name."; break;
    case CONSUME_SEMI_AFTER_CONST:
      fmt = "needs ';' after teh const thingy."; break;
    case CONSUME_VAR_NAME_AFTER_MUTEX:
      fmt = "needs a name after 'mutex'."; break;
    case CONSUME_EQUAL_AFTER_MUTEX:
      fmt = "needs '=' after teh mutex name."; break;
    case CONSUME_SEMI_AFTER_MUTEX:
      fmt = "needs ';' after teh mutex thingy."; break;
    case CONSUME_FN_NAME:
      fmt = "funs needs a name, dis one iz nameless."; break;
    case CONSUME_LPAREN_AFTER_FN_NAME:
      fmt = "needs '(' after teh fun's name."; break;
    case CONSUME_PARAM_NAME:
      fmt = "needs a param name here."; break;
    case CONSUME_RPAREN_AFTER_PARAMS:
      fmt = "needs ')' after teh paramz."; break;
    case CONSUME_LBRACE_BEFORE_FN_BODY:
      fmt = "needs '{' b4 teh fun's insides."; break;
    case CONSUME_SEMI_AFTER_BREAK:
      fmt = "needs ';' after 'break'."; break;
    case CONSUME_SEMI_AFTER_CONTINUE:
      fmt = "needs ';' after 'continue'."; break;
    case CONSUME_LPAREN_AFTER_IF:
      fmt = "needs '(' after 'if'."; break;
    case CONSUME_RPAREN_AFTER_IF_COND:
      fmt = "needs ')' after teh if condishun."; break;
    case CONSUME_LPAREN_AFTER_WHILE:
      fmt = "needs '(' after 'while'."; break;
    case CONSUME_RPAREN_AFTER_WHILE_COND:
      fmt = "needs ')' after teh while condishun."; break;
    case CONSUME_LPAREN_AFTER_FOR:
      fmt = "needs '(' after 'for'."; break;
    case CONSUME_SEMI_AFTER_FOR_COND:
      fmt = "needs ';' after teh for-loop condishun."; break;
    case CONSUME_RPAREN_AFTER_FOR_CLAUSES:
      fmt = "needs ')' after teh for clausez."; break;
    case CONSUME_SEMI_AFTER_RETURN:
      fmt = "needs ';' after teh return valu."; break;
    case CONSUME_RBRACE_CLOSE_BLOCK:
      fmt = "needs '}' 2 close dis block, it left open like a fridge."; break;
    case CONSUME_SEMI_AFTER_EXPR:
      fmt = "needs ';' after dat expresshun."; break;
    case CONSUME_CLASS_NAME:
      fmt = "classes needs a name too."; break;
    case CONSUME_PARENT_CLASS_NAME:
      fmt = "needs parent class name after ':'."; break;
    case CONSUME_LBRACE_BEFORE_CLASS_BODY:
      fmt = "needs '{' b4 teh class's insides."; break;
    case CONSUME_FUN_AFTER_OVERRIDE:
      fmt = "needs 'fun' after 'override', wut u overridin."; break;
    case CONSUME_FUN_IN_CLASS_BODY:
      fmt = "needs 'fun' insid teh class body."; break;
    case CONSUME_RBRACE_AFTER_CLASS_BODY:
      fmt = "needs '}' after teh class's insides."; break;
    case CONSUME_VAR_NAME_AFTER_FREE:
      fmt = "needs a name after 'free', wut we lettin go."; break;
    case CONSUME_SEMI_AFTER_FREE:
      fmt = "needs ';' after teh free thingy."; break;
    case CONSUME_RPAREN_AFTER_ARGS:
      fmt = "needs ')' after teh argz."; break;
    case CONSUME_RBRACKET_AFTER_INDEX:
      fmt = "needs ']' after teh index."; break;
    case CONSUME_PROP_NAME_AFTER_DOT:
      fmt = "needs a property name after '.'."; break;
    case CONSUME_RBRACKET_AFTER_ARRAY:
      fmt = "needs ']' after teh array stuffz."; break;
    case CONSUME_RPAREN_AFTER_EXPR:
      fmt = "needs ')' after teh expresshun."; break;
    case CONSUME_CLASS_NAME_AFTER_NEW:
      fmt = "needs class name after 'new', new wut??"; break;
    case CONSUME_LPAREN_AFTER_CLASS_NAME:
      fmt = "needs '(' after teh class's name."; break;
    case CONSUME_FN_NAME_AFTER_COROUTINE:
      fmt = "needs fun name after 'coroutine', coroutine wut??"; break;
    case CONSUME_RPAREN_GENERIC:
      fmt = "needs ')' rite bout now."; break;
    case CONSUME_LPAREN_AFTER_SUPER:
      fmt = "needs '(' after 'super'."; break;
    case CONSUME_RPAREN_AFTER_SUPER_ARGS:
      fmt = "needs ')' after super's argz."; break;
    case CONSUME_FUN_AFTER_ASYNC:
      fmt = "needs 'fun' after 'async', async wut??"; break;

    default:
      fmt = "dis message ID iz a msmakerystery, no can haz.";
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
