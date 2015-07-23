//
//  Interpreter.h
//  Regex
//
//  Created by Litherum on 7/21/15.
//  Copyright (c) 2015 Litherum. All rights reserved.
//

#ifndef __Regex__Interpreter__
#define __Regex__Interpreter__

#include "DFA.h"
#include "Machine.h"

class Interpreter: public Machine {
public:
    Interpreter(const DFA& dfa): dfa(dfa) {
    }
    
    virtual bool run(const std::string& s) const override {
        DFANode state(0);
        for (char c : s) {
            state = dfa.follow(state, c);
            if (state == DFA::invalidNode)
                return false;
        }
        return dfa.isEndNode(state);
    }
    
private:
    const DFA& dfa;
};

#endif /* defined(__Regex__Interpreter__) */
