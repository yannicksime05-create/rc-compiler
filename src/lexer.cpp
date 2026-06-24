#include <cctype>
#include "../include/lexer.h"

Token Lexer::comments_or_div_operator() {
    Token t;
    t.start = {column - 1, line};

    if(input.peek() == '/') {
        do{
            get();
        }while(input.peek() != '\n' && !input.eof());
        std::cout << "Single-line comment." << std::endl;
        t.type = TT::USELESS;
    }
    else if(input.peek() == '*') {
        bool ended = false;
        get();

        while( input.peek() != EOF ) {
            input.get(c);
            advance();

            if(c == '*' && input.peek() == '/') {
                get();
                std::cout << "Multi-lines comment." << std::endl;
                t.type = TT::USELESS;
                ended = true;
                break;
            }
        }

        if(!ended) error(t, "", "Error: Unterminated comment!");
    }
    else if(input.peek() == '=') {
        get();
        t.type = TT::SLASH_ASSIGN;
        t.value = "/=";
    }
    else {
        t.type = TT::SLASH;
        t.value = "/";
    }

    t.end = {column, line};
    return t;
}

Token Lexer::strings() {
    Token t;
    t.start = {column - 1, line};

    //Retirer l'initialisation de s pour enlever le ' " ' dans s.
    std::string s(1, c);
    bool backslash_found = false;
    while(input.get(c)) {
        advance();
        if(backslash_found) backslash_found = false;
        else if(c == '\\')  backslash_found = true;
        else if(c == '"' || c == '\'') {
//            advance();
            std::cout << "String found!" << std::endl;
            t.type = TT::STRING;
            break;
        }

        s += c;
    }

    if(input.eof()) {
        error(t, s, "Error: Unterminated string literal! Missing enclosing \"");
        return t;
    }

    //Commentez la ligne suivante pour enlever ' " ' dans s.
    s += c;
    t.value = s;
    t.end = {column, line};
    return t;
}

Token Lexer::keywords_and_identifiers() {
    Token t;
    t.start = {column - 1, line};
    std::string s(1, c);
    while( std::isalnum(static_cast<unsigned char>(input.peek())) || input.peek() == '_' ) {
        s += get();
    }

    t.type = get_word_type(s);
    t.type == TT::IDENTIFIER ? (std::cout << "Identifier found!" << std::endl) : (std::cout << "Keyword found!" << std::endl);
    t.value = s;
    t.end = {column, line};
    return t;
}

bool Lexer::scan_binary_numbers(Token& t, std::string& s) {
    if( input.peek() != '0' && input.peek() != '1' ) {
        error(t, s, "Error: Binary number has no digits!");
        return false;
    }

    do{
        s += get();
    }while( input.peek() == '0' || input.peek() == '1' );
    t.type = TT::INTEGER;

    std::cout << "Binary number found!" << std::endl;
    return true;
}

bool Lexer::scan_octal_numbers(Token& t, std::string& s) {
    if( input.peek() < '0' || input.peek() > '7' ) {
        error(t, s, "Error: Octal number has no digits!");
        return false;
    }

    do{
        s += get();
    }while( input.peek() >= '0' && input.peek() <= '7' );
    t.type = TT::INTEGER;

    std::cout << "Octal number found!" << std::endl;
    return true;
}

bool Lexer::scan_hex_numbers(Token& t, std::string& s) {
    char ch = static_cast<unsigned char>( input.peek() );
    if( !std::isxdigit(ch) ) {
        error(t, s, "Error: Hexdecimal number has no digits!");
        return false;
    }

    do{
        s += get();
    }while( std::isxdigit(static_cast<unsigned char>(input.peek())) );
    t.type = TT::INTEGER;

    std::cout << "Hexdecimal number found!" << std::endl;
    return true;
}

//
//  Grammar handled:
//    integer  →  digits  [int-suffix]
//    float    →  digits '.' digits? [exponent] [float-suffix]
//             |  '.'    digits      [exponent] [float-suffix]
//             |  digits             [exponent] [float-suffix]
//    exponent →  ('e'|'E') ['+'|'-'] digits
//    int-suffix   →  ('u'|'U') ('l'|'L') ('l'|'L')?
//                 |  ('l'|'L') ('l'|'L')? ('u'|'U')?
//    float-suffix →  'f' | 'F' | 'l' | 'L'

// Possible states while scanning the decimal part of a number.
enum class FloatState {
    INTEGER_PART,    // reading digits before any '.' or 'e'
    AFTER_DOT,       // just consumed '.', expecting digit or exponent
    FRACTION_PART,   // reading digits after '.'
    AFTER_E,         // just consumed 'e'/'E', expecting sign or digit
    AFTER_SIGN,      // just consumed '+'/'-', expecting digit
    EXPONENT_PART,   // reading exponent digits
};

//For debugging purposes
//const char *state_to_string(FloatState s) {
//    switch(s) {
//        case FloatState::INTEGER_PART:          return "integer_part";
//        case FloatState::AFTER_DOT:             return "after_dot";
//        case FloatState::FRACTION_PART:         return "fraction_part";
//        case FloatState::AFTER_E:               return "after_e";
//        case FloatState::AFTER_SIGN:            return "after_sign";
//        case FloatState::EXPONENT_PART:         return "exponent_part";
//    }
//
//    return "";
//}

void Lexer::scan_integral_part(Token& t, std::string& s) {
    while( std::isdigit(input.peek()) ) {
        c = get();
        s += c;
    }

    t.type = TT::INTEGER;
}

void Lexer::scan_floating_point_part(Token& t, std::string& s) {
    bool has_dot = c == '.' ? true : false, has_e = false, is_float;
    FloatState state = has_dot ? FloatState::AFTER_DOT : FloatState::INTEGER_PART;

    is_float = has_dot || has_e;
    char next = input.peek();
    while( next == '.' || std::isdigit(next) || next == '+' || next == '-' || next == 'e' || next == 'E' ) {
        c = static_cast<char>(get());
//        std::cout << "s = " << s << ", c = " << c << ", state = " << state_to_string(state) << std::endl;
        is_float = has_dot || has_e;
        s += c;

        if(state == FloatState::INTEGER_PART && c == '.') {
            has_dot = true;
            state = FloatState::AFTER_DOT;
        }
//        else if(state == FloatState::AFTER_DOT && std::isdigit(c)) {
//            state = FloatState::FRACTION_PART;
//        }
        else if((state == FloatState::INTEGER_PART || state == FloatState::AFTER_DOT) && std::tolower(c) == 'e') {
            has_e = true;
            state = FloatState::AFTER_E;
        }
        else if(state == FloatState::AFTER_E && (c == '+' || c == '-')) {
            state = FloatState::AFTER_SIGN;
        }
        else if((state == FloatState::AFTER_E || state == FloatState::AFTER_SIGN) && std::isdigit(c)) {
            state = FloatState::EXPONENT_PART;
        }


        next = static_cast<char>(input.peek());
        if(is_float && c == '.') {
            error(t, s, "Error: Too many decimal points in number");
            return;
        }
        else if(    (state == FloatState::AFTER_SIGN && !std::isdigit(next))         ||
                    (state == FloatState::AFTER_E && !std::isdigit(next) && next != '+' && next != '-')
                )
        {
            error(t, s, "Error: exponent has no digits");
            return;
        }
        else if(    (next == 'e' || next == 'E') &&
                    (state == FloatState::AFTER_E || state == FloatState::AFTER_SIGN || state == FloatState::EXPONENT_PART)
                )
        {
            s += next;
            error(t, s, "Unexpected exponent marker");
            return;
        }
    }

    t.type = TT::FLOAT;
}

bool Lexer::scan_integer_suffix(Token& t, std::string& s) {
    bool suffix_u = false, suffix_l = false, suffix_ll = false, suffix_ul = false, suffix_ull = false, first = true;

    char next = input.peek(), first_suffix;
    while( next == 'u' || next == 'U' || next == 'l' || next == 'L' ) {
        c = static_cast<char>(get());

        if(first) {
            first = false;
            first_suffix = std::tolower(c);
        }


        if( first_suffix == 'u' ) {
            if(!suffix_u)                                               suffix_u = true;
            else if(suffix_u && std::tolower(c) == 'l' && !suffix_ul)   suffix_ul = true;
            else if(suffix_ul && std::tolower(c) == 'l' && !suffix_ull) suffix_ull = true;
            else {
                s += c;
                error(t, s, "Unknown suffix");
                return false;
            }
        }


        else if( first_suffix == 'l' ) {
            if(!suffix_l)                                               suffix_l = true;
            else if(suffix_l && !suffix_ll)                             suffix_ll = true;
            else if(suffix_ll && std::tolower(c) == 'u' && !suffix_ull) suffix_ull = true;
            else {
                s += c;
                error(t, s, "Unknown suffix");
                return false;
            }

        }

        s += c;
        next = input.peek();
    }

    return true;

}

bool Lexer::scan_float_suffix(Token& t, std::string& s) {
    char next = input.peek();
    if( std::tolower(next) != 'l' && std::tolower(next) != 'f' ) return true;

    s += get();

    next = input.peek();
    if( std::tolower(next) == 'l' || std::tolower(next) == 'f' || std::tolower(next) == 'e' ) {
        s += get();
        error(t, s, std::tolower(next) == 'e' ? "Error: Suffix must come last in floating point literals" : "Error: Float literal can only have one suffix");
        return false;
    }

    return true;
}

Token Lexer::numbers() {
    Token t;
    t.start = {column - 1, line};
    std::string s(1, c);
    bool base10_number = true;

    if(c == '0') {
        char next = static_cast<char>(input.peek());
        if( std::isdigit(next) ) {
            s += get();
            error(t, s, "C-style octal numbers are discouraged!");
            return t;
        }
        else if(next == 'b' || next == 'B') {
            base10_number = false;
            s += get();
            if( !scan_binary_numbers(t, s) )    return t;
        }
        else if(next == 'o' || next == 'O') {
            base10_number = false;
            s += get();
            if( !scan_octal_numbers(t, s) )     return t;
        }
        else if(next == 'x' || next == 'X') {
            base10_number = false;
            s += get();
            if( !scan_hex_numbers(t, s) )       return t;
        }
    }

    if(base10_number) {
        scan_integral_part(t, s);

        //Note: Putting this here prevents non-base10 floating-point numbers. I read that some languages allowed them, but we'll stick with this for now.
        //c == '.' handles leading-dot floats (.5, .14e3) entered via next_token's case '.'.
        //input.peek() handles mid-number floats (3.14) and exponents (1e9).
        if(c == '.' || input.peek() == '.' || std::tolower(input.peek()) == 'e') scan_floating_point_part(t, s);
    }

    if(t.type == TT::INTEGER && !scan_integer_suffix(t, s))     return t;
    else if(t.type == TT::FLOAT && !scan_float_suffix(t, s))    return t;

    t.value = s;
    t.end = {column, line};
    return t;
}

Token Lexer::next_token() {
    Token t;

    //On ignore tous les espaces
    while( std::isspace(input.peek()) ) {
        input.get(c);
        advance();
    }

    t.start = {column, line};
    c = get();

    switch(c) {
        case EOF:
            --column;
            t.type = TT::END_OF_FILE;
            t.value = "End of file";
            break;
        case '{':
            t.type = TT::LBRACE;
            t.value = "{";
            break;
        case '}':
            t.type = TT::RBRACE;
            t.value = "}";
            break;
        case '(':
            t.type = TT::LPAREN;
            t.value = "(";
            break;
        case ')':
            t.type = TT::RPAREN;
            t.value = ")";
            break;
        case '[':
            t.type = TT::LBRACKET;
            t.value = "[";
            break;
        case ']':
            t.type = TT::RBRACKET;
            t.value = "]";
            break;
        case ',':
            t.type = TT::COMMA;
            t.value = ",";
            break;
        case '.':
            if( std::isdigit(input.peek()) ) {
                t = numbers();
                return t;
            }
            else {
                t.type = TT::DOT;
                t.value = ".";
            }
            break;
        case '?':
            t.type = TT::CONDITIONAL;
            t.value = "?";
            break;
        case '+':
            if(input.peek() == '+') {
                get();
                t.type = TT::INCREMENT;
                t.value = "++";
            }
            else if(input.peek() == '=') {
                get();
                t.type = TT::PLUS_ASSIGN;
                t.value = "+=";
            }
            else {
                t.type = TT::PLUS;
                t.value = "+";
            }
            break;
        case '-':
            if(input.peek() == '-') {
                get();
                t.type = TT::DECREMENT;
                t.value = "--";
            }
            else if(input.peek() == '=') {
                get();
                t.type = TT::MINUS_ASSIGN;
                t.value = "-=";
            }
            else {
                t.type = TT::MINUS;
                t.value = "-";
            }
            break;
        case '*':
            if(input.peek() == '=') {
                get();
                t.type = TT::STAR_ASSIGN;
                t.value = "*=";
            }
            else {
                t.type = TT::STAR;
                t.value = "*";
            }
            break;
        case '/':
            t = comments_or_div_operator();
            break;
        case '%':
            if(input.peek() == '=') {
                get();
                t.type = TT::MOD_ASSIGN;
                t.value = "%=";
            }
            else {
                t.type = TT::MOD;
                t.value = "%";
            }
            break;
        case '=':
            if(input.peek() == '=') {
                get();
                std::cout << "Comparaison found!" << std::endl;
                t.type = TT::EQUAL;
                t.value = "==";
            }
            else {
                std::cout << "Assignment found!" << std::endl;
                t.type = TT::ASSIGN;
                t.value = "=";
            }
            break;
        case '<':
            if(input.peek() == '=') {
                std::cout << "Comparaison found!" << std::endl;
                get();
                t.type = TT::LESS_EQUAL;
                t.value = "<=";
            }
            else if(input.peek() == '<') {
                get();
                if(input.peek() == '=') {
                    get();
                    t.type = TT::LEFT_SHIFT_ASSIGN;
                    t.value = "<<=";
                }
                else {
                    t.type = TT::LEFT_SHIFT;
                    t.value = "<<";
                }
            }
            else {
                std::cout << "Comparaison found!" << std::endl;
                t.type = TT::LESS;
                t.value = "<";
            }
            break;
        case '>':
            if(input.peek() == '=') {
                std::cout << "Comparaison found!" << std::endl;
                get();
                t.type = TT::GREATER_EQUAL;
                t.value = ">=";
            }
            else if(input.peek() == '>') {
                get();
                if(input.peek() == '=') {
                    get();
                    t.type = TT::RIGHT_SHIFT_ASSIGN;
                    t.value = ">>=";
                }
                else {
                    t.type = TT::RIGHT_SHIFT;
                    t.value = ">>";
                }
            }
            else {
                std::cout << "Comparaison found!" << std::endl;
                t.type = TT::GREATER;
                t.value = ">";
            }
            break;
        case ':':
            t.type = TT::COLON;
            t.value = ":";
            break;
        case ';':
            t.type = TT::SEMICOLON;
            t.value = ";";
            break;
        case '&':
            if(input.peek() == '&') {
                get();
                std::cout << "Logic and found!" << std::endl;
                t.type = TT::AND;
                t.value = "&&";
            }
            else if(input.peek() == '=') {
                get();
                std::cout << "Bitwise and assignment found!" << std::endl;
                t.type = TT::BIT_AND_ASSIGN;
                t.value = "&=";
            }
            else {
                std::cout << "Bitwise and found!\n" << std::endl;
                t.type = TT::BIT_AND;
                t.value = "&";
            }
            break;
        case '|':
            if(input.peek() == '|') {
                get();
                std::cout << "Logic or found!" << std::endl;
                t.type = TT::OR;
                t.value = "||";
            }
            else if(input.peek() == '=') {
                get();
                std::cout << "Bitwise or assignment found!" << std::endl;
                t.type = TT::BIT_OR_ASSIGN;
                t.value = "|=";
            }
            else {
                std::cout << "Bitwise or found!" << std::endl;
                t.type = TT::BIT_OR;
                t.value = "|";
            }
            break;
        case '!':
            if(input.peek() == '=') {
                get();
                t.type = TT::NOT_EQUAL;
                t.value = "!=";
            }
            else {
                t.type = TT::NOT;
                t.value = "!";
            }
            break;
        case '~':
            t.type = TT::BIT_NOT;
            t.value = "~";
            break;
        case '^':
            if(input.peek() == '=') {
                get();
                t.type = TT::BIT_XOR_ASSIGN;
                t.value = "^=";
            }
            else {
                t.type = TT::BIT_XOR;
                t.value = "^";
            }
            break;
        default:
            //=============== STRING LITERALS ===============
            if(c == '"' || c == '\'') {
                t = strings();
                return t;
            }
            //=============== KEYWORDS & IDENTIFIERS ===============
            else if( std::isalpha(static_cast<unsigned char>(c)) || c == '_' ) {
                t = keywords_and_identifiers();
                return t;
            }
            //=============== NUMBERS ===============
            else if( std::isdigit(c) ) {
                t = numbers();
                return t;
            }
            else {
                std::string s(1, c);
                error(t, s, "Unknown token found!");
                return t;
            }
        break;
    }

    t.end = {column, line};
    return t;
}
