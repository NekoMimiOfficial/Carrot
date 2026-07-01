# How to generally add new keywords/grammar  
Here we will discuss the optimal steps to add a new keyword to ninjin  

1. add the token to `token.h` in `TokenType`.  
2. register it in the `lexer.cpp` in `keywords`.  
3. register any new AST statements in `ast.h`.  
4. declare your new statement in `parser.h`.  
5. add the corresponding logic to `parser.cpp` and register it to the correct parsing step.  
6. here you add any changes to any additional file to make your new statement work, most likely in `interpreter.cpp` or if it's a low level core statement then you modify the necessary file.  
7. build and test!  
