# How to add a new platform  
Have you ever had the urge to port Nekko/Carrot to your platform/architecture? No?  
Well... here are the instructions anyways :3  
First it is essential to point out that the main core of the interpreter is the embedded core (Nekko) which would be the thing you want to port first, This allows you to create any form of application that might intergrate the Ninjin language into it, The process of porting Nekko to a new architecture or platform includes the following steps:  
1. to make things easier you will want to copy `components/nekko/src/linux` and `components/nekko/platform/linux` to a new `src` and `platform` folder that defines your architecture/platform (ex: `components/nekko/src/wii` and `components/nekko/platform/wii`)  
2. all what is left for you to do code-wise is to port the platform specific code (the files inside the linux folders you just copied) to make them work against your compiler of choice, the usual files you'll need to port are: `fs.cpp` (home directory expansion), `builtin.cpp` (for nin module loading), `asset_store.cpp` (resolving `@`-prefixed module paths), and `coroutine.cpp` (`coroutineRun`/`coroutineJoin`, which use `std::async` on Linux but could use whatever native threading your platform offers)  
3. we support adding platform specific code, this means if your platform has a specific feature that is unique to it you can add it just for that platform (look at `adding_new_platform_functions.md`), note the the print function is considered platform dependant and you'll need to implement it here  
4. edit the `components/nekko/CMakeLists.txt` file to add your own architecture/platform build instructions, usually you dont need to do anything special since Nekko is only a library  
5. build Nekko making sure you've passed the correct cmake flags to enable your platform build and disable the linux build `-DBUILD_LINUX=off`  
6. you can now move onto making your Carrot port which is easier, (note: you MUST copy `libnekko.a`/`libnekko.so` to the build folder of Carrot to build it)  
7. add your platform/architecture specific files and changes to the CMakeLists as you did with Nekko (note that this is necessary because not all platforms provide a CLI)  
8. build and test, enjoy!  

> [!WARNING]  
> C++ has no stable ABI. If you rebuild Nekko after changing a header (even just adding a field), you MUST re-copy the fresh `libnekko.a`/`libnekko.so` into Carrot's (or your own consuming project's) build folder and do a full rebuild there too. Linking a freshly-compiled executable against a stale library is a very fast way to get baffling heap corruption that has nothing to do with your actual logic, ask us how we know qwq  
