//
//  JIT.h
//  Regex
//
//  Created by Litherum on 7/21/15.
//  Copyright (c) 2015 Litherum. All rights reserved.
//

#ifndef __Regex__JIT__
#define __Regex__JIT__

#include <memory>

namespace Regex {
class DFA;

class JIT {
public:
    JIT(const DFA&);
    bool operator()(const std::string& s) const;
    
private:
    class Tracker;

    std::shared_ptr<void> machineCode;
};

}

#endif /* defined(__Regex__JIT__) */
