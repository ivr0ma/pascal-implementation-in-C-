#include <iostream>
#include <string>
#include <cstdio>
#include <ctype.h>
#include <cstdlib>
#include <vector>
#include <stack>
#include <algorithm>

#include "syntax.cpp"

using namespace std;


//////////////////////////  Класс Parser  /////////////////////////////////
template <class T, class T_EL>
void from_st ( T & st, T_EL & i ) {
    i = st.top(); st.pop();
}

class Parser {
    Lex          curr_lex;
    type_of_lex  c_type;
    int          c_val;
    Scanner      scan;
    stack < int >           st_int;
    stack < type_of_lex >   st_lex;
    void  P();
    void  D1();
    void  D();
    void  B();
    void  S();
    void  E();
    void  E1();
    void  T();
    void  F();
    void  dec ( type_of_lex type);
    void  check_id ();
    void  check_op ();
    void  check_not ();
    void  eq_type ();
    void  eq_bool ();
    void  check_id_in_read ();
    void  gl () {
        curr_lex  = scan.get_lex ();
        c_type    = curr_lex.get_type ();
        c_val     = curr_lex.get_value ();
    }
public:
    vector <Lex> poliz;
    Parser ( const char *program ) : scan (program) { }
    void  analyze();
};

void Parser::analyze () {
    gl ();
    P ();
    if (c_type != LEX_FIN)
        throw curr_lex;
    //for_each( poliz.begin(), poliz.end(), [](Lex l){ cout << l; });
    for ( Lex l : poliz )
        cout << l;
    cout << endl << "Yes!!!" << endl;
}

void Parser::P () {
    if ( c_type == LEX_PROGRAM ) {
        gl ();
    }
    else
        throw curr_lex;
    D1 ();
    if ( c_type == LEX_SEMICOLON )
        gl ();
    else
        throw curr_lex;
    B ();
}

void Parser::D1 () {
    if ( c_type == LEX_VAR ) {
        gl ();
        D ();
        while ( c_type == LEX_COMMA ) {
            gl ();
            D ();
        }
    }
    else
        throw curr_lex;
}

void Parser::D () {
    if ( c_type != LEX_ID )
        throw curr_lex;
    else {
        st_int.push ( c_val );
        gl ();
        while ( c_type == LEX_COMMA ) {
            gl ();
            if ( c_type != LEX_ID )
                throw curr_lex;
            else {
                st_int.push ( c_val );
                gl ();
            }
        }
        if ( c_type != LEX_COLON )
            throw curr_lex;
        else {
            gl ();
            if ( c_type == LEX_INT ) {
                dec ( LEX_INT );
                gl ();
            }
            else
            if ( c_type == LEX_BOOL ) {
                dec ( LEX_BOOL );
                gl ();
            }
            else
                throw curr_lex;
        }
    }
}

void Parser::B () {
    if ( c_type == LEX_BEGIN ) {
        gl ();
        S ();
        while ( c_type == LEX_SEMICOLON ) {
            gl ();
            S ();
        }
        if ( c_type == LEX_END ) {
            gl ();
        }
        else {
            throw curr_lex;
        }
    }
    else
        throw curr_lex;
}

void Parser::S () {
    int pl0, pl1, pl2, pl3;

    if ( c_type == LEX_IF ) {
        gl ();
        E ();
        eq_bool ();
        pl2 = poliz.size();
        poliz.push_back ( Lex() );
        poliz.push_back ( Lex(POLIZ_FGO) );
        if ( c_type == LEX_THEN ) {
            gl ();
            S ();
            pl3 = poliz.size ();
            poliz.push_back ( Lex () );

            poliz.push_back ( Lex ( POLIZ_GO ) );
            poliz[pl2] = Lex ( POLIZ_LABEL, poliz.size() );

            if ( c_type == LEX_ELSE ) {
                gl ();
                S ();
                poliz[pl3] = Lex ( POLIZ_LABEL, poliz.size() );
            }
            else
                throw curr_lex;
        }
        else
            throw curr_lex;
    }//end if
    else if ( c_type == LEX_WHILE ) {
        pl0 = poliz.size ();
        gl ();
        E ();
        eq_bool ();
        pl1 = poliz.size ();
        poliz.push_back ( Lex () );
        poliz.push_back ( Lex (POLIZ_FGO) );
        if ( c_type == LEX_DO ) {
            gl();
            S();
            poliz.push_back ( Lex ( POLIZ_LABEL, pl0 ) );
            poliz.push_back ( Lex ( POLIZ_GO) );
            poliz[pl1] = Lex ( POLIZ_LABEL, poliz.size() );
        }
        else
            throw curr_lex;
    }//end while
    else if ( c_type == LEX_READ ) {
        gl ();
        if ( c_type == LEX_LPAREN ) {
            gl ();
            if ( c_type == LEX_ID ) {
                check_id_in_read ();
                poliz.push_back ( Lex( POLIZ_ADDRESS, c_val) );
                gl();
            }
            else
                throw curr_lex;
            if ( c_type == LEX_RPAREN ) {
                gl ();
                poliz.push_back ( Lex (LEX_READ) );
            }
            else
                throw curr_lex;
        }
        else
            throw curr_lex;
    }//end read
    else if ( c_type == LEX_WRITE ) {
        gl ();
        if ( c_type == LEX_LPAREN ) {
            gl ();
            E ();
            if ( c_type == LEX_RPAREN ) {
                gl ();
                poliz.push_back ( Lex ( LEX_WRITE ) );
            }
            else
                throw curr_lex;
        }
        else
            throw curr_lex;
    }//end write
    else if ( c_type == LEX_ID ) {
        check_id ();
        poliz.push_back (Lex ( POLIZ_ADDRESS, c_val ) );
        gl();
        if ( c_type == LEX_ASSIGN ) {
            gl ();
            E ();
            eq_type ();
            poliz.push_back ( Lex ( LEX_ASSIGN ) );
        }
        else
            throw curr_lex;
    }//assign-end
    else
        B();
}

void Parser::E () {
    E1 ();
    if ( c_type == LEX_EQ  || c_type == LEX_LSS || c_type == LEX_GTR ||
         c_type == LEX_LEQ || c_type == LEX_GEQ || c_type == LEX_NEQ ) {
        st_lex.push ( c_type );
        gl ();
        E1 ();
        check_op ();
    }
}

void Parser::E1 () {
    T ();
    while ( c_type == LEX_PLUS || c_type == LEX_MINUS || c_type == LEX_OR) {
        st_lex.push ( c_type );
        gl ();
        T ();
        check_op ();
    }
}

void Parser::T () {
    F ();
    while ( c_type == LEX_TIMES || c_type == LEX_SLASH || c_type == LEX_AND) {
        st_lex.push ( c_type );
        gl ();
        F ();
        check_op ();
    }
}

void Parser::F () {
    if ( c_type == LEX_ID ) {
        check_id ();
        poliz.push_back ( Lex ( LEX_ID, c_val ) );
        gl ();
    }
    else if ( c_type == LEX_NUM ) {
        st_lex.push ( LEX_INT );
        poliz.push_back ( curr_lex );
        gl ();
    }
    else if ( c_type == LEX_TRUE ) {
        st_lex.push ( LEX_BOOL );
        poliz.push_back ( Lex (LEX_TRUE, 1) );
        gl ();
    }
    else if ( c_type == LEX_FALSE) {
        st_lex.push ( LEX_BOOL );
        poliz.push_back ( Lex (LEX_FALSE, 0) );
        gl ();
    }
    else if ( c_type == LEX_NOT ) {
        gl ();
        F ();
        check_not ();
    }
    else if ( c_type == LEX_LPAREN ) {
        gl ();
        E ();
        if ( c_type == LEX_RPAREN)
            gl ();
        else
            throw curr_lex;
    }
    else
        throw curr_lex;
}

////////////////////////////////////////////////////////////////

void Parser::dec ( type_of_lex type ) {
    int i;
    while ( !st_int.empty () ) {
        from_st ( st_int, i );
        if ( TID[i].get_declare () )
            throw "twice";
        else {
            TID[i].put_declare ();
            TID[i].put_type ( type );
        }
    }
}

void Parser::check_id () {
    if ( TID[c_val].get_declare() )
        st_lex.push ( TID[c_val].get_type () );
    else
        throw "not declared";
}

void Parser::check_op () {
    type_of_lex t1, t2, op, t = LEX_INT, r = LEX_BOOL;

    from_st ( st_lex, t2 );
    from_st ( st_lex, op );
    from_st ( st_lex, t1 );

    if ( op == LEX_PLUS || op == LEX_MINUS || op == LEX_TIMES || op == LEX_SLASH )
        r = LEX_INT;
    if ( op == LEX_OR || op == LEX_AND )
        t = LEX_BOOL;
    if ( t1 == t2  &&  t1 == t )
        st_lex.push (r);
    else
        throw "wrong types are in operation";
    poliz.push_back (Lex (op) );
}

void Parser::check_not () {
    if (st_lex.top() != LEX_BOOL)
        throw "wrong type is in not";
    else
        poliz.push_back ( Lex (LEX_NOT) );
}

void Parser::eq_type () {
    type_of_lex t;
    from_st ( st_lex, t );
    if ( t != st_lex.top () )
        throw "wrong types are in :=";
    st_lex.pop();
}

void Parser::eq_bool () {
    if ( st_lex.top () != LEX_BOOL )
        throw "expression is not boolean";
    st_lex.pop ();
}

void Parser::check_id_in_read () {
    if ( !TID [c_val].get_declare() )
        throw "not declared";
}

////////////////////////////////////////////////////////////////

class Executer {
public:
    void execute ( vector<Lex> & poliz );
};

void Executer::execute ( vector<Lex> & poliz ) {
    Lex pc_el;
    stack < int > args;
    int i, j, index = 0, size = poliz.size();
    while ( index < size ) {
        pc_el = poliz [ index ];
        switch ( pc_el.get_type () ) {
            case LEX_TRUE: case LEX_FALSE: case LEX_NUM: case POLIZ_ADDRESS: case POLIZ_LABEL:
                args.push ( pc_el.get_value () );
                break;

            case LEX_ID:
                i = pc_el.get_value ();
                if ( TID[i].get_assign () ) {
                    args.push ( TID[i].get_value () );
                    break;
                }
                else
                    throw "POLIZ: indefinite identifier";

            case LEX_NOT:
                from_st ( args, i );
                args.push( !i );
                break;

            case LEX_OR:
                from_st ( args, i );
                from_st ( args, j );
                args.push ( j || i );
                break;

            case LEX_AND:
                from_st ( args, i );
                from_st ( args, j );
                args.push ( j && i );
                break;

            case POLIZ_GO:
                from_st ( args, i );
                index = i - 1;
                break;

            case POLIZ_FGO:
                from_st ( args, i );
                from_st ( args, j );
                if ( !j ) index = i - 1;
                break;

            case LEX_WRITE:
                from_st ( args, j );
                cout << j << endl;
                break;

            case LEX_READ:
                int k;
                from_st ( args, i );
                if ( TID[i].get_type () == LEX_INT ) {
                    cout << "Input int value for" << TID[i].get_name () << endl;
                    cin >> k;
                }
                else {
                    string j;
                    while (1) {
                        cout << "Input boolean value (true or false) for" << TID[i].get_name() << endl;
                        cin >> j;
                        if ( j != "true" && j != "false" ) {
                            cout << "Error in input:true/false" << endl;
                            continue;
                        }
                        k = ( j == "true" ) ? 1 : 0;
                        break;
                    }
                }
                TID[i].put_value (k);
                TID[i].put_assign ();
                break;

            case LEX_PLUS:
                from_st ( args, i );
                from_st ( args, j );
                args.push ( i + j );
                break;

            case LEX_TIMES:
                from_st ( args, i );
                from_st ( args, j );
                args.push ( i * j );
                break;

            case LEX_MINUS:
                from_st ( args, i );
                from_st ( args, j );
                args.push ( j - i );
                break;

            case LEX_SLASH:
                from_st ( args, i );
                from_st ( args, j );
                if (!i) {
                    args.push ( j / i );
                    break;
                }
                else
                    throw "POLIZ:divide by zero";

            case LEX_EQ:
                from_st ( args, i );
                from_st ( args, j );
                args.push ( i == j );
                break;

            case LEX_LSS:
                from_st ( args, i );
                from_st ( args, j );
                args.push ( j < i );
                break;

            case LEX_GTR:
                from_st ( args, i );
                from_st ( args, j );
                args.push ( j > i );
                break;

            case LEX_LEQ:
                from_st ( args, i );
                from_st ( args, j );
                args.push ( j <= i );
                break;

            case LEX_GEQ:
                from_st ( args, i );
                from_st ( args, j );
                args.push ( j >= i );
                break;

            case LEX_NEQ:
                from_st ( args, i );
                from_st ( args, j );
                args.push ( j != i );
                break;

            case LEX_ASSIGN:
                from_st ( args, i );
                from_st ( args, j );
                TID[j].put_value (i);
                TID[j].put_assign ();
                break;

            default:
                throw "POLIZ: unexpected elem";
        }//end of switch
        ++index;
    };//end of while
    cout << "Finish of executing!!!" << endl;
}

class Interpretator {
    Parser   pars;
    Executer E;
public:
    Interpretator ( const char* program ): pars (program) {}
    void     interpretation ();
};

void Interpretator::interpretation () {
    pars.analyze ();
    E.execute ( pars.poliz );
}

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