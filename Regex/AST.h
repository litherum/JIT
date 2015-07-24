//
//  AST.h
//  Regex
//
//  Created by Litherum on 7/23/15.
//  Copyright (c) 2015 Litherum. All rights reserved.
//

#ifndef __Regex__AST__
#define __Regex__AST__

#include "NFA.h"

// FIXME: These classes are all unnecessary. The parser can just build the NFAs directly.

class ASTNode {
public:
    virtual NFA resolve() const = 0;
    virtual ~ASTNode() {
    }
};

class Concatenation : public ASTNode {
public:
    Concatenation(const ASTNode& l, const ASTNode& r): l(l), r(r) {
    }
    
    virtual NFA resolve() const override {
        NFA result(l.resolve());
        result.concatenate(r.resolve());
        return result;
    }
    
private:
    const ASTNode& l;
    const ASTNode& r;
};

class Star : public ASTNode {
public:
    Star(const ASTNode& c): c(c) {
    }
    
    virtual NFA resolve() const override {
        NFA result(c.resolve());
        result.star();
        return result;
    }
    
private:
    const ASTNode& c;
};

class Alternation : public ASTNode {
public:
    Alternation(const ASTNode& l, const ASTNode& r): l(l), r(r) {
    }
    
    virtual NFA resolve() const override {
        NFA result(l.resolve());
        result.alternate(r.resolve());
        return result;
    }
    
private:
    const ASTNode& l;
    const ASTNode& r;
};

class Literal : public ASTNode {
public:
    Literal(char c): c(c) {
    }

    virtual NFA resolve() const override {
        return NFA(c);
    }

private:
    char c;
};

class Epsilon : public Literal {
public:
    Epsilon(): Literal(0) {
    }
};

#endif /* defined(__Regex__AST__) */
