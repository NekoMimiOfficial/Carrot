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
| `DOT` | `x.object` | access an object from a module or class. |


# Built-ins  
| Fn Name | Arguments | Description |
| --------------- | --------------- | --------------- |
| `import()` | {string} | Imports a nin module. |
| `clock()` | {} | Returns the unix epoch time. |
| `str()` | {AnyType} | Converts any viable type to a string. |
| `num()` | {string|float} | Converts any viable type to an integer. |
| `type()` | {AnyType} | Returns the type of a variable. |
| `len()` | {string|array} | Returns the length of an array or string. |
| `push()` | {array, AnyType} | Pushes a value to the end of an array. |
| `pop()` | {array} | Pops the last value off an array. |
| `sqrt()` | {float} | Squares the provided number. |
| `abs()` | {float} | Returns absolute value of the provided number. |
| `floor()` | {float} | Floors the provided number. |
| `input()` | {string} | Asks the user to input data, returns a string, accepts an input prompt. |
| `system()` | {string} | Executes a command on the system shell, returns the exit status code. |
| `get()` | {string} | Runs a GET request to the specified URL, returns the data back from the URL. |
