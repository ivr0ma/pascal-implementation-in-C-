//
// Created by ivr0m on 04.05.2022.
//

#ifndef PASCAL_SCANNER_H
#define PASCAL_SCANNER_H

#include <iostream>
#include <string>
#include <cstdio>
#include <ctype.h>
#include <cstdlib>
#include <vector>
#include <stack>
#include <algorithm>

#include "Lex.h"
#include "Ident.h"

using namespace std;

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

#endif //PASCAL_SCANNER_H
