#ifndef DIAGNOSTICS_H
#define DIAGNOSTICS_H

#include <iostream>
#include <stdexcept>
#include <vector>
#include <string>
#include "token.h"

enum class Severity { NOTE, WARNING, ERROR };

struct CompileError : std::runtime_error {
    SourceRange range;

    CompileError(const SourceRange& r, const std::string& msg) : std::runtime_error(msg), range(r) {}
};

/** Thrown when the compiler must stop right now (e.g. error limit reached). Caught once, in main(). */
struct CompilationAborted : std::runtime_error {
    CompilationAborted(const std::string& msg) : std::runtime_error(msg) {}
};

/**
*   The single place where diagnostics are formatted and printed. Output looks like:
*
*   test.rc:12:13: error: use of undefined identifier 'x'
*      12 |     int y = x + 1;
*         |             ^
*/
class DiagnosticEngine {
    const std::string& filename;
    const std::string& source;
    std::ostream& out;
    std::vector<size_t> lines_offsets;

    const size_t MAX_ERRORS = 20;
    size_t errors = 0, warnings = 0;

    const char *color(Severity s);
    const char *reset();
    const char *label(Severity s);
    int find_line_index(size_t offset);
    Location locate(size_t offset);
    void print_snippet(Severity s, int lineno, const SourceRange& r);
    void report(Severity s, const SourceRange& r, const std::string& msg);

public:
    DiagnosticEngine(const std::string& fname, const std::string& src, std::ostream& o = std::cerr);

    //used by the lexer
    void error(const Token& t, const std::string& msg)          { report(Severity::ERROR, t.range, msg); }
    void error(const SourceRange& r, const std::string& msg)    { report(Severity::ERROR, r, msg); }
    void error(const CompileError& e)                           { report(Severity::ERROR, e.range, e.what()); }


    void warning(const SourceRange& r, const std::string& msg)  { report(Severity::WARNING, r, msg); }
    void note(const SourceRange& r, const std::string& msg)     { report(Severity::NOTE, r, msg); }

    bool has_errors() { return errors > 0; }

    void print_summary() {
        out << "=== Build finished: " << errors << " error(s), " << warnings << " warning(s) generated ===\n";
    }
};

#endif // DIAGNOSTICS_H
