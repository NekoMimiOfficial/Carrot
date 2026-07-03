# How to add a platform Callable  
Core callables are functions always defined by the interpreter at runtime, not through the bootstrap **and exist only for a specific platform**.  
This means if you're porting Carrot to a platform that you want to extend some special capabilities (like GPIO pins on an ESP32) you can do so without affecting the main builtins or interpreter code  
To make this possible we've made sure that our API was as user-friendly as possible so we've created 3 methods for you to use!  
`registerBuiltin`, `registerBuiltinFn` and `registerBuiltinClass`  
The former allows you to register any Value (including functions and classes) but you have to provide a name  
The latter two allow you to simply pass in a shared_ptr of a Callable or a Class and it'll handle the name itself  
Otherwise making the functions/classes/values is exactly the same, you create a new struct for Nin* types or simply pass in a string/double/monostate for those types respectively  

Steps to create your own platform dependant Values:  
1. ensure you've setup your platform folders and added them to the `CMakeLists.txt` file  
2. under `platform/[YOUR_PLATFORM/]` create a header file for your Value definitions and a cpp file for your registery, you **MUST** name the header file `platform_core_builtins.h` and the cpp file `platform_registery.cpp`, you may add other header files to keep your project structure clean  
3. inside your header include `value.h` and start defining your functions/classes/values/etc...  
4. inside your cpp file include the header file you made (`platform_core_builtins.h`) and `platform.h`  
5. inside your cpp file define `void registerPlatformBuiltins(Interpreter *interp)` and within it you can call `interp->registerBuiltinFn(std::make_shared<YOUR_FN>());` to register your function, the same can be done for `registerBuiltinClass` and `registerBuiltin`, you can technically also do `interp->registerBuiltin("x", "hello, world!");` which will add a variable called `x` and set its contents to "hello, world!"  
