# RC Compiler

I love programming languages and I wanted to know how they work under the hood so I decided to create my own one (or at least sketch one).
I know it's gonna be tough, or even really tough, but I still want to give it a try. And by the end, I think this will also improve my coding skills so there's nothing I should be afraid of!

In the project's hierarchy, you'll see:
- **ebnfs** : ebnf for the language
- **features**: an alphabetically sorted list of all the features I want in this compiler;
- **include**: header files for the lexer, parser...;
- **src**: implementation files.

# How to compile and run the project:

> ### On Linux
> 1. g++ include/\*.h src/*.cpp main.cpp -o rc  
> 2. ./rc tests/program.txt
>
> ### On Windows
> 1. g++ include/\*.h src/*.cpp main.cpp -o rc.exe  
> 2. rc.exe tests/program.txt
> 
> If you want to execute any file other than program.txt, please comment line 20-22 in main.cpp and then compile and run.  
> If you don't, the program will enter an infinite loop because the parser will try to do something he can't.


If 
- you're interested by the features and want to suggest new ones,
- want to improve my code,
- or want to make any other kind of suggestions,
feel free to pull a request, I'll be glad to hear you!



Sime K. Yannick.
