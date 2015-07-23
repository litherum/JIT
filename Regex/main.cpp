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
    return std::unique_ptr<Machine>(new JIT(dfa));
}

int main(int argc, const char * argv[]) {
    // 01*1(A|B)
    NFA a('0');
    NFA b('1');
    b.star();
    a.concatenate(std::move(b));
    a.concatenate(NFA('1'));
    NFA c('A');
    c.alternate(NFA('B'));
    a.concatenate(std::move(c));

    std::unique_ptr<Machine> m(compile(DFA(a)));
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
