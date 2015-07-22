//
//  DFA.cpp
//  Regex
//
//  Created by Litherum on 7/21/15.
//  Copyright (c) 2015 Litherum. All rights reserved.
//

#include <map>
#include <queue>
#include <set>
#include <cassert>

#include "DFA.h"
#include "NFA.h"

static NFANodeCollection epsilonClosure(const NFANode& node) {
    NFANodeCollection result;
    std::queue<std::reference_wrapper<const NFANode>> workQueue;
    workQueue.push(node);
    while (!workQueue.empty()) {
        const NFANode& current(workQueue.front());
        workQueue.pop();
        if (result.find(current) != result.end())
            continue;
        result.insert(current);
        current.iterateEdges([&](const std::unique_ptr<char>& c, const NFANode& neighbor) {
            if (c == nullptr)
                workQueue.push(neighbor);
        });
    }
    return result;
}

DFA::DFA(const NFANode& startNode, const NFANode& endNode): startNode(nullptr) {
    std::set<NFANodeCollection, NFANodeCollectionComparator> completed;
    std::map<NFANodeCollection, DFANode*, NFANodeCollectionComparator> map;
    std::queue<const NFANodeCollection> workQueue;
    NFANodeCollection closure(epsilonClosure(startNode));
    nodes.emplace_back();
    this->startNode = &nodes[0];
    map.insert(std::make_pair(closure, &nodes[0]));
    workQueue.emplace(std::move(closure));
    
    while (!workQueue.empty()) {
        const NFANodeCollection current(workQueue.front());
        workQueue.pop();
        const auto& i = map.find(current);
        assert(i != map.end());
        assert(i->second != nullptr);
        DFANode& dfaNode(*i->second);
        // This is a Map / Reduce operation
        std::map<char, NFANodeCollection> outgoingEdges;
        for (const NFANode& nfaNode : current) {
            nfaNode.iterateEdges([&](const std::unique_ptr<char>& c, const NFANode& neighbor) {
                if (c != nullptr) // epsilon
                    outgoingEdges.insert(std::make_pair(*c, NFANodeCollection())).first->second.insert(neighbor);
            });
        }
        for (const auto& outgoingEdge : outgoingEdges) {
            NFANodeCollection nfaNeighbor;
            for (const NFANode& n : outgoingEdge.second) {
                const NFANodeCollection& closure(epsilonClosure(n));
                nfaNeighbor.insert(closure.cbegin(), closure.cend());
            }
            auto p = map.insert(std::make_pair(nfaNeighbor, nullptr));
            if (p.second) {
                nodes.emplace_back();
                p.first->second = &nodes[nodes.size() - 1];
            }
            assert(p.first->second != nullptr);
            DFANode& dfaNeighbor(*p.first->second);
            dfaNode.addEdge(outgoingEdge.first, dfaNeighbor);
            if (completed.find(nfaNeighbor) == completed.end())
                workQueue.emplace(nfaNeighbor);
            completed.insert(std::move(nfaNeighbor));
        }
    }

    for (const auto& n : map) {
        if (n.first.find(endNode) != n.first.end())
            endNodes.emplace(*n.second);
    }
}