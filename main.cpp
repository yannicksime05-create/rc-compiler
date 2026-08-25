#include "include/lexer.h"
#include "include/parser.h"
#include "include/printer.h"
#include "include/semantic_analyser.h"
#include "include/cpp_generator.h"
#include <fstream>

int main(int argc, char *argv[]) {
//    if(argc < 2) {
//        std::cerr << "Usage: " << argv[0] << "<filename>" << std::endl;
//        return 1;
//    }
//
//    std::ifstream f(argv[1]);
//    if(!f.is_open()) {
//        std::cerr << "Error: Could not open file " << argv[1] << std::endl;
//        return 1;
//    }


    std::ifstream f("transpilation_test.rc");


    Lexer lexer(f);
    std::vector<Token>& tokens = lexer.lex();
    f.close();

    std::cout << "tokens size = " << tokens.size() << "." << std::endl;

    Parser parser(tokens);
    Program p = *parser.parse();

    Printer printer;
    printer.print(p);

    SemanticAnalyser analyser(p);
    analyser.analyze();

    CppGenerator gen;
    std::ofstream output("out.cpp");
    output << gen.generate(p);
    output.close();

    system("g++ -std=c++17 out.cpp -o out");

    return 0;
}
