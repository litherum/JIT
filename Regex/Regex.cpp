//
//  Regex.cpp
//  Regex
//
//  Created by Litherum on 7/26/15.
//  Copyright © 2015 Litherum. All rights reserved.
//

#include "Regex.h"

#include "DFA.h"
#include "NFA.h"
#include "Parser.h"

namespace Regex {

JIT jit(const std::string& s, bool& success) {
    Parser parser(s);
    if (!parser.parse()) {
        success = false;
        return JIT(DFA(NFA(0)));
    }
    success = true;
    return JIT(DFA(parser.takeNFA()));
}

Interpreter interpret(const std::string& s, bool& success) {
    Parser parser(s);
    if (!parser.parse()) {
        success = false;
        return Interpreter(DFA(NFA(0)));
    }
    success = true;
    return Interpreter(DFA(parser.takeNFA()));
}

}