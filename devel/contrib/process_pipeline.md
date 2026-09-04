# We present to you some words that are full of tech buzz!  
We know projects on a big scale can be hard to get an idea of, so we present to you with this document that walks you through each step in the interpreter :D  
So umm have this small example that we'll be walking it on the processing pipeline (yayy more tech buzz :3c)  
```js
let x = 5;
print(x);
```

## (Stage 0) the entrypoint  
Everything starts in `main.cpp`. It reads your `.nin` file (or a line you typed into the REPL) into one big string, then hands that string off to a `Lexer`. This file also owns the actual `Interpreter` object, it lives right there in `main()`'s own scope so it gets cleaned up properly when the program ends (not as some sneaky global that outlives everything, we learned that lesson the hard way believe me qwq).  

## (Stage 1) turning text into tokens via `token.h`, `lexer.h`, `lexer.cpp`  
`token.h` defines what a `Token` is, just a little struct holding a `TokenType` (an enum like `LET`, `IDENTIFIER`, `NUMBER`, `SEMICOLON`...), the actual text it came from, and what line it was on.  

`lexer.h`/`lexer.cpp` is the actual `Lexer` class. It reads your source one character at a time and groups characters into tokens. For our example, it spits out something like:  
`LET`, `IDENTIFIER("x")`, `EQUAL`, `NUMBER(5)`, `SEMICOLON`, `PRINT`, `LPAREN`, `IDENTIFIER("x")`, `RPAREN`, `SEMICOLON`, `EOF_TOKEN`  
Nothing means anything yet — we've just sorted the text into labeled boxes.  
*please note that in this stage print is a builtin statement, we will be changing this to be a platform function*  

## (Stage 2) turning tokens into a tree via `parser.h`, `parser.cpp`  
The `Parser` takes that flat list of tokens and figures out the actual structure. It knows the grammar rules (a `let` is followed by a name, then `=`, then an expression, then `;`), and as it recognizes each piece, it builds little tree nodes out of them.  

For `let x = 5;` it builds a `VarDecl` node (holding the name `x` and an expression for `5`).  
For `print(x);` it builds a `PrintStmt` node (holding an expression that reads variable `x`). *again this will be changed later*  

Both of these node types are defined elsewhere, the parser just knows how to build them, not what they look like internally. We do that next :3  

## (Stage 3) what the tree nodes actually look like via `ast.h`, `exprNodes.h`, `stmtNodes.h`  
`ast.h` sets the whole thing up, it defines the two base shapes, `Expr` (anything that produces a value, like `5` or `x` or `5 + 2`) and `Stmt` (anything that's a full instruction, like `let x = 5;` or `print(x);`). It also defines two little template helpers, `ExprAcceptor` and `StmtAcceptor`, whose whole job is to automatically give every node a way to say "hey interpreter, I'm one of these, here's what to do with me".  

The actual node definitions live in two separate files to keep things tidy, `exprNodes.h` has all the expression nodes (`LiteralExpr`, `VariableExpr`, `BinaryExpr`, `CallExpr`, and so on), `stmtNodes.h` has all the statement nodes (`VarDecl`, `PrintStmt`, `IfStmt`, `WhileStmt`, and so on). Every single one of these just inherits from `ExprAcceptor<ThemselvesSpecifically>` or `StmtAcceptor<ThemselvesSpecifically>` and gets its own node definition automatically.  

## (Stage 4) the interpreter's list of what it knows how to do via `visitor.h`  
`visitor.h` defines two interfaces: `StmtVisitor` and `ExprVisitor`. Each one is basically a big list listing every single kind of node that exists, with one matching "key slot?" per kind (`visit(VarDecl&)`, `visit(PrintStmt&)`, `visit(LiteralExpr&)`, etc). Whoever wants to actually run a Nin program has to fill in every single slot on this list. Guess who does that... ME!!!! NEKOMIMI!!!! *wait no...*  

## (Stage 5) the Interpreter itself via `interpreter.h`  
The `Interpreter` class signs up for both lists (`StmtVisitor` and `ExprVisitor`) and promises to fill in every slot. It also has two tiny helper functions, `execute()` and `evaluate()`, which don't really do any work themselves, they just ask a tree node "oi mate! what kinda node are ye? :3c" (that's the `accept()` trick from stage 3 kicking in), and the node calls back the matching menu slot on the interpreter automatically. So the interpreter never has to guess what kind of node it's looking at, the node itself always knows and routes the call correctly.  

## (Stage 6) the interpreter waking up via `interpreter.cpp`  
*I'm waking up to ash and dust i wipe my- oh wait... not this qwq*  
This file has the actual `Interpreter` constructor and its `reset()` function, which is where every builtin function (`print`, `import`, `len`, `push`, all of them) gets registered into a starting `Environment` (see stage 8). It also has some shared setup bits like `makeBoundMethod` (used for class methods, so `this` works correctly), and it pulls in `ninfunction.h` for the one struct that represents a user-defined Nin function as something callable from C++. *yes, ik, it makes my head swirl too qwq*  

## (Stage 7) actually running statements and expressions via `exec.cpp` and `eval.cpp`  
Here's where all those list slots from `visitor.h` finally get filled in with real logic.  

`exec.cpp` has every statement's recipe. For our example, `Interpreter::visit(VarDecl&)` lives here, it evaluates the initializer expression (`5`) and stores it under the name `x`. `Interpreter::visit(PrintStmt&)` also lives here, it evaluates whatever expression it was given and prints the result. *not for long tho*  

`eval.cpp` has every expression's recipe. `Interpreter::visit(LiteralExpr&)` just hands back the literal value it's holding (in our case, `5`). `Interpreter::visit(VariableExpr&)` looks up a name in the current scope and hands back whatever's stored there.  

So for our little script, the trip looks like: `execute(VarDecl)` -> calls `evaluate(LiteralExpr 5)` -> gets back `5` -> stores it as `x`. Then `execute(PrintStmt)` -> calls `evaluate(VariableExpr x)` -> looks `x` up -> gets back `5` -> prints it.  

## (Stage 8) where "x" actually lives via `environment.h`  
Every scope (the top level of your script, a function body, an `if` block, whatever) gets its own `Environment`, basically a little labeled box of name into value pairs. Each box remembers the box it was created inside of, so looking something up means checking your own box first, then asking your parent box, then its parent, and so on, until you find it or run out of boxes to ask. `const`, `global`, and `mutex` are just small flags stuck onto a name inside one of these boxes, and every single operation on any box waits its turn behind one shared lock, so even if multiple coroutines are poking at things at once, nothing ever gets corrupted. *oh gosh i really hope so qwq*  

## (Stage 9) what a "Value" even is via `value.h`  
This file defines `Value` itself, a Nin variable can be a number, a string, a boolean, nil, or one of several "boxed" object types: `NinCallable` (anything callable, builtins, user functions, bound methods), `NinArray`, `NinModule`, `NinClass`, `NinInstance`, `NinCoroutine`, and `NinNative` (for values handed over by C++ modules). Every single one of these is just a small struct.  

## (Stage 10) user defined functions specifically via `ninfunction.h`  
When your script writes `fun greet() { ... }`, we need something that C++ can actually call when the script calls `greet()`. That's what `NinFunction` is, it remembers which `FunctionStmt` node it came from, and which `Environment` box existed at the moment it was defined. Calling it means create a new scope then put the arguments into it, and run the function's body through `executeBlock` just like any other block of statements.  

## (Stage 11) async/coroutine taking a detour via `coroutine.h`, `coroutine.cpp`  
When a script does `coroutine someAsyncFn()` and calls `.run()`, the function's body actually gets handed off to run on a real background thread (`coroutineRun`), completely separate from the main script's thread. Since `Environment` already has that one shared lock protecting everything, this is safe even though two different chunks of code might genuinely be running at the exact same moment. `.yield()` just checks in on whether that background thread has finished yet, without ever blocking the main script while it waits.  

## (Stage 12) builtins and platform stuff via `builtin.h`, `builtin.cpp`, `platform_core_builtins.h`, `platform_registry.cpp`  
Things like `print`, `len`, `push`, `import` are defined as little `NinCallable` structs in `builtin.h`, with any platform specific bits (like `loadmodule`, which needs `dlopen`) split out into each platform's own folder so the core interpreter never has to care which OS it's running on. All of these get registered into the very first `Environment` box back in stage 6, which is why they're available absolutely everywhere in a script, from the very first line.  

## That's the whole round trip, Here's a TL;DR:  
1. `main.cpp` reads your source text  
2. `token.h`/`lexer.cpp` turns it into tokens  
3. `parser.cpp` builds a tree, using node shapes from `exprNodes.h`/`stmtNodes.h` (set up by `ast.h`)  
4. `interpreter.h` walks that tree (with slots defined in `visitor.h`), calling into `exec.cpp` for statements and `eval.cpp` for expressions  
5. variables live in `environment.h` boxes, values themselves are defined in `value.h`  
6. user functions get wrapped as `NinFunction` (`ninfunction.h`), async ones can hop onto a background thread via `coroutine.cpp`  
7. builtins like `print` (for now) (from `builtin.h`/platform files) do the actual "external execution" part  
8. `5` shows up on your screen, and I have a big grin on my face :3c  

Yep that's it! nothing much y'see? it's very simple and straight forward and definitely very easy to traverse the tens of files for the specific one you might need :D  
