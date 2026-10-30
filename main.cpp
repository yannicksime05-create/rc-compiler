#include <fstream>
#include <sstream>
#include "include/diagnostic.h"
#include "include/lexer.h"
#include "include/parser.h"
#include "include/printer.h"
#include "include/semantic_analyser.h"
//#include "include/cpp_generator.h"

std::string read_file(const std::string& path) {
    std::ifstream file(path, std::ios::in | std::ios::binary);
    if(!file) throw std::runtime_error("Cannot open file '" + path + "'");

    std::ostringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

int main(int argc, char *argv[]) {
//    if(argc < 2) {
//        std::cerr << "Usage: " << argv[0] << "<filename>" << std::endl;
//        return 1;
//    }

    const std::string filename = "tests/program.txt";
//    const std::string filename(argv[1]);
    std::string source = read_file(filename);
    DiagnosticEngine engine(filename, source);

    try{
        Lexer lexer(source, engine);
        std::vector<Token>& tokens = lexer.lex();

//        std::cout << "tokens size = " << tokens.size() << "." << std::endl;

        Parser parser(tokens, engine);
        Program p = *parser.parse();

//        Printer printer;
//        printer.print(p);

        SemanticAnalyser analyser(p, engine);
        analyser.analyze();

//        CppGenerator gen;
//        std::ofstream output("out.cpp");
//        output << gen.generate(p);
//        output.close();
    }
    catch(const CompilationAborted& e) {
        std::cerr << e.what();
    }
    engine.print_summary();

//    system("g++ -std=c++17 out.cpp -o out");

    return 0;
}
