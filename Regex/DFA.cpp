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

static void printNFANodeCollection(const NFANodeCollection& collection) {
    for (const NFANode& node : collection)
        std::cout << &node << " ";
    std::cout << std::endl;
}

DFA::DFA(const NFANode& startNode, const NFANode& endNode): startNode(nullptr) {
    const std::size_t initialIndex(std::numeric_limits<std::size_t>::max());
    std::set<NFANodeCollection, NFANodeCollectionComparator> completed;
    std::map<NFANodeCollection, std::size_t, NFANodeCollectionComparator> map;
    std::map<std::size_t, std::map<char, std::size_t>> incidence;
    std::queue<const NFANodeCollection> workQueue;
    NFANodeCollection closure(epsilonClosure(startNode));
    nodes.emplace_back();
    incidence.emplace(std::make_pair(std::size_t(0), std::map<char, std::size_t>()));
    map.emplace(std::make_pair(closure, 0));
    workQueue.emplace(std::move(closure));
    
    while (!workQueue.empty()) {
        const NFANodeCollection current(workQueue.front());
        printNFANodeCollection(current);
        workQueue.pop();
        const auto& i(map.find(current));
        assert(i != map.end());
        assert(i->second != initialIndex);
        std::cout << " ==> " << i->second << std::endl;
        std::size_t dfaNodeIndex(i->second);
        // This is a Map / Reduce operation
        std::map<char, NFANodeCollection> outgoingEdges;
        for (const NFANode& nfaNode : current) {
            nfaNode.iterateEdges([&](const std::unique_ptr<char>& c, const NFANode& neighbor) {
                if (c != nullptr) // epsilon
                    outgoingEdges.insert(std::make_pair(*c, NFANodeCollection())).first->second.insert(neighbor);
            });
        }
        for (const auto& outgoingEdge : outgoingEdges) {
            NFANodeCollection neighbor;
            for (const NFANode& node : outgoingEdge.second) {
                const NFANodeCollection& closure(epsilonClosure(node));
                neighbor.insert(closure.cbegin(), closure.cend());
            }
            auto p(map.insert(std::make_pair(neighbor, initialIndex)));
            if (p.second) {
                nodes.emplace_back();
                incidence.emplace(std::make_pair(nodes.size() - 1, std::map<char, std::size_t>()));
                p.first->second = nodes.size() - 1;
            }
            assert(p.first->second != initialIndex);
            incidence[dfaNodeIndex].emplace(std::make_pair(outgoingEdge.first, p.first->second));
            //nodes[dfaNodeIndex].addEdge(outgoingEdge.first, nodes[p.first->second]);
            if (completed.find(neighbor) == completed.end())
                workQueue.push(neighbor);
            completed.emplace(std::move(neighbor));
        }
    }

    for (const auto& i : incidence) {
        for (const auto& j : i.second) {
            nodes[i.first].addEdge(j.first, nodes[j.second]);
        }
    }
    
    this->startNode = &nodes[0];
    for (const auto& n : map) {
        if (n.first.find(endNode) != n.first.end())
            endNodes.insert(nodes[n.second]);
    }
}