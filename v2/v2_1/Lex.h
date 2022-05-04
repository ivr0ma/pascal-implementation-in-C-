//
// Created by ivr0m on 04.05.2022.
//

#ifndef PASCAL_LEX_H
#define PASCAL_LEX_H

#include <iostream>
#include <string>
#include <cstdio>
#include <ctype.h>
#include <cstdlib>
#include <vector>
#include <stack>
#include <algorithm>

using namespace std;

enum type_of_lex {
    LEX_NULL,                                                                                   /* 0*/
    LEX_AND, LEX_BEGIN, LEX_BOOL, LEX_DO, LEX_ELSE, LEX_END, LEX_IF, LEX_FALSE, LEX_INT,        /* 9*/
    LEX_NOT, LEX_OR, LEX_PROGRAM, LEX_READ, LEX_THEN, LEX_TRUE, LEX_VAR, LEX_WHILE, LEX_WRITE,  /*18*/
    LEX_FIN,                                                                                    /*19*/
    LEX_SEMICOLON, LEX_COMMA, LEX_COLON, LEX_ASSIGN, LEX_LPAREN, LEX_RPAREN, LEX_EQ, LEX_LSS,   /*27*/
    LEX_GTR, LEX_PLUS, LEX_MINUS, LEX_TIMES, LEX_SLASH, LEX_LEQ, LEX_NEQ, LEX_GEQ,              /*35*/
    LEX_NUM,                                                                                    /*36*/
    LEX_ID,                                                                                     /*37*/
    POLIZ_LABEL,                                                                                /*38*/
    POLIZ_ADDRESS,                                                                              /*39*/
    POLIZ_GO,                                                                                   /*40*/
    POLIZ_FGO                                                                                   /*41*/
};

/*==========================================================================
 *-------------------------------Класс Lex----------------------------------
 *==========================================================================
 * Содержательно внутреннее представление лексем – это пара
 * (тип_лексемы, значение_лексемы).
 * Значение лексемы – это номер строки в таблице лексем соответствующего
 * класса, содержащей информацию о лексеме, или непосредственное значение,
 * например, в случае целых чисел
 *==========================================================================
 */
class Lex {
    type_of_lex   t_lex;
    int           v_lex;
public:
    Lex ( type_of_lex t = LEX_NULL, int v = 0 ): t_lex (t), v_lex (v)  { }

    type_of_lex  get_type () const {
        return t_lex;
    }

    int get_value () const {
        return v_lex;
    }
    friend ostream & operator<< ( ostream &s, Lex l );
};


#endif //PASCAL_LEX_H