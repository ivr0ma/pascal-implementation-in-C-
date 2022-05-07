#ifndef PASCAL_IDENT_H
#define PASCAL_IDENT_H

#include <iostream>
#include <string>
#include <cstdio>
#include <ctype.h>
#include <cstdlib>
#include <vector>
#include <stack>
#include <algorithm>

#include "Lex.h"

using namespace std;

/*==========================================================================
 *------------------------------Класс Ident---------------------------------
 *==========================================================================
 * Информация о лексеме
 *==========================================================================
 */
class Ident {
    string      name;  // название переменной
    string      proc_name;  // название переменной
    bool        declare;  // было ли описано это имя
    type_of_lex type;  //
    bool        assign;  // флаг для интерполяции
    int          value;  // значение
public:
    Ident() {
        declare = false;
        assign  = false;
    }
    bool operator== ( const string& s ) const {
        return name == s;
    }
    Ident ( const string n ) {
        name    = n;
        proc_name = "";
        declare = false;
        assign  = false;
    }
    string get_name () const {
        return name;
    }
    string get_proc_name () const {
        return proc_name;
    }
    bool get_declare () const {
        return declare;
    }
    void put_declare () {
        declare   = true;
    }
    type_of_lex get_type () const {
        return type;
    }
    void put_type ( type_of_lex t ) {
        type      = t;
    }
    void put_proc_name ( string name ) {
        proc_name  = name;
    }
    bool get_assign () const {
        return assign;
    }
    void put_assign () {
        assign    = true;
    }
    int  get_value () const {
        return value;
    }
    void put_value ( int v ) {
        value     = v;
    }
};


#endif //PASCAL_IDENT_H