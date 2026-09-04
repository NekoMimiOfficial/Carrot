# How to generally add new keywords/grammar  
Here we will discuss the optimal steps to add a new keyword to ninjin  

1. add the token to `token.h` in `TokenType`.  
2. register it in the `lexer.cpp` in `keywords` (skip this if you're adding a symbol/operator instead of a word-keyword, that goes in `scanToken` instead).  
3. add your new node struct: expressions go in `exprNodes.h`, statements go in `stmtNodes.h`. Have it inherit from `ExprAcceptor<YourNode>` (expressions) or `StmtAcceptor<YourNode>` (statements) instead of `Expr`/`Stmt` directly.  
4. add a matching entry to the right visitor interface in `visitor.h`, `virtual Value visit(YourNode &e) = 0;` for expressions, `virtual void visit(YourNode &s) = 0;` for statements and declare your new struct near the top of that file alongside the others.  
5. declare your new statement/expression in `parser.h`.  
6. add the new logic to `parser.cpp` and register it to the correct parsing step.  
7. declare the matching override in `interpreter.h`, `Value visit(YourNode &e) override;` or `void visit(YourNode &s) override;`.  
8. implement the logic thus statements go in `exec.cpp`, expressions go in `eval.cpp`.  
9. build and test!  

> [!NOTE]  
> The one exception to step 3 is `AsyncFunctionStmt`, which inherits from `FunctionStmt` directly (to reuse its fields) rather than from `StmtAcceptor`. If your new node needs to reuse an existing node's fields the same way, you'll need to write its `accept()` override by hand too, just do `void accept(StmtVisitor &v) override { v.visit(*this); }`, one line, same idea as what the template generates automatically for everyone else.  

> [!NOTE]  
> `exprNodes.h` and `stmtNodes.h` are only ever included through `ast.h` (which sets up `ExprAcceptor`/`StmtAcceptor` right before including them), never include either of those two files directly.
