//
//  Machine.h
//  Regex
//
//  Created by Litherum on 7/21/15.
//  Copyright (c) 2015 Litherum. All rights reserved.
//

#ifndef Regex_Machine_h
#define Regex_Machine_h

#include <string>

namespace Regex {

class Machine {
public:
    virtual ~Machine() {
    }
    
    virtual bool run(const std::string& s) const = 0;
};

}

#endif
