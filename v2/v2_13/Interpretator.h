#ifndef PASCAL_INTERPRETATOR_H
#define PASCAL_INTERPRETATOR_H

#include <iostream>
#include <string>
#include <cstdio>
#include <ctype.h>
#include <cstdlib>
#include <vector>
#include <stack>
#include <algorithm>

#include "Parser.h"
#include "Executer.h"

/*==========================================================================
 *--------------------------Класс Interpretator-----------------------------
 *==========================================================================
 * Интерпретация входного файла с программой. Содержит объекты
 * классов Parser и Executer
 *==========================================================================
 */
class Interpretator {
    Parser   pars;
    Executer E;
public:
    Interpretator ( const char* program ): pars (program) {}
    void     interpretation ();
};

/*==========================================================================
 * лексический, синтаксический, семантический анализ
 *==========================================================================
 */
void Interpretator::interpretation () {
    pars.analyze ();
    E.execute ( pars.poliz );
}

#endif //PASCAL_INTERPRETATOR_H