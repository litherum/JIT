//
//  main.cpp
//  Regex
//
//  Created by Litherum on 7/11/15.
//  Copyright © 2015 Litherum. All rights reserved.
//

#include <iostream>
#include <map>
#include <queue>

#include "AST.h"
#include "DFA.h"
#include "JIT.h"

extern "C" {
int yyparse();
char *thestring;
}

extern std::vector<std::unique_ptr<ASTNode>> nodeStorage;

static std::unique_ptr<Machine> compile(const DFA& dfa) {
    return std::unique_ptr<Machine>(new JIT(dfa));
}

int main(int argc, const char * argv[]) {
    thestring = "01*1(A|B)";
    yyparse();

    std::unique_ptr<Machine> m(compile(DFA(nodeStorage[nodeStorage.size() - 1]->resolve())));
    assert(m->run("0110") == false);
    assert(m->run("011A") == true);
    assert(m->run("011B") == true);
    assert(m->run("011AB") == false);
    assert(m->run("0111A") == true);
    assert(m->run("1") == false);
    assert(m->run("01B") == true);
    assert(m->run("00") == false);
    assert(m->run("01101") == false);
    assert(m->run("01111") == false);
    return 0;
}
