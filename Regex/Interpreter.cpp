//
//  Interpreter.cpp
//  Regex
//
//  Created by Litherum on 7/26/15.
//  Copyright © 2015 Litherum. All rights reserved.
//

#include "Interpreter.h"

namespace Regex {

Interpreter::Interpreter(const DFA& dfa): dfa(dfa) {
}
    
bool Interpreter::run(const std::string& s) const {
    DFANode state(0);
    for (char c : s) {
        state = dfa.follow(state, c);
        if (state == DFA::invalidNode)
            return false;
    }
    return dfa.isEndNode(state);
}

}