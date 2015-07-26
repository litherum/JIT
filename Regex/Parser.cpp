//
//  Parser.cpp
//  Regex
//
//  Created by Litherum on 7/26/15.
//  Copyright © 2015 Litherum. All rights reserved.
//

#include "Parser.h"

namespace Regex {

Parser::Parser(const std::string& string) : string(string), stringIndex(0), failed(false) {
}

NFAHandle Parser::alternate(NFAHandle a, NFAHandle b) {
    assert(a < nfaStorage.size() && b < nfaStorage.size());
    nfaStorage[a].alternate(std::move(nfaStorage[b]));
    return a;
}

NFAHandle Parser::star(NFAHandle a) {
    assert(a < nfaStorage.size());
    nfaStorage[a].star();
    return a;
}

NFAHandle Parser::plus(NFAHandle a) {
    assert(a < nfaStorage.size());
    nfaStorage[a].plus();
    return a;
}

NFAHandle Parser::concatenate(NFAHandle a, NFAHandle b) {
    assert(a < nfaStorage.size() && b < nfaStorage.size());
    nfaStorage[a].concatenate(std::move(nfaStorage[b]));
    return a;
}

NFAHandle Parser::literal(char c) {
    nfaStorage.emplace_back(NFA(c));
    return nfaStorage.size() - 1;
}

NFAHandle Parser::epsilon() {
    nfaStorage.emplace_back(NFA(0));
    return nfaStorage.size() - 1;
}

NFAHandle Parser::dot() {
    CSet s;
    static_assert(sizeof(int) > sizeof(char), "std::numeric_limits<char>::max() + 1 needs to be representable by int");
    for (int i = 1; i <= std::numeric_limits<char>::max(); ++i)
        s.insert(i);
    return csetImpl(s);
}

CSetHandle Parser::crange(char c) {
    CSet result = { c };
    csetStorage.emplace_back(std::move(result));
    return csetStorage.size() - 1;
}

CSetHandle Parser::crange(char a, char b) {
    CSet result;
    static_assert(sizeof(int) > sizeof(char), "std::numeric_limits<char>::max() + 1 needs to be representable by int");
    for (int i = a; i <= b; ++i)
        result.insert(i);
    csetStorage.emplace_back(std::move(result));
    return csetStorage.size() - 1;
}

CSetHandle Parser::csetUnion(CSetHandle a, CSetHandle b) {
    assert(a < csetStorage.size() && b < csetStorage.size());
    CSet& as(csetStorage[a]);
    const CSet& bs(csetStorage[b]);
    as.insert(bs.begin(), bs.end());
    return a;
}

NFAHandle Parser::cset(CSetHandle s) {
    assert(s < csetStorage.size());
    const CSet& ss(csetStorage[s]);
    return csetImpl(ss);
}

NFAHandle Parser::ncset(CSetHandle s) {
    assert(s < csetStorage.size());
    const CSet& ss(csetStorage[s]);
    CSet neg;
    static_assert(sizeof(int) > sizeof(char), "std::numeric_limits<char>::max() + 1 needs to be representable by int");
    for (int i = 1; i <= std::numeric_limits<char>::max(); ++i) {
        if (ss.find(i) == ss.end())
            neg.insert(i);
    }
    return csetImpl(neg);
}

NFAHandle Parser::csetImpl(const CSet& ss) {
    assert(!ss.empty());
    auto i(ss.begin());
    NFA result(*i);
    for (++i; i != ss.end(); ++i)
        result.alternate(NFA(*i));
    nfaStorage.emplace_back(std::move(result));
    return nfaStorage.size() - 1;
}

}

int regexlex(YYSTYPE *lvalp, Regex::Parser* parser)
{
    char c = parser->readCharacter();
    if (c == '\\') {
        lvalp->c = parser->readCharacter();
        return CHARACTER;
    }
    switch (c) {
    case 0:
        return 0;
    case '|':
    case '*':
    case '+':
    case '(':
    case ')':
    case '[':
    case ']':
    case '-':
    case '.':
        return c;
    default:
        lvalp->c = c;
        return CHARACTER;
    }
}

void regexerror(Regex::Parser* parser, const char *s)
{
    parser->fail();
}