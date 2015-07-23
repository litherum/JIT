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

#include "DFA.h"
#include "Machine.h"

class JIT: public Machine {
public:
    JIT(const DFA& dfa);
    
    virtual bool run(const std::string& s) const override;
    
private:
    class Tracker;
    
    class Unmapper {
    public:
        void operator()(void* ptr) const;
        
        void setLength(size_t length) {
            this->length = length;
        }
        
    private:
        size_t length;
    };
    
    std::unique_ptr<void, Unmapper> machineCode;
};

#endif /* defined(__Regex__JIT__) */
