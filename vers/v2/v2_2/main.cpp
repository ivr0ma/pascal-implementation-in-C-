#include <iostream>
#include <string>

#include "Lex.h"
#include "Scanner.h"
#include "Interpretator.h"

using namespace std;

const char *
        Scanner::TW    [] = { "", "and", "begin", "bool", "do", "else", "end", "if", "false", "int", "not", "or", "program",
                              "read", "then", "true", "var", "while", "write", NULL };

const char *
        Scanner::TD    [] = {"@", ";", ",", ":", ":=", "(", ")", "=", "<", ">", "+", "-", "*", "/", "<=", "!=", ">=", NULL };

int main () {
    try {
        Interpretator I ( "C:\\Users\\ivr0m\\CLionProjects\\pascal\\prog.txt"  );
        I.interpretation ();
        return 0;
    }
    catch ( char c ) {
        cout << "unexpected symbol " << c << endl;
        return 1;
    }
    catch ( Lex l ) {
        cout << "unexpected lexeme" << l << endl;
        return 1;
    }
    catch ( const char *source ) {
        cout << source << endl;
        return 1;
    }
}