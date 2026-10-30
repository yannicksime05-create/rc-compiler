#include "../include/diagnostic.h"

DiagnosticEngine::DiagnosticEngine(const std::string& fname, const std::string& src, std::ostream& o) : filename(fname), source(src), out(o) {
    lines_offsets.push_back(0);
    for(size_t i = 0; i < src.size(); ++i) if(src[i] == '\n') lines_offsets.push_back(i + 1);
}

const char *DiagnosticEngine::color(Severity s) {
    switch(s) {
        case Severity::NOTE:       return "\033[1;36m"; // cyan
        case Severity::WARNING:    return "\033[1;35m"; // magenta
        default:                   return "\033[1;31m"; // red
    }
}

const char *DiagnosticEngine::reset() {
    return "\033[0m";
}

const char *DiagnosticEngine::label(Severity s) {
    switch(s) {
        case Severity::NOTE:       return "note";
        case Severity::WARNING:    return "warning";
        default:                   return "error";
    }
}

int DiagnosticEngine::find_line_index(size_t offset) {
    if(lines_offsets.empty()) return -1;

    size_t l = 0, r = lines_offsets.size(), m = 0;
    while(l < r) {
        m = l + (r - l) / 2;

        if (lines_offsets[m] <= offset) l = m + 1; // On cherche si une ligne suivante peut aussi convenir
        else                            r = m;     // m est trop grand, on réduit la borne supérieure
    }

    return static_cast<int>(l - 1);
}

/**
*   This function can be rewritten using std::upper_bound like this:
*
*   auto it = std::upper_bound(lines_offsets.begin(), lines_offsets.end(), offset);
*   int line = static_cast<int>(it - lines_offsets.begin());
*   int col = static_cast<int>(offset - lines_offsets[line - 1] + 1);
*   return {col, line};
*
*   but std::upper_bound requires <algorithm> and including that whole file for one function isn't something I want.
*/
Location DiagnosticEngine::locate(size_t offset) {
    int line_index = find_line_index(offset);
    int col = static_cast<int>(offset - lines_offsets[line_index]);

    //lines and cols always start at 1, so we add it.
    return {col + 1, line_index + 1};

}

void DiagnosticEngine::report(Severity s, const SourceRange& r, const std::string& msg) {
    if(s == Severity::ERROR)        ++errors;
    else if(s == Severity::WARNING) ++warnings;

    Location start = locate(r.begin);

    //result: filename:line:col: error/note/warning: msg
    out << filename << ":" << start.line << ":" << start.col << ": " << color(s) << label(s) << ": " << reset() << msg << "\n";
    print_snippet(s, start.line, r);

    if(s == Severity::ERROR && errors >= MAX_ERRORS) {
        out << "too many errors emitted, stopping now\n";
        throw CompilationAborted("error limit reached");
    }
}

void DiagnosticEngine::print_snippet(Severity s, int lineno, const SourceRange& r) {
    size_t line_begin = lines_offsets[lineno - 1];
    size_t line_end = static_cast<size_t>(lineno) < lines_offsets.size() ? lines_offsets[lineno] : source.size();

    std::string text = source.substr(line_begin, line_end - line_begin);
    while(!text.empty() && (text.back() == '\n' || text.back() == '\r')) text.pop_back();

    std::string gutter(std::to_string(lineno).size(), ' ');

    std::string indent;
    for(size_t i = line_begin; i < r.begin; ++i) indent += source[i] == '\t' ? '\t' : ' ';

    //if r is a multi-line range (r.end is greater), we stop at the end of the line.
    size_t highlight_end_offset = r.end < line_end ? r.end : line_end;
    size_t width = highlight_end_offset > r.begin ? highlight_end_offset - r.begin : 1;

    out << " " << lineno << " | " << text << "\n";
    out << " " << gutter << " | " << indent << color(s) << std::string(width, '^') << reset() << "\n";
}
