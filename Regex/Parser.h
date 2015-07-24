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

#include "NFA.h"

typedef std::size_t NFAHandle;
#include "ParserGrammar.hpp"

class RegexParser;
extern int regexparse(RegexParser*);

class RegexParser {
public:

    RegexParser(std::string string) : string(string), stringIndex(0), failed(false) {
    }

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

    NFAHandle alternate(NFAHandle a, NFAHandle b) {
        assert(a < nfaStorage.size() && b < nfaStorage.size());
        nfaStorage[a].alternate(std::move(nfaStorage[b]));
        return a;
    }

    NFAHandle star(NFAHandle a) {
        assert(a < nfaStorage.size());
        nfaStorage[a].star();
        return a;
    }

    NFAHandle concatenate(NFAHandle a, NFAHandle b) {
        assert(a < nfaStorage.size() && b < nfaStorage.size());
        nfaStorage[a].concatenate(std::move(nfaStorage[b]));
        return a;
    }

    NFAHandle literal(char c) {
        nfaStorage.emplace_back(NFA(c));
        return nfaStorage.size() - 1;
    }

    NFAHandle epsilon() {
        nfaStorage.emplace_back(NFA(0));
        return nfaStorage.size() - 1;
    }
    
private:
    std::vector<NFA> nfaStorage;
    std::string string;
    std::size_t stringIndex;
    bool failed;
};

inline int regexlex(YYSTYPE *lvalp, RegexParser* parser)
{
    char c = parser->readCharacter();
    if (c == '\\')
        c = parser->readCharacter();
    switch (c) {
    case 0:
        return 0;
    case '|':
    case '*':
    case '(':
    case ')':
        return c;
    default:
        lvalp->c = c;
        return CHARACTER;
    }
}

inline void regexerror(RegexParser* parser, const char *s)
{
    parser->fail();
}

#endif /* defined(__Regex__Parser__) */
