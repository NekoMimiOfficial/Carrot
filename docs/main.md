# Carrot  
This document aims to make you understand and use the ninjin language of the carrot UI toolkit, carrot is a trademark owned by NekoMimi <nekomimi@tilde.team> 2026.  
You may not claim ownership of said name nor project, licensed by the NekoCense license.  

Carrot is a UI toolkit with support for a special interpreted OOP language called Ninjin, Carrot also provides an easy way to create applications with logic by extending certain widgets to callable functions, this allows you to manipulate the DOM as you see fit and base the content on logic operations.  
Ninjin is a simple language to learn with sensible expression evaluation and is brackets and semi-colon dependent, it brings you the simplicity of an easy language like python but with good syntax and never has you guessing what the outcome of an expression would be!  

# Grammar  

| Keyword | Example | Description |
| --------------- | --------------- | --------------- |
| `let` | `let x = 1;` | Defines a variable. |
| `fun` | `fun helloWorld() {}` | Defines a function. |
| `if` | `if (expr) {}` | Runs logic based on a condition. |
| `while` | `while (expr) {}` | Keeps running logic so long as a condition is met. |
| `print` | `print(any)` | Writes to stdout. |
| `class` | `class Main {}` | Creates a new class. |
| `new` | `let x = new Main();` | Creates a new child object of a class. |
| `DOT` | `x.object` | Access an object from a module or class. |
| `async` | `async fun x() {}` | Defines an asynchronous function. |
| `coroutine` | `let x = coroutine y();` | Creates a coro instance of an async function. |
| `const` | `const PI = 3.14;` | Defines a variable as a constant. |
| `global` | `global i = 0;` | Defines a global variable to the current and child scopes. |
| `override` | `override fun function_inside_a_class() {}` | Overrides a function inside a class, usually inherited. |
| `COLON` | `classGen2 : classGen1 {}` | Inherits a base class. |
| `free` | `free i;` | Frees a declaration from the environment. |


# Built-ins  

| Fn Name | Arguments | Description |
| --------------- | --------------- | --------------- |
| `import()` | {string} | Imports a nin module. |
| `loadmodule()` | {string} | Imports an external C++API module shared object. |
| `input()` | {string} | Asks the user to input data, returns a string, accepts an input prompt. |
| `system()` | {string} | Executes a command on the system shell, returns the exit status code. |
| `argv()` | {} | Returns an array of the argv. |
| `exit()` | {int} | Exits the app with the provided exit code. |
| `sleep()` | {int} | Sleeps for a defined amount of milliseconds |
| `coroutine.run()` | {} | Runs a coroutine instance. |
| `coroutine.yield()` | {AnyType} | Yields the result of the coroutine if it has finished or the passed value if not. |
| `clock()` | {} | Returns the unix epoch time. |
| `str()` | {AnyType} | Converts any viable type to a string. |
| `num()` | {string|float} | Converts any viable type to an integer. |
| `type()` | {AnyType} | Returns the type of a variable. |
| `len()` | {string|array} | Returns the length of an array or string. |
| `push()` | {array, AnyType} | Pushes a value to the end of an array. |
| `pop()` | {array} | Pops the last value off an array. |

