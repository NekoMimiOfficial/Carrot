# Usagi and how to use it for your own needs  
Usagi is our core embeddable interpreter, it allows you to use the ninjin language in any project you wish.  
To add it follow these simple steps:  
1. compile `components/usagi` for your preferred platform/architecture  
2. take the generated `libusagi.a` and the header files inside `components/usagi/head`, you may install them as a global library or use it in your project manually  
3. in your project you can use the header files to include the lexer and interpreter then use that to create your own runners or access the environment etc (to read or modify variable/call states)  
4. modify your project build file to include the library and the header files if needed  
5. build and verify  

> [!NOTE]
> You can get an idea of how the usagi core works by looking into the Carrot interpreter  
> Carrot uses Usagi and consists of 1 main file which makes it an easy file to read and give insight  
