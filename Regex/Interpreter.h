//
//  Interpreter.h
//  Regex
//
//  Created by Litherum on 7/21/15.
//  Copyright (c) 2015 Litherum. All rights reserved.
//

#ifndef __Regex__Interpreter__
#define __Regex__Interpreter__

#include <string>

#include "DFA.h"

namespace Regex {

class Interpreter {
public:
    Interpreter(const DFA& dfa);
    bool operator()(const std::string& s) const;
    
private:
    DFA dfa;
};

}

#endif /* defined(__Regex__Interpreter__) */
