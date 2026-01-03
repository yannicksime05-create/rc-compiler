#include "include/lexer.h"
#include "include/parser.h"
#include <fstream>

int main(int argc, char *argv[]) {
    if(argc < 2) {
        std::cerr << "Usage: " << argv[0] << "<filename>" << std::endl;
        return 1;
    }

    std::ifstream f(argv[1]);
    if(!f.is_open()) {
        std::cerr << "Error: Could not open file " << argv[1] << std::endl;
        return 1;
    }

    Lexer lexer(f);
    std::vector<Token>& tokens = lexer.lex();
    f.close();
    std::cout << "AST:" << std::endl;
    Parser parser(tokens);
    parser.parse();

    return 0;
}
