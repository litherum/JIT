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
    return std::unique_ptr<Machine>(new JIT(dfa.getNodes(), dfa.getStartNode(), dfa.getEndNodes()));
}

int main(int argc, const char * argv[]) {
    // 01+10
    NFANode nodes[5];
    nodes[0].addEdge(std::unique_ptr<char>(new char('0')), nodes[1]);
    nodes[1].addEdge(std::unique_ptr<char>(new char('1')), nodes[2]);
    nodes[2].addEdge(nullptr, nodes[1]);
    nodes[2].addEdge(std::unique_ptr<char>(new char('1')), nodes[3]);
    nodes[3].addEdge(std::unique_ptr<char>(new char('0')), nodes[4]);

    std::cout << "Nodes: ";
    for (const NFANode& node : nodes)
        std::cout << &node << " ";
    std::cout << std::endl;

    std::unique_ptr<Machine> m(compile(DFA(nodes[0], nodes[4])));
    std::cout << m->run("0110") << std::endl;
    std::cout << m->run("01110") << std::endl;
    std::cout << m->run("1") << std::endl;
    std::cout << m->run("010") << std::endl;
    std::cout << m->run("01101") << std::endl;
    std::cout << m->run("01111") << std::endl;
    return 0;
}
