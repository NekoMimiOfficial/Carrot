# How to add a new platform  
Have you ever had the urge to port Usagi/Carrot to your platform/architecture? No?  
Well... here are the instructions anyways :3  
First it is essential to point out that the main core of the interpreter is the embedded core (Usagi) which would be the thing you want to port first, This allows you to create any form of application that might intergrate the Ninjin language into it, The process of porting Usagi to a new architecture or platform includes the following steps:  
1. to make things easier you will want to copy `components/usagi/src/linux` and `components/usagi/platform/linux` to a new `src` and `platform` folder that defines your architecture/platform (ex: `components/usagi/src/wii` and `components/usagi/platform/wii`)  
2. all what is left for you to do code-wise is to port the platform specific code (the files inside the linux folders you just copied) to make them work against your compiler of choice  
3. edit the `components/usagi/CMakeLists.txt` file to add your own architecture/platform build instructions, usually you dont need to do anything special since Usagi is only a library  
4. build Usagi making sure you've passed the correct cmake flags to enable your platform build and disable the linux build `-DBUILD_LINUX=off`  
5. you can now move onto making your Carrot port which is easier, (note: you MUST copy `libusagi.a` to the build folder of Carrot to build it)  
6. add your platform/architecture specific files and changes to the CMakeLists as you did with Usagi (note that this is necessary because not all platforms provide a CLI)  
7. build and test, enjoy!  
