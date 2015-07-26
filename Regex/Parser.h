//
//  Parser.h
//  Regex
//
//  Created by Litherum on 7/24/15.
//  Copyright (c) 2015 Litherum. All rights reserved.
//

#ifndef __Regex__Parser__
#define __Regex__Parser__

#include <cassert>
#include <string>
#include <unordered_set>

#include "NFA.h"

typedef std::size_t NFAHandle;
typedef std::unordered_set<char> CSet;
typedef std::size_t CSetHandle;
#include "ParserGrammar.hpp"

namespace Regex {
class Parser;
}

extern int regexparse(Regex::Parser*);

namespace Regex {

class Parser {
public:
    Parser(const std::string& string);

    bool parse() {
        regexparse(this);
        return !failed;
    }

    void fail() {
        failed = true;
    }

    NFA takeNFA() {
        if (nfaStorage.empty())
            epsilon();
        return std::move(nfaStorage[0]);
    }

    char readCharacter() {
        return string[stringIndex++];
    }

    NFAHandle alternate(NFAHandle a, NFAHandle b);
    NFAHandle star(NFAHandle a);
    NFAHandle plus(NFAHandle a);
    NFAHandle concatenate(NFAHandle a, NFAHandle b);
    NFAHandle literal(char c);
    NFAHandle epsilon();
    NFAHandle dot();
    CSetHandle crange(char c);
    CSetHandle crange(char a, char b);
    CSetHandle csetUnion(CSetHandle a, CSetHandle b);
    NFAHandle cset(CSetHandle s);
    NFAHandle ncset(CSetHandle s);
    
private:

    NFAHandle csetImpl(const CSet& ss);

    std::vector<NFA> nfaStorage;
    std::vector<CSet> csetStorage;
    std::string string;
    std::size_t stringIndex;
    bool failed;
};

}

int regexlex(YYSTYPE *lvalp, Regex::Parser* parser);
void regexerror(Regex::Parser* parser, const char *s);

#endif /* defined(__Regex__Parser__) */
