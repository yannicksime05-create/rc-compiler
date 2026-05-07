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

        if(!ended) {
            t.end = {column, line};
            error(t, "", "Error: Unterminated comment!");
        }
    }
    else if(input.peek() == '=') {
        get();
        t.type = TT::SLASH_ASSIGN;
        t.value = "/=";
    }
    else {
        t.type = TT::SLASH;          //                      /
        t.value = "/";
    }
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
//                    std::cout << "character after the end of the loop = " << c << std::endl;
    if(input.eof()) {
        t.end = {column, line};
        error(t, s, "Error: Unterminated string literal! Missing enclosing \"");
        return t;
    }

    //Commentez la ligne suivante pour enlever ' " ' dans s.
    s += c;
    t.value = s;
    return t;
}

Token Lexer::keywords_and_identifiers() {
    Token t;
    t.start = {column - 1, line};
    std::string s(1, c);
    while( std::isalnum(static_cast<unsigned char>(input.peek())) || input.peek() == '_' ) {
//        input.get(c); ++column;
//        s += c;
        s += get();
    }

    t.type = get_word_type(s);
    t.type == TT::IDENTIFIER ? (std::cout << "Identifier found!" << std::endl) : (std::cout << "Keyword found!" << std::endl);
    t.value = s;
    return t;
}

void Lexer::scan_binary_numbers(Token& t, std::string& s) {
    if( input.peek() != '0' && input.peek() != '1' ) {
        t.end = {column, line};
        error(t, s, "Error: Binary number has no digits!");
        return;
    }

    do{
        s += get();
    }while( input.peek() == '0' || input.peek() == '1' );
    t.type = TT::INTEGER;
    t.value = s;
    t.end = {column, line};

    std::cout << "Binary number found!" << std::endl;
}

void Lexer::scan_octal_numbers(Token& t, std::string& s) {
    if( input.peek() < '0' || input.peek() > '7' ) {
        t.end = {column, line};
        error(t, s, "Error: Octal number has no digits!");
        return;
    }

    do{
        s += get();
    }while( input.peek() >= '0' && input.peek() <= '7' );
    t.value = s;
    t.end = {column, line};
    t.type = TT::INTEGER;
    std::cout << "Octal number found!" << std::endl;
}

void Lexer::scan_hex_numbers(Token& t, std::string& s) {
    char ch = static_cast<unsigned char>( input.peek() );
    if( !std::isxdigit(ch) ) {
        t.end = {column, line};
        error(t, s, "Error: Hexdecimal number has no digits!");
        return;
    }

    do{
        s += get();
    }while( std::isxdigit(static_cast<unsigned char>(input.peek())) );
    t.value = s;
    t.end = {column, line};
    t.type = TT::INTEGER;
    std::cout << "Hexdecimal number found!" << std::endl;
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
    SUFFIX,          // reading the type suffix — must be the last character
};

const char *state_to_string(FloatState s) {
    switch(s) {
        case FloatState::INTEGER_PART:          return "integer_part";
        case FloatState::AFTER_DOT:             return "after_dot";
        case FloatState::FRACTION_PART:         return "fraction_part";
        case FloatState::AFTER_E:               return "after_e";
        case FloatState::AFTER_SIGN:            return "after_sign";
        case FloatState::EXPONENT_PART:         return "exponent_part";
        case FloatState::SUFFIX:                return "suffix";
    }

    return "";
}

void Lexer::base10_or_float_numbers(Token& t, std::string& s) {
    bool has_dot = c == '.' ? true : false, has_e = false, is_float, has_l = false, has_ll = false, has_u = false;
    FloatState state = has_dot ? FloatState::AFTER_DOT : FloatState::INTEGER_PART;

    char prev, next;
    c = static_cast<char>(get());
    while(  c == '.'                                  ||
            std::isdigit(c)                           ||
            c == '+' || c == '-'                      ||
            c == 'e' || c == 'E'                      ||
            c == 'l' || c == 'L'                      ||
            ( is_float && (c == 'f' || c == 'F') )    ||
            ( !is_float && (c == 'u' || c == 'U') )
        )
    {

//        std::cout << "s = " << s << ", c = " << c << ", state = " << state_to_string(state) << std::endl;
        is_float = has_dot || has_e;
        s += c;

        if(state == FloatState::INTEGER_PART && c == '.') {
            has_dot = true;
            state = FloatState::AFTER_DOT;
        }
//        else if(state == FloatState::AFTER_DOT && !has_e) {
//            state = FloatState::FRACTION_PART;
//        }
        else if((state == FloatState::INTEGER_PART || state == FloatState::AFTER_DOT) && std::tolower(c) == 'e') {
//        (state == FloatState::INTEGER_PART || state == FloatState::AFTER_DOT) && (c == 'e' || c == 'E')
            has_e = true;
            state = FloatState::AFTER_E;
        }
        else if(state == FloatState::AFTER_E && (c == '+' || c == '-')) {
            state = FloatState::AFTER_SIGN;
        }
        else if((state == FloatState::AFTER_E || state == FloatState::AFTER_SIGN) && std::isdigit(c)) {
            state = FloatState::EXPONENT_PART;
        }
        else if(std::tolower(c) == 'u' || std::tolower(c) == 'l' || std::tolower(c) == 'f') {
            state = FloatState::SUFFIX;

            if(std::tolower(c) == 'u') has_u = true;
            else if(std::tolower(c) == 'l' && !has_l) has_l = true;
            else if(std::tolower(c) == 'l' && has_l) has_ll = true;
        }


        next = static_cast<char>(input.peek());
        if(is_float && c == '.') {
            t.end = {column, line};
            error(t, s, "Error: Too many decimal points in number");
            return;
        }
        else if(    (state == FloatState::AFTER_SIGN && !std::isdigit(next))         ||
                    (state == FloatState::AFTER_E && !std::isdigit(next) && next != '+' && next != '-')
                )
        {
            t.end = {column, line};
            error(t, s, "Error: exponent has no digits");
            return;
        }
        else if(is_float && state == FloatState::SUFFIX && (next == 'l' || next == 'L' || next == 'f' || next == 'F')) {
            t.end = {column, line};
            error(t, s, "Float literal has more than one suffix");
            return;
        }
        else if( (  state == FloatState::AFTER_E     ||
                    state == FloatState::AFTER_SIGN  ||
                    state == FloatState::SUFFIX      ||
                    state == FloatState::EXPONENT_PART
                )
                && (next == 'e' || next == 'E')
        ) {
            s += next;
            t.end = {column, line};
            error(t, s, "Unexpected exponent marker");
            return;
        }
        else if(    (has_u && std::tolower(next) == 'u') ||
                    (has_ll && std::tolower(next) == 'l')
        ) {
//            c += get(); s += c;
            s += next;
            t.end = {column, line};
            error(t, s, "Unknown suffix");
            return;
        }


        prev = c;
        c = get();
    }
    input.unget(); --column;

    t.type = is_float ? TT::FLOAT : TT::INTEGER;
    t.value = s;
}

Token Lexer::numbers() {
    Token t;
    t.start = {column - 1, line};
    std::string s(1, c);

    if(c == '0') {
        char next = static_cast<unsigned char>(input.peek());
        if( std::isdigit(next) ) {
//            c = get(); s += c;
            s += get();
            t.end = {column, line};
            error(t, s, "C-style octal numbers are discouraged!");
            return t;
        }
        else if(next == 'b' || next == 'B') {
//            c = get(); s += c;
            s += get();
            scan_binary_numbers(t, s);
            return t;
        }
        else if(next == 'o' || next == 'O') {
//            c = get(); s += c;
            s += get();
            scan_octal_numbers(t, s);
            return t;
        }
        else if(next == 'x' || next == 'X') {
//            c = get(); s += c;
            s += get();
            scan_hex_numbers(t, s);
            return t;
        }
    }

//    scan_decimal_or_float(t, s, c == '.' ? true : false);
    base10_or_float_numbers(t, s);
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
            }
            else {
                t.type = TT::DOT;
                t.value = ".";
            }
            break;
        case '?':
            t.type = TT::QUESTION;
            t.value = "?";
            break;
        case '+':
            if(input.peek() == '+') {
                get();
                t.type = TT::PLUS_PLUS;
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
                t.type = TT::MINUS_MINUS;
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
            else if( std::isalpha(static_cast<unsigned char>(c)) || c == '_') {
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
                t.end = {column, line};
                error(t, s, "Unknown token found!");
                return t;
            }
        break;
    }

    t.end = {column, line};
    return t;
}
