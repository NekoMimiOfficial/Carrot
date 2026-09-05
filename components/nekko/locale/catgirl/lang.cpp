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
    ss << "[Unsupported Type]";
  }
}

std::string LOC_Internal(int id, std::initializer_list<std::any> args) {
  std::string_view fmt = "";

  switch (id) {
    case UDEF_VAR:
      fmt = "hehe... i-i couldn't find '{}' anywhewe nya... did it wun away? >w<"; break;
    case ASSIGN_TO_CONST_VAR:
      fmt = "nyo nyo nyo~ '{}' is a const, you can't touch that siwwy~ *bonks paw*"; break;
    case ASSIGN_TO_UDEF_VAR:
      fmt = "umu... '{}' doesn't exist yet nya, you gotta 'let' it in fiwst pwease~"; break;
    case FREE_UDEF_VAR:
      fmt = "nya?? you can't fwee '{}', i nevew even had it in my widdle paws~"; break;
    case PREDEFINED_VAR:
      fmt = "'{}' awweady exists nya!! *puffs up wike an angwy kitten* can't make it twice~"; break;
    case RET_EXCEPTION:
      fmt = "'return' outside a function?? nyaa that's confusing, where do i even wetuwn to~ >_<"; break;
    case BREAK_EXCEPTION:
      fmt = "'break' but dewe's no woop to bweak nya... *sad kitten noises*"; break;
    case CONTINUE_EXCEPTION:
      fmt = "'continue' but dewe's no woop to continue eithew nya, i'm lost~"; break;
    case FN_TOO_MANY_ARGS:
      fmt = "'{}' onwy wants {} thingies but you gave it {} nya!! too many pwesents~ *overwhelmed*"; break;
    case FN_MISSING_ARG:
      fmt = "'{}' is hungwy nya, it needs '{}' pwease feed it~ *purrs expectantly*"; break;
    case FN_UNEXPECTED_KWARG:
      fmt = "'{}' doesn't know what '{}' means nya, *tiwts head* what's dat~"; break;
    case FN_NOT_VARIADIC:
      fmt = "'{}' doesn't do those namey-awgy things nya, just gib normaw ones pwease~"; break;
    case CORO_RUNNING:
      fmt = "dis coworoutine is awready wunning ow finished nya, can't do it again~ *dizzy*"; break;
    case OP_PLUS_MISMATCH:
      fmt = "'+' wants numbews ow wowds nya, not dis... *confused mew* (line {})"; break;
    case DIV_ZERO:
      fmt = "eek!! can't divide by zewo nya, math just bwoke~ (line {})"; break;
    case EXPECT_FN:
      fmt = "onwy functions can be cawwed nya~ (line {})"; break;
    case FN_NOT_ENOUGH_ARGS:
      fmt = "'{}' wants {} thingies but onwy got {} nya, missing some~ (line {})"; break;
    case ARRAY_INDEX_NAN:
      fmt = "awway index hasta be a numbew nya~ *confused taiw wag*"; break;
    case ARRAY_INDEX_OOB:
      fmt = "index {} feww wight off teh edge of teh awway nya!! *catches it* gotcha~"; break;
    case STRING_INDEX_NAN:
      fmt = "stwing index needs to be a numbew too nya~"; break;
    case STRING_INDEX_OOB:
      fmt = "index {} is outside teh wowd nya, oopsie daisy~"; break;
    case TYPE_UNINDEXABLE:
      fmt = "you can't [] into dis kind of thing nya~ (line {})"; break;
    case PARSE_TOO_MANY_PARAMS:
      fmt = "255 pawametews is teh max nya, dat's a wot of fwiends~ cawm down~"; break;
    case PARSE_TOO_MANY_ARGS:
      fmt = "255 awguments is teh max nya, dat's pwenty of cheezies~"; break;
    case PARSE_DEFAULT_PARAM_ORDER:
      fmt = "'{}' has no defauwt but comes aftew one dat duz nya, puwease put it fiwst~"; break;
    case PARSE_BREAK_OUTSIDE_LOOP:
      fmt = "'break' but no woop hewe nya~ (line {})"; break;
    case PARSE_CONTINUE_OUTSIDE_LOOP:
      fmt = "'continue' but no woop hewe eithew nya~ (line {})"; break;
    case PARSE_RETURN_OUTSIDE_FUNC:
      fmt = "'return' but no function to wetuwn fwom nya~ (line {})"; break;
    case PARSE_AWAIT_OUTSIDE_ASYNC:
      fmt = "'await' but dis function isn't async nya, can't wait hewe~ (line {})"; break;
    case PARSE_EXPECT_EXPRESSION:
      fmt = "i-i was expecting something at wine {} nya, but got dis weiwd token '{}' instead >w< *confused purr*"; break;
    case PARSE_INVALID_ASSIGN_TARGET:
      fmt = "you can't put stuff into dat at wine {} nya, it doesn't have a swot~"; break;
    case ASSIGN_CONST_ARRAY:
      fmt = "awway '{}' is const nya, no touchy touchy~ *paws away your hand*"; break;
    case INDEX_ASSIGN_NOT_ARRAY:
      fmt = "you can onwy put stuff into awways wif [] nya, dis isn't one~"; break;
    case MODIFY_CONST_ARRAY:
      fmt = "dis awway is const nya, hands off pwease~"; break;
    case MODULE_NO_MEMBER:
      fmt = "dis moduwe doesn't have '{}' inside nya, i checked evewywhewe wif my widdle paws~"; break;
    case UNDEF_PROPERTY:
      fmt = "no pwopewty cawwed '{}' hewe nya~ *sniffs around confused*"; break;
    case NOT_GETTABLE:
      fmt = "onwy moduwes n instances have pwopewties to get nya~ (line {})"; break;
    case NOT_SETTABLE:
      fmt = "onwy moduwes n instances wet you set pwopewties nya~ (line {})"; break;
    case NEW_NOT_CLASS:
      fmt = "you used 'new' on something dat isn't a cwass nya!! (line {})"; break;
    case INIT_ARG_MISMATCH:
      fmt = "'{}.init' wants {} thingies but got {} nya, oopsie~"; break;
    case NO_INIT_WITH_ARGS:
      fmt = "cwass '{}' has no init nya, why you gib it awguments~ *confused taiw fwick*"; break;
    case SUPER_OUTSIDE_METHOD:
      fmt = "'super' outside a cwass method?? nya dat doesn't make sense~ *dizzy*"; break;
    case NO_PARENT_CLASS:
      fmt = "cwass '{}' has no pawent nya, it's a widdle owphan~ *hugs it*"; break;
    case PARENT_NO_INIT:
      fmt = "pawent cwass has no init to caww nya, nuffin happened~"; break;
    case SUPER_ARG_MISMATCH:
      fmt = "super() wants {} thingies but got {} nya~"; break;
    case CORO_NOT_FN:
      fmt = "coworoutine: '{}' isn't even a function nya~ *confused blink*"; break;
    case CORO_NOT_ASYNC:
      fmt = "coworoutine: '{}' is a function but not an async one nya, can't coworoutine dat~"; break;
    case AWAIT_OUTSIDE_CORO:
      fmt = "'await' but no coworoutine is wunning wight now nya~ (line {})"; break;
    case SUPERCLASS_NOT_CLASS:
      fmt = "'{}' isn't a cwass nya, can't be a pawent~"; break;
    case METHOD_NEEDS_OVERRIDE:
      fmt = "method '{}' is awready in teh pawent cwass nya, say 'override' if you wanna weplace it pwease~"; break;
    case OVERRIDE_NOT_IN_PARENT:
      fmt = "can't ovewwide '{}' nya, pawent cwass doesn't have dat~"; break;
    case OPERAND_NOT_NUMBER:
      fmt = "'{}' needs a numbew nya, dis isn't one~ (line {})"; break;
    case OPERANDS_NOT_NUMBERS:
      fmt = "'{}' needs TWO numbews nya, one ow both awen't~ (line {})"; break;
    case UNEXPECTED_AMP:
      fmt = "nya?? what's dis wonewy '&' at wine {}, did you mean '&&'? *tiwts head*"; break;
    case UNEXPECTED_PIPE:
      fmt = "nya?? what's dis wonewy '|' at wine {}, did you mean '||'? *tiwts head*"; break;
    case UNEXPECTED_CHAR:
      fmt = "what's dis '{}' at wine {} nya, nevew seen dat befowe~ *sniffs it*"; break;
    case UNTERMINATED_STRING:
      fmt = "dis stwing stawting at wine {} nevew cwosed nya, where did it go~ *searches frantically*"; break;
    case INVALID_HEX_LITERAL:
      fmt = "0x needs hex digits aftew it at wine {} nya, dis has none~"; break;
    case INVALID_BYTE_SIZE:
      fmt = "bytes gotta be 2 chawactews nya, you gave {} at wine {}, too smoww ow too big~"; break;
    case INVALID_NUMBER_TRAILING_CHAR:
      fmt = "'{}' is stuck onto teh end of a numbew at wine {} nya, dat doesn't bewong dewe~"; break;
    case CONSUME_VAR_NAME_AFTER_GLOBAL:
      fmt = "needs a name aftew 'global' nya, what awe we cawwing dis~"; break;
    case CONSUME_EQUAL_AFTER_GLOBAL:
      fmt = "needs '=' aftew teh gwobaw name nya~"; break;
    case CONSUME_SEMI_AFTER_GLOBAL:
      fmt = "needs ';' aftew teh gwobaw thingy nya~"; break;
    case CONSUME_VAR_NAME_AFTER_LET:
      fmt = "needs a name aftew 'let' nya~"; break;
    case CONSUME_SEMI_AFTER_LET:
      fmt = "needs ';' aftew teh let thingy nya~"; break;
    case CONSUME_VAR_NAME_AFTER_CONST:
      fmt = "needs a name aftew 'const' nya~"; break;
    case CONSUME_EQUAL_AFTER_CONST:
      fmt = "needs '=' aftew teh const name nya~"; break;
    case CONSUME_SEMI_AFTER_CONST:
      fmt = "needs ';' aftew teh const thingy nya~"; break;
    case CONSUME_VAR_NAME_AFTER_MUTEX:
      fmt = "needs a name aftew 'mutex' nya~"; break;
    case CONSUME_EQUAL_AFTER_MUTEX:
      fmt = "needs '=' aftew teh mutex name nya~"; break;
    case CONSUME_SEMI_AFTER_MUTEX:
      fmt = "needs ';' aftew teh mutex thingy nya~"; break;
    case CONSUME_FN_NAME:
      fmt = "functions need a name too nya~ *pouts*"; break;
    case CONSUME_LPAREN_AFTER_FN_NAME:
      fmt = "needs '(' aftew teh function's name nya~"; break;
    case CONSUME_PARAM_NAME:
      fmt = "needs a pawametew name hewe nya~"; break;
    case CONSUME_RPAREN_AFTER_PARAMS:
      fmt = "needs ')' aftew teh pawametews nya~"; break;
    case CONSUME_LBRACE_BEFORE_FN_BODY:
      fmt = "needs '{' befowe teh function's insides nya~"; break;
    case CONSUME_SEMI_AFTER_BREAK:
      fmt = "needs ';' aftew 'break' nya~"; break;
    case CONSUME_SEMI_AFTER_CONTINUE:
      fmt = "needs ';' aftew 'continue' nya~"; break;
    case CONSUME_LPAREN_AFTER_IF:
      fmt = "needs '(' aftew 'if' nya~"; break;
    case CONSUME_RPAREN_AFTER_IF_COND:
      fmt = "needs ')' aftew teh if condition nya~"; break;
    case CONSUME_LPAREN_AFTER_WHILE:
      fmt = "needs '(' aftew 'while' nya~"; break;
    case CONSUME_RPAREN_AFTER_WHILE_COND:
      fmt = "needs ')' aftew teh while condition nya~"; break;
    case CONSUME_LPAREN_AFTER_FOR:
      fmt = "needs '(' aftew 'for' nya~"; break;
    case CONSUME_SEMI_AFTER_FOR_COND:
      fmt = "needs ';' aftew teh fow-woop condition nya~"; break;
    case CONSUME_RPAREN_AFTER_FOR_CLAUSES:
      fmt = "needs ')' aftew teh fow cwauses nya~"; break;
    case CONSUME_SEMI_AFTER_RETURN:
      fmt = "needs ';' aftew teh wetuwn vawue nya~"; break;
    case CONSUME_RBRACE_CLOSE_BLOCK:
      fmt = "needs '}' to cwose dis bwock nya, it's weft open wike a fowgotten window~"; break;
    case CONSUME_SEMI_AFTER_EXPR:
      fmt = "needs ';' aftew dat expwession nya~"; break;
    case CONSUME_CLASS_NAME:
      fmt = "cwasses need a name too nya~"; break;
    case CONSUME_PARENT_CLASS_NAME:
      fmt = "needs pawent cwass name aftew ':' nya~"; break;
    case CONSUME_LBRACE_BEFORE_CLASS_BODY:
      fmt = "needs '{' befowe teh cwass's insides nya~"; break;
    case CONSUME_FUN_AFTER_OVERRIDE:
      fmt = "needs 'fun' aftew 'override' nya, what awe you ovewwiding~"; break;
    case CONSUME_FUN_IN_CLASS_BODY:
      fmt = "needs 'fun' inside teh cwass body nya~"; break;
    case CONSUME_RBRACE_AFTER_CLASS_BODY:
      fmt = "needs '}' aftew teh cwass's insides nya~"; break;
    case CONSUME_VAR_NAME_AFTER_FREE:
      fmt = "needs a name aftew 'free' nya, what awe we wetting go~ *sniffwes*"; break;
    case CONSUME_SEMI_AFTER_FREE:
      fmt = "needs ';' aftew teh fwee thingy nya~"; break;
    case CONSUME_RPAREN_AFTER_ARGS:
      fmt = "needs ')' aftew teh awguments nya~"; break;
    case CONSUME_RBRACKET_AFTER_INDEX:
      fmt = "needs ']' aftew teh index nya~"; break;
    case CONSUME_PROP_NAME_AFTER_DOT:
      fmt = "needs a pwopewty name aftew '.' nya~"; break;
    case CONSUME_RBRACKET_AFTER_ARRAY:
      fmt = "needs ']' aftew teh awway stuffies nya~"; break;
    case CONSUME_RPAREN_AFTER_EXPR:
      fmt = "needs ')' aftew teh expwession nya~"; break;
    case CONSUME_CLASS_NAME_AFTER_NEW:
      fmt = "needs cwass name aftew 'new' nya, new what~ *confused*"; break;
    case CONSUME_LPAREN_AFTER_CLASS_NAME:
      fmt = "needs '(' aftew teh cwass's name nya~"; break;
    case CONSUME_FN_NAME_AFTER_COROUTINE:
      fmt = "needs function name aftew 'coroutine' nya, coworoutine what~"; break;
    case CONSUME_RPAREN_GENERIC:
      fmt = "needs ')' wight about now nya~"; break;
    case CONSUME_LPAREN_AFTER_SUPER:
      fmt = "needs '(' aftew 'super' nya~"; break;
    case CONSUME_RPAREN_AFTER_SUPER_ARGS:
      fmt = "needs ')' aftew supew's awguments nya~"; break;
    case CONSUME_FUN_AFTER_ASYNC:
      fmt = "needs 'fun' aftew 'async' nya, async what~"; break;

    default:
      fmt = "hehe i-i don't know what dis message id is nya... *hides in a box* sowwy~";
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
