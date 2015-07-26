//
//  Regex.h
//  Regex
//
//  Created by Litherum on 7/26/15.
//  Copyright © 2015 Litherum. All rights reserved.
//

#include <string>

#include "Interpreter.h"
#include "JIT.h"

namespace Regex {

JIT jit(const std::string& s, bool& success);
Interpreter interpret(const std::string& s, bool& success);

}