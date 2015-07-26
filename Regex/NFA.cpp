//
//  NFA.cpp
//  Regex
//
//  Created by Litherum on 7/26/15.
//  Copyright © 2015 Litherum. All rights reserved.
//

#include "NFA.h"

namespace Regex {

bool NFANodeCollectionComparator::operator() (const NFANodeCollection& nc1, const NFANodeCollection& nc2) const {
    // Compare lexicographically
    for (auto i1(nc1.begin()), i2(nc2.begin()); i1 != nc1.end() && i2 != nc2.end(); ++i1, ++i2) {
        if (&i1->get() < &i2->get())
            return true;
        if (&i1->get() > &i2->get())
            return false;
    }
    return nc1.size() < nc2.size();
}

NFA::NFA(char c) {
    std::unique_ptr<NFANode> start(new NFANode());
    std::unique_ptr<NFANode> end(new NFANode());
    start->addEdge(c, *end.get());
    startNode = start.get();
    endNode = end.get();
    nodes.emplace_back(std::move(start));
    nodes.emplace_back(std::move(end));
}

void NFA::concatenate(NFA&& nfa) {
    endNode->addEdge(0, *nfa.startNode);
    endNode = nfa.endNode;
    takeNodes(std::move(nfa));
}

void NFA::star() {
    endNode->addEdge(0, *startNode);
    std::unique_ptr<NFANode> start(new NFANode());
    std::unique_ptr<NFANode> end(new NFANode());
    start->addEdge(0, *end);
    start->addEdge(0, *startNode);
    endNode->addEdge(0, *end);
    startNode = start.get();
    endNode = end.get();
    nodes.emplace_back(std::move(start));
    nodes.emplace_back(std::move(end));
}

void NFA::plus() {
    endNode->addEdge(0, *startNode);
}

void NFA::alternate(NFA&& nfa) {
    std::unique_ptr<NFANode> start(new NFANode());
    start->addEdge(0, *startNode);
    start->addEdge(0, *nfa.startNode);
    std::unique_ptr<NFANode> end(new NFANode());
    endNode->addEdge(0, *end);
    nfa.endNode->addEdge(0, *end);
    startNode = start.get();
    endNode = end.get();
    takeNodes(std::move(nfa));
    nodes.emplace_back(std::move(start));
    nodes.emplace_back(std::move(end));
}

}