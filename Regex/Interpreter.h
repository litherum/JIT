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
#include "Machine.h"

namespace Regex {

class Interpreter: public Machine {
public:
    Interpreter(const DFA& dfa);
    virtual bool run(const std::string& s) const override;
    
private:
    DFA dfa;
};

}

#endif /* defined(__Regex__Interpreter__) */
