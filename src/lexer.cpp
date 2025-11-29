#include <cctype>
#include "../include/lexer.h"

Token Lexer::handleCommentsOrSlash() {
    Token t;
    if(input.peek() == '/') {
        do{
            input.get(); ++column;
        }while(input.peek() != '\n');
        std::cout << "Single-line comment." << std::endl;
        t.type = TT::USELESS;
    }
    else if(input.peek() == '*') {
        //on saute le premier * de '/*'
        input.get(); ++column;
        //Cette boucle peut être réécrite avec une boucle do...while en gardant exactement les
        //éléments tels quels; Le problème est que si on utilise une boucle do...while
        //et qu'on teste le cas /* (et après la fin du fichier), la boucle do...while
        //récupèrera le EOF avant d'échouer sur la condition. Du coup pour éviter que EOF ne soit lu
        //inutilement, il vaut mieux utiliser une boucle while.
        while(input.peek() != EOF) {
            input.get(c);
            advance();
//                    std::cout << "line = " << line << ", column = " << column << std::endl;

            //On vérifie que le caractère qu'on vient de lire est '*'
            if(c == '*') {
                //Si tel est le cas, on vérifie si le caractère suivant est '/',
                //marquant la fin du commentaire multilignes. Si tel est le cas,
                //on le récupère et on sort de la bouche.
                if(input.peek() == '/') {
                    input.get(); ++column;
//                            std::cout << "Multi-lines comment." << column << std::endl;
                    std::cout << "Multi-lines comment." << std::endl;
                    t.type = TT::USELESS;
                    break;
                }
            }
        }
        if(input.peek() == EOF) {
            std::cerr << "Error: unterminated comment! at line: " << line << ", column: " << column << std::endl;
        }
    }
    else if(input.peek() == '=') {
        input.get(); ++column;
        t.type = TT::SLASH_EQUAL;
        t.value = "/=";
    }
    else {
        t.type = TT::SLASH;          //                      /
        t.value = "/";
    }
    return t;
}

Token Lexer::handleStringLiterals() {
    Token t;
    //Retirer l'initialisation de s pour enlever le ' " ' dans s.
    std::string s(1, c);
    bool backslash_found = false;
    while(input.get(c)) {
        advance();
//                  std::cout << "line = " << line << ", column = " << column << std::endl;
        if(backslash_found) backslash_found = false;
        else if(c == '\\')  backslash_found = true;
        else if(c == '"' || c == '\'') {
            advance();
            std::cout << "String found!" << std::endl;
            t.type = TT::STRING;
            break;
        }

        s += c;
    }
//                    std::cout << "character after the end of the loop = " << c << std::endl;
    if(input.eof()) {
        t.type = TT::UNKNOWN;
        std::cerr << "Error: Unterminated string literal! Missing \" at line: " << line << ", column: " << column << std::endl;
    }

    //Commentez la ligne suivante pour enlever ' " ' dans s.
    s += c;
    t.value = s;
    return t;
}

Token Lexer::handleKeywordsAndIdentifiers() {
    Token t;
    std::string s(1, c);
    while( std::isalnum(static_cast<unsigned char>(input.peek())) || input.peek() == '_' ) {
        input.get(c); ++column;
        s += c;
    }

    t.type = getWordType(s);
    t.type == TT::IDENTIFIER ? (std::cout << "Identifier found!" << std::endl) : (std::cout << "Keyword found!" << std::endl);
    t.value = s;
    return t;
}

Token Lexer::handleNumberLiterals() {
    Token t;
    //Peu importe de quel caractère il s'agit, c'est un caractère valide donc on le met une fois dans s.
    std::string s(1, c);
    bool is_float = false;

    //Si le caractère qui nous a amené dans cette fonction est 0,
    //on regarde s'il s'agit d'un nombre en base 2, 8 ou 16
    if(c == '0') {
        char next = static_cast<unsigned char>(input.peek());
    //                        std::cout << "next = " << next << std::endl;
        if( std::isdigit(next) ) {
            t.type = TT::UNKNOWN;
            input.get(c); ++column;
            s += c;
            t.value = s;
            t.loc = {column, line};
            std::cout << "C-style octal numbers are discouraged!" << " Line = " << line << ", column = " << column << std::endl;
//                            std::cout << "line = " << line << ", column = " << column << std::endl;
            return t;
        }
        else if(next == 'b' || next == 'B') {
            do{
                input.get(c); ++column;
                s += c;
            }while( input.peek() == '0' || input.peek() == '1' );
            t.value = s;
            t.loc = {column, line};
            if(s == "0b" || s == "0B") {
                t.type = TT::UNKNOWN;
                std::cerr << "Error: Binary number has no digits! Line: " << line << ", column: " << column << std::endl;
                return t;
            }
            t.type = TT::INTEGER;
            std::cout << "Binary number found!" << std::endl;
//                            std::cout << "line = " << line << ", column = " << column << std::endl;
            return t;
        }
        else if(next == 'o' || next == 'O') {
            do{
                input.get(c); ++column;
                s += c;
            }while( input.peek() >= '0' && input.peek() <= '7' );
            t.value = s;
            t.loc = {column, line};
            if(s == "0o" || s == "0O") {
                t.type = TT::UNKNOWN;
                std::cerr << "Error: Octal number has no digits! Line: " << line << ", column: " << column << std::endl;
                return t;
            }
            t.type = TT::INTEGER;
            std::cout << "Octal number found!" << std::endl;
//                            std::cout << "line = " << line << ", column = " << column << std::endl;
            return t;
        }
        else if(next == 'x' || next == 'X') {
            do{
                input.get(c); ++column;
                s += c;
            }while( std::isxdigit(static_cast<unsigned char>(input.peek())) );
            t.value = s;
            t.loc = {column, line};
            if(s == "0x" || s == "0X") {
                t.type = TT::UNKNOWN;
                std::cerr << "Error: Hexdecimal number has no digits! Line: " << line << ", column: " << column << std::endl;
                return t;
            }
            t.type = TT::INTEGER;
            std::cout << "Hexdecimal number found!" << std::endl;
//                            std::cout << "line = " << line << ", column = " << column << std::endl;
            return t;
        }
    }

    //Si c'est le '.' qui nous a amené ici, alors le nombre qu'on lit est déjà un nombre réel
    if(c == '.') is_float = true;

    //Si le caractère qui nous amène dans cette fonction n'est ni '0' ni '.',
    //et étant donné qu'on l'a déjà traité (lors de la création de s), on récupère le caractère suivant
    //pour ne pas traiter le meme caractère plusieurs fois de suite.
    c = static_cast<unsigned char>(input.get()); ++column;

    /**
    *   Quand on vient de lire 'e', '.', ou un quelconque suffixe, on a besoin
    *   d'attendre la prochaine itération avant de savoir s'il y a une erreur.
    *   Par exemple:
    *       - On vient de lire 'e' et skip_this_iteration = true.
    *           A la prochaine itération, skip_this_iteration = false étant donné que j'ai déjà lu 'e'.
    *           Si le caractère actuel (c) est encore égal à 'e', on sait qu'il y a erreur !
    *
    *   Le meme scénario se produit si on lit + ou - et qu'à l'itération d'après on lit encore + ou -
    *   Pareil pour '.' à l'étape i et '.' à l'étape i+1
    *
    *
    *   Cette variable permet donc de gérer ces situtations.
    */
    bool skip_this_iteration = false;

    bool sci_not_found = false, sci_not_sign_found = false, suffix_found = false, digit_after_e = false, dot_found = false; //0.5e10
    bool u_found = false, l_found = false, ll_found = false;
    while(  std::isdigit(c) ||
            c == '.' ||
            c == 'e' || c == 'E' ||
            ( sci_not_found && (c == '+' || c == '-') ) ||
            c == 'f' || c == 'F' || c == 'l' || c == 'L' ||
            c == 'u' || c == 'U' )
    {
        if(!is_float && ( c == '.' || c == 'e' || c == 'E' )) is_float = true;

        if(is_float) {
//          digits . digits [exponent] [suffix]
//          .digits [exponent] [suffix]
//          digits [.] [exponent] [suffix]
            //0-9, e, E, +, -, f, F, l, L

            //Si à l'itération précédente on a trouvé un 'e', '.' ou un quelconque suffixe,
            //on doit etre vigilent car c (le caractère actuel) pourrait poser problème du coup,
            //on dit au compilateur de ne pas sauter cette itération.
            /**
            *   Que se passerait-il si cette condition était placée après celles où skip_this_iteration passe à true ?
            *   - le compilateur entrerait dans l'une des ces conditions,
            *   - mettrait skip_this_iteration à true,
            *   - entrerait ensuite dans celle-ci (car l'une des variables vaudra true),
            *   - puis remettra skip_this_iteration à false
            *   Tout ça dans la même itération !
            *
            *   Conclusion: Cette condition doit impérativement venir avant toutes celles où skip_this_iteration passe à true !
            */
            if( dot_found ||
                sci_not_found || sci_not_sign_found ||
                suffix_found
            )   skip_this_iteration = false;

            //Dans chacun de ces cas, le compilateur doit attendre la prochaine étape avant de savoir s'il y a un problème.
            if(!dot_found && c == '.') {
                dot_found = true;
                skip_this_iteration = true;
            }
            else if(!sci_not_found && (c == 'e' || c == 'E')) {
                sci_not_found = true;
                skip_this_iteration = true;
            }
            else if(sci_not_found && !sci_not_sign_found && (c == '+' || c == '-')) {
                sci_not_sign_found = true;
                skip_this_iteration = true;
            }
            else if(!suffix_found && (c == 'f' || c == 'F' || c == 'l' || c == 'L')) {
                suffix_found = true;
                skip_this_iteration = true;
            }
            else if((sci_not_found || sci_not_sign_found) && !digit_after_e && std::isdigit(c)) digit_after_e = true;


            //Si '.' a déjà été trouvé à l'itération précédente (donc à celle-ci on a skip_this_iteration = false et dot_found = true)
            if(!skip_this_iteration && c == '.') {
                //Ancienne condition: dot_found && c == '.'
                //En cas de problème avec la nouvelle, réessayer celle-ci
                s += c;
                t.type = TT::UNKNOWN;
                t.value = s;
                std::cerr << "Error: Too many decimal points in number at line: " << line << ", column: " << column << std::endl;
                return t;
            }
            else if(suffix_found && (c == 'e' || c == 'E')) {
                s += c;
                t.type = TT::UNKNOWN;
                t.value = s;
                std::cerr << "Error: Suffix misplaced! Must come last at line: " << line << ", column: " << column << std::endl;
                return t;
            }
            else if(!skip_this_iteration && (c == 'e' || c == 'E' || c == '+' || c == '-'))
            {
                //Ancienne condition: !skip_this_iteration && ( ((sci_not_found) && (c == 'e' || c == 'E')) || ((sci_not_sign_found) && (c == '+' || c == '-')) )
                //En cas de problème avec la nouvelle, réessayer celle-ci
                s += c;
                t.type = TT::UNKNOWN;
                t.value = s;
                std::cerr << "Error: Exponent has no digits at line: " << line << ", column: " << column << std::endl;
                return t;
            }
            //Si un suffixe a déjà été trouvé à l'itération précédente (à celle-ci on a skip_this_iteration = false et suffix_found = true)
            //et qu'à celle-ci on en trouve encore un autre, c'est qu'il y a une erreur.
            else if(!skip_this_iteration && (c == 'f' || c == 'F' || c == 'l' || c == 'L'))
            {
                s += c;
                t.type = TT::UNKNOWN;
                t.value = s;
                std::cerr << "Error: Floats can have only one suffix at line: " << line << ", column: " << column << std::endl;
                return t;
            }
        }
        else {
            if(u_found || ll_found) skip_this_iteration = false;

            if(!u_found && (c == 'u' ||  c == 'U')) {
                u_found = true;
                skip_this_iteration = true;
            }
            else if(!l_found && (c == 'l' || c == 'L')) {
                l_found = true;
                skip_this_iteration = true;
            }
            else if(l_found && (c == 'l' || c == 'L')) {
                ll_found = true;
                skip_this_iteration = true;
            }


            if( (!skip_this_iteration && (c == 'u' || c == 'U')) ||
                (!skip_this_iteration && (c == 'l' || c == 'L'))
            ) {
                //Ancienne condition : u_found && (c == 'u' || c == 'U')
                s += c;
                t.value = s;
                t.type = TT::UNKNOWN;
                t.loc = {column, line};
                std::cerr << "Error: Duplicated suffix at line: " << line << ", column: " << column << std::endl;
                return t;
            }
        }

        //Si le caractère à réussi à éviter tous les pièges, c'est qu'il est valide et il faut l'ajouter dans s.
        s += c;
        c = static_cast<unsigned char>(input.get()); ++column;
    }
    input.putback(c); --column;
    if((sci_not_found || sci_not_sign_found) && !digit_after_e) {
//        s += c;
        t.type = TT::UNKNOWN;
        t.value = s;
        std::cerr << "Error: Exponent has no digits at line: " << line << ", column: " << column << std::endl;
        return t;
    }


    t.type = is_float ? TT::FLOAT : TT::INTEGER;
    t.value = s;

    return t;
}

const Token Lexer::getNextToken() {
    Token t;

    //On ignore tous les espaces
    while( std::isspace(input.peek()) ) {
        input.get(c);
        advance();
    }

//    std::cout << "line = " << line << ", column = " << column << std::endl;
    c = input.get();
    //On sait que le caractère qu'on vient de lire n'est pas '\n' (ou bien un quelconque espace),
    //donc on peut incrémenter column manuellement.
    ++column;

    switch(c) {
        case EOF:
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
        case '+':
            if(input.peek() == '+') {
                input.get(); ++column;
                t.type = TT::PLUS_PLUS;
                t.value = "++";
            }
            else if(input.peek() == '=') {
                input.get(); ++column;
                t.type = TT::PLUS_EQUAL;
                t.value = "+=";
            }
            else {
                t.type = TT::PLUS;
                t.value = "+";
            }
            break;
        case '-':
            if(input.peek() == '-') {
                input.get(); ++column;
                t.type = TT::MINUS_MINUS;
                t.value = "--";
            }
            else if(input.peek() == '=') {
                input.get(); ++column;
                t.type = TT::MINUS_EQUAL;
                t.value = "-=";
            }
            else {
                t.type = TT::MINUS;
                t.value = "-";
            }
            break;
        case '*':
            if(input.peek() == '=') {
                input.get(); ++column;
                t.type = TT::STAR_EQUAL;
                t.value = "*=";
            }
            else {
                t.type = TT::STAR;
                t.value = "*";
            }
            break;
        case '/':
            t = handleCommentsOrSlash();
            break;
        case '%':
            if(input.peek() == '=') {
                input.get(); ++column;
                t.type = TT::MOD_EQUAL;
                t.value = "%";
            }
            else {
                t.type = TT::MOD;
                t.value = "%";
            }
            break;
        case '=':
            if(input.peek() == '=') {
                input.get(); ++column;
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
                input.get(); ++column;
                t.type = TT::LESS_EQUAL;
                t.value = "<=";
            }
            else if(input.peek() == '<') {
                input.get(); ++column;
                if(input.peek() == '=') {
                    input.get(); ++column;
                    t.type = TT::SHIFT_LEFT_EQUAL;
                    t.value = "<<=";
                }
                else {
                    t.type = TT::SHIFT_LEFT;
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
                input.get(); ++column;
                t.type = TT::GREATER_EQUAL;
                t.value = ">=";
            }
            else if(input.peek() == '>') {
                input.get(); ++column;
                if(input.peek() == '=') {
                    input.get(); ++column;
                    t.type = TT::SHIFT_RIGHT_EQUAL;
                    t.value = ">>=";
                }
                else {
                    t.type = TT::SHIFT_RIGHT;
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
                input.get(); ++column;
                std::cout << "Logic and found!" << std::endl;
                t.type = TT::AND;
                t.value = "&&";
            }
            else {
                std::cout << "Bitwise and found!\n" << std::endl;
                t.type = TT::BIT_AND;
                t.value = "&";
            }
            break;
        case '|':
            if(input.peek() == '|') {
                input.get(); ++column;
                std::cout << "Logic or found!" << std::endl;
                t.type = TT::OR;
                t.value = "||";
            }
            else {
                std::cout << "Bitwise or found!" << std::endl;
                t.type = TT::BIT_OR;
                t.value = "|";
            }
            break;
        case '!':
            t.type = TT::NOT;
            t.value = "!";
            break;
        case '^':
            t.type = TT::CARET;
            t.value = "^";
            break;
        default:
            //=============== STRING LITERALS ===============
            if(c == '"' || c == '\'') {
                t = handleStringLiterals();
            }
            //=============== KEYWORDS & IDENTIFIERS ===============
            else if( std::isalpha(static_cast<unsigned char>(c)) || c == '_') {
                t = handleKeywordsAndIdentifiers();
            }
            //=============== NUMBERS ===============
            else if ( std::isdigit(c) || (c == '.' && std::isdigit(input.peek())) ) {
                t = handleNumberLiterals();
            }
            else {
                std::string s(1, c);
                std::cerr << "Unknown token found! At line: " << line << ", column: " << column << std::endl;
                t.type = TT::UNKNOWN;
                t.value = s;
            }
        break;
    }
    t.loc = {column, line};

    return t;
}
