//
// Created by ivr0m on 04.05.2022.
//

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

#endif //PASCAL_INTERPRETATOR_H
