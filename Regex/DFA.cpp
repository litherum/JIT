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
#include <iostream>

#include "DFA.h"
#include "NFA.h"

namespace Regex {

const DFANode DFA::invalidNode = std::numeric_limits<DFANode>::max();

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
        current.iterateEdges([&](char c, const NFANode& neighbor) {
            if (c == 0)
                workQueue.push(neighbor);
        });
    }
    return result;
}

DFA::DFA(const NFA& nfa) {
    const NFANode& startNode(nfa.start());
    const NFANode& endNode(nfa.end());

    std::unordered_set<DFANode> completed;
    std::map<NFANodeCollection, DFANode, NFANodeCollectionComparator> map;
    std::queue<NFANodeCollection> workQueue;
    NFANodeCollection closure(epsilonClosure(startNode));
    incidence.emplace_back(std::unordered_map<char, DFANode>());
    map.emplace(std::make_pair(closure, 0));
    workQueue.emplace(std::move(closure));
    
    while (!workQueue.empty()) {
        const NFANodeCollection current(workQueue.front());
        workQueue.pop();
        const auto& i(map.find(current));
        assert(i != map.end());
        assert(i->second != invalidNode);
        DFANode node(i->second);

        // This is a Map / Reduce operation
        std::unordered_map<char, NFANodeCollection> outgoingEdges;
        for (const NFANode& nfaNode : current) {
            nfaNode.iterateEdges([&](char c, const NFANode& neighbor) {
                if (c != 0) // epsilon
                    outgoingEdges.insert(std::make_pair(c, NFANodeCollection())).first->second.insert(neighbor);
            });
        }
        for (const auto& outgoingEdge : outgoingEdges) {
            NFANodeCollection neighbor;
            for (const NFANode& node : outgoingEdge.second) {
                const NFANodeCollection& closure(epsilonClosure(node));
                neighbor.insert(closure.cbegin(), closure.cend());
            }
            auto p(map.insert(std::make_pair(neighbor, invalidNode)));
            if (p.second) {
                incidence.emplace_back(std::unordered_map<char, DFANode>());
                p.first->second = incidence.size() - 1;
            }
            DFANode dfaNeighbor = p.first->second;
            assert(dfaNeighbor != invalidNode);
            incidence[node].emplace(std::make_pair(outgoingEdge.first, dfaNeighbor));
            if (completed.find(dfaNeighbor) == completed.end())
                workQueue.push(neighbor);
            completed.emplace(dfaNeighbor);
        }
    }

    for (const auto& n : map) {
        if (n.first.find(endNode) != n.first.end())
            endNodes.insert(n.second);
    }
}

}