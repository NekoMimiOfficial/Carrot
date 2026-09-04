# Compiling the components  
You might look at this project and wonder how do i exactly compile it?  
First, each component is in it's own folder inside the `components` folder, if you wish to compile a component (like Nekko for example) then change into its directory (ex: `components/nekko`) where you can see the `CMakeLists.txt` file  
The cmake config is very standard, before building make sure you have the dependencies needed to compile the specific component, open the `CMakeLists.txt` file and check what libraries it needs to get linked to (ex: SDL, CURL, etc...) and install the correct devel packages for them  
You can now commence compiling, Our preferred method is running these subset of commands: `mkdir build && cd build && cmake .. && make -j12`  
This will: make a `build` directory and switch to it, then create the necessary make files from the `CMakeLists.txt` file in the outer directory and finally make the project (the -j12 splits the compilation into 12 jobs, instead of 12 write in you CPU thread count, if you're unsure how many threads you have using -j4 is always a safe option and also keeping it as -j12 will work but it might make the compilation significantly slower)  

> [!WARNING]  
> If you're compiling a component that links against Nekko (like Carrot), remember that C++ doesn't have a stable ABI. Whenever you rebuild Nekko, you need to recopy the fresh `libnekko.a`/`libnekko.so` into that component's build folder and rebuild it too, Forgetting to do this will make you debug why it's segfaulting til you realize it at 3 am qwq.  
