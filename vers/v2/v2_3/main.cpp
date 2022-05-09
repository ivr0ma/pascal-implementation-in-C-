#include <iostream>
#include <string>

#include "Lex.h"
#include "Scanner.h"
#include "Interpretator.h"

using namespace std;

//Таблица служебных слов
const char *
        Scanner::TW    [] = { "", "and", "begin", "bool", "do", "else", "end", "if", "false", "int", "not", "or", "program",
                              "read", "then", "true", "var", "while", "write", NULL };

//Таблица разделителей
const char *
        Scanner::TD    [] = {".", ";", ",", ":", ":=", "(", ")", "=", "<", ">", "+", "-", "*", "/", "<=", "!=", ">=", NULL };

/*==========================================================================
 *----------------------------------Main------------------------------------
 *==========================================================================
 * Запуск интерпретации, вывод сообщений об ошибке
 *==========================================================================
 */
int main () {
    try {
        Interpretator I ( "C:\\Users\\ivr0m\\CLionProjects\\pascal\\prog.txt"  );
        I.interpretation ();
        return 0;
    }

    //лексическая ошибка
    catch ( char c ) {
        cout << "unexpected symbol " << c << endl;
        return 1;
    }

    //синтаксическая ошибка
    catch ( Lex l ) {
        cout << "unexpected lexeme" << l << endl;
        return 1;
    }

    //семантическая ошибка, ошибка выполнения
    catch ( const char *source ) {
        cout << source << endl;
        return 1;
    }
}