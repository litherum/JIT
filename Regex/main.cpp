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

#include "DFA.h"
#include "JIT.h"
#include "NFA.h"

static std::unique_ptr<Machine> compile(const DFA& dfa) {
    return std::unique_ptr<JIT>(new JIT(dfa.getNodes(), dfa.getStartNode(), dfa.getEndNodes()));
}

int main(int argc, const char * argv[]) {
    NFANode startNode;
    NFANode endNode;

    std::unique_ptr<Machine> m = compile(DFA(startNode, endNode));
    std::cout << m->run("110") << std::endl;
    std::cout << m->run("0") << std::endl;
    std::cout << m->run("1010") << std::endl;
    std::cout << m->run("11") << std::endl;
    std::cout << m->run("112") << std::endl;
    return 0;
}
