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
    Interpreter(const DFANode& startNode, const DFANode& endNode): startNode(startNode), endNode(endNode) {
    }
    
    virtual bool run(const std::string& s) const override {
        const DFANode* state = &startNode;
        for (char c : s) {
            if (const DFANode* n = state->follow(c))
                state = n;
            else
                return false;
        }
        return state == &endNode;
    }
    
private:
    const DFANode& startNode;
    const DFANode& endNode;
};

#endif /* defined(__Regex__Interpreter__) */
