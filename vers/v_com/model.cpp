/* 02.05.2022 */
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
    LEX_NULL,        /* 0*/
    LEX_AND, LEX_BEGIN, LEX_BOOL, LEX_DO, LEX_ELSE, LEX_END, LEX_IF, LEX_FALSE, LEX_INT,        /* 9*/
    LEX_NOT, LEX_OR, LEX_PROGRAM, LEX_READ, LEX_THEN, LEX_TRUE, LEX_VAR, LEX_WHILE, LEX_WRITE,  /*18*/
    LEX_FIN,             /*19   маркер конца  */
    LEX_SEMICOLON, LEX_COMMA, LEX_COLON, LEX_ASSIGN, LEX_LPAREN, LEX_RPAREN, LEX_EQ, LEX_LSS, /*27*/
    LEX_GTR, LEX_PLUS, LEX_MINUS, LEX_TIMES, LEX_SLASH, LEX_LEQ, LEX_NEQ, LEX_GEQ,              /*35*/
    LEX_NUM,     /*36*/
    LEX_ID,    /*37*/
    POLIZ_LABEL, //для ссылок на элементы полиза     /*38*/
    POLIZ_ADDRESS,   //обозначение операндов-адресов в таблице имён   /*39*/
    POLIZ_GO,   //безусловный переход     /*40*/
    POLIZ_FGO   //переход по лжи   /*41*/
};
 
/////////////////////////  Класс Lex  //////////////////////////
 
class Lex {
    type_of_lex   t_lex; //тип лексемы
    int           v_lex; //указатель на информацию о ней (ссылается на таблицу TID)
public:
    Lex ( type_of_lex t = LEX_NULL, int v = 0 ): t_lex (t), v_lex (v)  { } //так называемая лексема-пустышка, которая создаётся в начале и позднее будет заменена на настоящую лексему
    type_of_lex  get_type () const { //посмотреть тип лексемы
    	  return t_lex; 
    }
    int get_value () const { //посмотреть значение лексемы
    	  return v_lex; 
    }
  friend ostream & operator<< ( ostream &s, Lex l ); //напечатать лексему
};
 
/////////////////////  Класс Ident  ////////////////////////////
 
class Ident { //идентификатор
    string      name;
    bool        declare; //описана ли переменная
    type_of_lex type;
    bool        assign;  //объявленна ли переменная
    int          value;  //значение
public:
    Ident() { 
        declare = false; 
        assign  = false; 
    }
    bool operator== ( const string& s ) const { 
        return name == s; 
    }
    Ident ( const string n ) { //строка --> объект Ident
        name    = n;
        declare = false; 
        assign  = false;
    }
    // работа с полями - просмотр и присваивание 
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
 // 3 таблицы - TW (слов), TD (разделителей) и TID (идентификаторов - то, что мы получаем в программе)
vector<Ident> TID;
 
int put ( const string & buf ) { //метод put кладёт очередной идентификатор в таблицу 
    vector<Ident>::iterator k;
 
    if ( ( k = find ( TID.begin (), TID.end (), buf ) ) != TID.end () ) //ищем объект, совпадающий с buf
        return k - TID.begin(); //если он там есть, выдаём его позицию
    TID.push_back ( Ident(buf) ); //иначе кладём объект и возвращаем его позицию
    return TID.size () - 1;
}
 
/////////////////////////////////////////////////////////////////
 
class Scanner {
    FILE * fp; //текст программы
    char   c;
    int look ( const string buf, const char ** list ) { //приватная, т.к. нужна только классу Scaner 
    	//ищет строку, заданную в buf в таблицах TW & TD (эти таблицы это массив указателей на строки) 
    	//если ф-я нашла строку, то выдаёт её позицию, иначе 0
        int i = 0;
        while ( list[i] ) {
            if ( buf == list[i] )
                return i;
            ++i;
        }
        return 0;
    }
    void gc () { //считываем символ из файла с программой
        c = fgetc (fp);
    }
public:
    static const char * TW [], * TD [];
    Scanner ( const char * program ) { //открываем файл на чтение
        if ( !(fp = fopen ( program, "r" )) ) 
            throw  "can’t open file" ;
    }
    Lex get_lex ();
};
 
const char *
Scanner::TW    [] = { "", "and", "begin", "bool", "do", "else", "end", "if", "false", "int", "not", "or", "program",
                      "read", "then", "true", "var", "while", "write", NULL };
 
const char *
Scanner::TD    [] = { "@", ";", ",", ":", ":=", "(", ")", "=", "<", ">", "+", "-", "*", "/", "<=", "!=", ">=", NULL };
 
Lex Scanner::get_lex () {       // дай лексему! // выделяем очередную лексему с того момента в программе, где мы остановились в прошлый раз
// если такой лексемы нет, сивол, "плохой" символ выбрасывается исключением
    enum    state { H, IDENT, NUMB, COM, ALE, NEQ };
    int     d, j;
    string  buf;
    state   CS = H;
    do {
        gc ();
        switch ( CS ) {
            case H:
                if ( c==' ' || c == '\n' || c== '\r' || c == '\t' ); 
                    else if ( isalpha (c) ) {
                        buf.push_back (c);
                        CS  = IDENT;
                    }
                    else if ( isdigit (c) ) {
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
                        return Lex ( LEX_FIN ); //конец
                    else if (c == '!') {
                        buf.push_back (c);
                        CS  = NEQ;
                    }
                    else { // операции +-/*
                        buf.push_back (c);
                        if ( ( j = look ( buf, TD) ) ){
                            return Lex ( (type_of_lex)( j + (int) LEX_FIN ), j );
                        }
                        else
                            throw c;
                    }
                    break;
            case IDENT: //идентификатор
                if ( isalpha (c) || isdigit (c) ) {
                    buf.push_back (c); 
                }
                else {
                    ungetc ( c, fp );
                    if ( (j = look ( buf, TW) ) ) {
                        return Lex ( (type_of_lex) j, j );
                        }
                        else {
                        j   = put ( buf );
                        return Lex ( LEX_ID, j );
                    }
                }
                break;
            case NUMB: //число
                if ( isdigit (c) ) {
                    d = d * 10 + ( c - '0' );
                }
                else {
                    ungetc ( c, fp );
                    return Lex ( LEX_NUM, d );
                }
                break;
            case COM: //комментарий - пропускаем всё до выхода
                if ( c == '}' ) {
                    CS  = H;
                }
                else if ( c == '@' || c == '{' )
                    throw c;
                break;
            case ALE: // :, <, > - ищем в таблице разделителей
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
 
//////////////////////////  Класс Parser  /////////////////////////////////
template <class T, class T_EL>
void from_st ( T & st, T_EL & i ) { //вытаскивает элемент из вспомогательного стека
    i = st.top(); st.pop();
}
 
class Parser {
    Lex          curr_lex; //текущая лексема
    type_of_lex  c_type;
    int          c_val;
    Scanner      scan; //Scanner является частью Parser'a
    stack < int >           st_int; //для семантического анализа - запонимаем здесь описанные переменные
    stack < type_of_lex >   st_lex; //семантический стек - хранятся типы операндов и обозначение операций
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
    vector <Lex> poliz;   // !! в этом объекте размещается генерируемая программа
    Parser ( const char *program ) : scan (program) { }
    void  analyze();
};
 
void Parser::analyze () { //метод, который смотрит что мы дошли до конца
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
        while ( c_type == LEX_COMMA ) {  //пока запятая, заносим в стек обработки описаний номера идентификаторов из TID 
            gl ();
            if ( c_type != LEX_ID )
                throw curr_lex;
            else {
                st_int.push ( c_val );
                gl ();
            }
        }
        if ( c_type != LEX_COLON )  //встретили двоеточие, вызываем метод dec который занесёт информацию об элементах в Ident
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
    while ( !st_int.empty () ) {  //вынимаем элементы(точнее их номера в таблице TID из вспомогательного стека индентификаторов
        from_st ( st_int, i );
        if ( TID[i].get_declare () )  //заносим информацию о них в класс Ident
            throw "twice";            //и выдаём ошибку если описано дважды
        else {
            TID[i].put_declare ();
            TID[i].put_type ( type );
        }
    }
}
 
void Parser::check_id () {        //описана ли переменная
    if ( TID[c_val].get_declare() )
        st_lex.push ( TID[c_val].get_type () ); //если да, заносим в семантический стек (операции и типы операндов)
    else 
        throw "not declared";
}
 
void Parser::check_op () {      //контроль совпадения типов внутри тройки "операнд операция операнд"
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
    poliz.push_back (Lex (op) );      //добавляем в ПОЛИЗ операцию op
}
 
void Parser::check_not () {      //контроль операции not
    if (st_lex.top() != LEX_BOOL)
        throw "wrong type is in not";
    else  
        poliz.push_back ( Lex (LEX_NOT) );  //добавляем в ПОЛИЗ операцию not
}
 
void Parser::eq_type () {        //контроль операции присваивания
    type_of_lex t;
    from_st ( st_lex, t );
    if ( t != st_lex.top () )
        throw "wrong types are in :=";
    st_lex.pop();
}
 
void Parser::eq_bool () {       //контроль выражения перехода (только bool в условиях перехода if и while)
    if ( st_lex.top () != LEX_BOOL )
        throw "expression is not boolean";
    st_lex.pop ();
  }
 
void Parser::check_id_in_read () {    //описана ли переменная, которую мы считываем 
    if ( !TID [c_val].get_declare() )
        throw "not declared";
}
 
////////////////////////////////////////////////////////////////
 
class Executer {  //выполнение программы (метод execute)
public:
    void execute ( vector<Lex> & poliz );
};
 
void Executer::execute ( vector<Lex> & poliz ) { //в качестве аргумента получается вектор poliz, составленный из лексем
    Lex pc_el;     //текущий элемент, который мы интерпретируем
    stack < int > args;   //стек, с помощью которого мы интерпретируем ПОЛИЗ
    int i, j, index = 0, size = poliz.size();
    while ( index < size ) {  //size - последняя позиция в ПОЛИЗЕ
        pc_el = poliz [ index ];
        switch ( pc_el.get_type () ) {
            case LEX_TRUE: case LEX_FALSE: case LEX_NUM: case POLIZ_ADDRESS: case POLIZ_LABEL:  //значение константы кладём в стек
                args.push ( pc_el.get_value () ); 
                break;
 
            case LEX_ID:    //переменная 
                i = pc_el.get_value ();
                if ( TID[i].get_assign () ) {   //ошибка если переменной не было присвоено значение (смотрим по TID)
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
                index = i - 1;   //переход на метку i-1 так как в конце цикла i++
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
                TID[i].put_assign (); //поднимается флаг assign (переменной было присвоено значение)
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
 
class Interpretator { //здесь объекты классов Parser и Executer
    Parser   pars;
    Executer E;
public:
    Interpretator ( const char* program ): pars (program) {}  //получаем имя программы с кодом
    void     interpretation ();
};
 
void Interpretator::interpretation () {
    pars.analyze ();  //анализ (лексический, синтаксический, семантический + код в ПОЛИЗЕ)
    E.execute ( pars.poliz );
}
 
int main () {
    try {
        Interpretator I ( "prog.txt" );
        I.interpretation ();
        return 0;
    }
    catch ( char c ) {  //лексическая ошибка
        cout << "unexpected symbol " << c << endl;
        return 1;
    }
    catch ( Lex l ) {   //синтаксическая ошибка
        cout << "unexpected lexeme" << l << endl;
        return 1;
    }
    catch ( const char *source ) {  //семантическая ошибка или ошибка выполнения 
        cout << source << endl;
        return 1;
    }
}
