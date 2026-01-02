#include "include/lexer.h"
#include "include/parser.h"
#include <fstream>

int main() {
    std::ifstream f("../tests_files/operations.txt");
    Lexer lexer(f);
    std::vector<Token>& tokens = lexer.lex();
    f.close();
    std::cout << "AST:" << std::endl;
    Parser parser(tokens);
    parser.parse();

    return 0;
}
