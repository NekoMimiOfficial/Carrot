# How to add a core Callable  
Core callables are functions always defined by the interpreter at runtime, not through the bootstrap.  
This includes lots of essential functions such as imports and array manipulation.  
It is generally advised to add your functions to the bootstrap instead, however if necessary we will provide the steps to add a builtin core function  

> [!NOTE]
> If you need any C++ custom function to be added to the bootstrap then you can create your custom FFI library with stubs and import that to the bootstrap then provide callables to use the stubbed logic from the C++ side.  

Please try to keep your function cross platform, adding a new function is done via the `builtin.h` file strictly, any platform dependant code is defined in `builtin.h` then written in `builtin.cpp` in EACH platform's folder.  
Creating your function is identical to creating an FFI function, however registering it is done inside the `interpreter.cpp` file under the `Interpreter` constructor.  
