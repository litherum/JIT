//
//  ASTParserHelpers.cpp
//  Regex
//
//  Created by Litherum on 7/24/15.
//  Copyright (c) 2015 Litherum. All rights reserved.
//

extern "C" {
#include "ASTParserHelpers.h"
}

#include "AST.h"

std::vector<std::unique_ptr<ASTNode>> nodeStorage;

ASTNodeForParsing concatenation(ASTNodeForParsing a, ASTNodeForParsing b) {
    std::unique_ptr<ASTNode> node(new Concatenation(*static_cast<ASTNode*>(a), *static_cast<ASTNode*>(b)));
    ASTNodeForParsing result(node.get());
    nodeStorage.emplace_back(std::move(node));
    return result;
}

ASTNodeForParsing star(ASTNodeForParsing a) {
    std::unique_ptr<ASTNode> node(new Star(*static_cast<ASTNode*>(a)));
    ASTNodeForParsing result(node.get());
    nodeStorage.emplace_back(std::move(node));
    return result;
}

ASTNodeForParsing alternation(ASTNodeForParsing a, ASTNodeForParsing b) {
    std::unique_ptr<ASTNode> node(new Alternation(*static_cast<ASTNode*>(a), *static_cast<ASTNode*>(b)));
    ASTNodeForParsing result(node.get());
    nodeStorage.emplace_back(std::move(node));
    return result;
}

ASTNodeForParsing literal(char c) {
    std::unique_ptr<ASTNode> node(new Literal(c));
    ASTNodeForParsing result(node.get());
    nodeStorage.emplace_back(std::move(node));
    return result;
}

ASTNodeForParsing epsilon() {
    std::unique_ptr<ASTNode> node(new Epsilon());
    ASTNodeForParsing result(node.get());
    nodeStorage.emplace_back(std::move(node));
    return result;
}