#ifndef PASCAL_SYNTAX
#define PASCAL_SYNTAX

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


/*==========================================================================
 *------------------------------Класс Ident---------------------------------
 *==========================================================================
 *
 *==========================================================================
 */
class Ident {
    string      name;  // название переменной
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
        declare = false;
        assign  = false;
    }
    string get_name () const {
        return name;
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

//////////////////////  TID  ///////////////////////

vector <Ident> TID;  // Таблица идентификаторов TID

int put ( const string & buf ) {
    vector<Ident>::iterator k;

    if ( ( k = find ( TID.begin (), TID.end (), buf ) ) != TID.end () )
        return k - TID.begin();
    TID.push_back ( Ident(buf) );
    return TID.size () - 1;
}

/////////////////////////////////////////////////////////////////

class Scanner {
    FILE * fp;
    char   c;

    int look ( const string buf, const char ** list ) {
        int i = 0;
        while ( list[i] ) {
            if ( buf == list[i] )
                return i;
            ++i;
        }
        return 0;
    }

    void gc () {
        c = fgetc (fp);
    }
public:
    static const char * TW [], * TD [];

    Scanner ( const char * program ) {
        if ( !(fp = fopen ( program, "r" )) )
            throw  "can't open file" ;
    }

    Lex get_lex ();
};

const char *
        Scanner::TW    [] = { "", "and", "begin", "bool", "do", "else", "end", "if", "false", "int", "not", "or", "program",
                              "read", "then", "true", "var", "while", "write", NULL };

const char *
        Scanner::TD    [] = {"@", ";", ",", ":", ":=", "(", ")", "=", "<", ">", "+", "-", "*", "/", "<=", "!=", ">=", NULL };

Lex Scanner::get_lex () {
    enum    state { H, IDENT, NUMB, COM, ALE, NEQ };
    int     d, j;
    string  buf;
    state   CS = H;

    do {
        gc ();
        switch ( CS ) {
            case H:
                if ( c==' ' || c == '\n' || c== '\r' || c == '\t' );
                else if ( isalpha (c) ) {  // если буква
                    buf.push_back (c);
                    CS  = IDENT;
                }
                else if ( isdigit (c) ) {  // если цифра
                    d   = c - '0';
                    CS  = NUMB;
                }
                else if ( c== '{' ) {
                    CS  = COM;
                }
                else if ( c == ':' || c == '<' || c == '>' ) {
                    buf.push_back (c);
                    CS  = ALE;
                }
                else if (c == '@')
                    return Lex ( LEX_FIN );
                else if (c == '!') {
                    buf.push_back (c);
                    CS  = NEQ;
                }
                else {
                    buf.push_back (c);
                    if ( ( j = look ( buf, TD) ) ){
                        return Lex ( (type_of_lex)( j + (int) LEX_FIN ), j );
                    }
                    else
                        throw c;
                }
                break;
            case IDENT:
                if ( isalpha (c) || isdigit (c) ) {
                    buf.push_back (c);
                }
                else {
                    ungetc ( c, fp );
                    if ( (j = look ( buf, TW) ) ) {
                        return Lex ( (type_of_lex) j, j );
                    }
                    else {
                        j   = put ( buf );  // записываем в таблицу и возвр-ем номер
                        return Lex ( LEX_ID, j );
                    }
                }
                break;
            case NUMB:
                if ( isdigit (c) ) {
                    d = d * 10 + ( c - '0' );
                }
                else {
                    ungetc ( c, fp );
                    return Lex ( LEX_NUM, d );
                }
                break;
            case COM:
                if ( c == '}' ) {
                    CS  = H;
                }
                else if ( c == '@' || c == '{' )
                    throw c;
                break;
            case ALE:
                if ( c == '=' ) {
                    buf.push_back ( c );
                    j   = look ( buf, TD );
                    return Lex ( (type_of_lex) ( j + (int) LEX_FIN ), j );
                }
                else {
                    ungetc ( c, fp );
                    j   = look ( buf, TD );
                    return Lex ( (type_of_lex) ( j + (int) LEX_FIN ), j );
                }
                break;
            case NEQ:
                if ( c == '=' ) {
                    buf.push_back(c);
                    j   = look ( buf, TD );
                    return Lex ( LEX_NEQ, j );
                }
                else
                    throw '!';
                break;
        } //end switch
    } while (true);
}

ostream & operator<< ( ostream &s, Lex l ) {
    string t;
    if ( l.t_lex <= LEX_WRITE )
        t = Scanner::TW[l.t_lex];
    else if ( l.t_lex >= LEX_FIN && l.t_lex <= LEX_GEQ )
        t = Scanner::TD[ l.t_lex - LEX_FIN ];
    else if ( l.t_lex == LEX_NUM )
        t = "NUMB";
    else if ( l.t_lex == LEX_ID )
        t = TID[l.v_lex].get_name ();
    else if ( l.t_lex == POLIZ_LABEL )
        t = "Label";
    else if ( l.t_lex == POLIZ_ADDRESS )
        t = "Addr";
    else if ( l.t_lex == POLIZ_GO )
        t = "!";
    else if ( l.t_lex == POLIZ_FGO )
        t = "!F";
    else
        throw l;
    s << '(' << t << ',' << l.v_lex << ");" << endl;
    return s;
}

#endif //PASCAL_SYNTAX