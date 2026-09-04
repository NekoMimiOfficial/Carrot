# Nekko and how to use it for your own needs  
Nekko is our core embeddable interpreter, it allows you to use the ninjin language in any project you wish.  
To add it follow these simple steps:  
1. compile `components/nekko` for your preferred platform/architecture  
2. take the generated `libnekko.so` (or `libnekko.a` if you built with `-DBUILD_SHARED=OFF`) and the header files inside `components/nekko/head`, you may install them as a global library or use it in your project manually  
3. in your project you can use the header files to include the lexer and interpreter then use that to create your own runners or access the environment etc (to read or modify variable/call states)  
4. modify your project build file to include the library and the header files if needed  
5. build and verify!  

> [!NOTE]  
> You can get an idea of how the nekko core works by looking into the Carrot interpreter  
> Carrot uses Nekko and consists of 1 main file which makes it an easy file to read and give insight  

> [!WARNING]  
> C++ has no stable ABI, so whatever headers your project compiles against MUST exactly match the `libnekko` you're linking. If you (or me :3) change anything in `components/nekko/head`, you need to rebuild both Nekko and your project together, copy the fresh library over, and do a clean rebuild on your side. Mixing an old library with new headers (or vice versa :3) doesn't fail to link, it links fine and then corrupts memory at runtime in ways that are very annoying to track down *trust me...*.  

> [!NOTE]  
> Each `Interpreter` instance keeps its own scope chain per-thread internally (`env` is `thread_local`), so it's safe to drive one `Interpreter` from multiple threads at once (for example letting `async`/`coroutine` scripts run their bodies on background threads) without you needing to add any locking of your own `Environment` already serializes its own state behind a single internal lock.  
