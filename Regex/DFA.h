//
//  DFA.h
//  Regex
//
//  Created by Litherum on 7/21/15.
//  Copyright (c) 2015 Litherum. All rights reserved.
//

#ifndef __Regex__DFA__
#define __Regex__DFA__

#include <cassert>
#include <limits>
#include <map>
#include <memory>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <vector>

class NFANode;

typedef std::size_t DFANode;

class DFA {
public:
    static const DFANode invalidNode;

    DFA(const NFANode& startNode, const NFANode& endNode);

    void addEdge(DFANode source, char c, DFANode destination) {
        assert(source < incidence.size());
        incidence[source].insert(std::make_pair(c, destination));
    }

    DFANode follow(DFANode source, char c) const {
        assert(source < incidence.size());
        const auto& m(incidence[source]);
        const auto& iter(m.find(c));
        if (iter == m.end())
            return invalidNode;
        return iter->second;
    }

    std::size_t size() const {
        return incidence.size();
    }

    bool isEndNode(DFANode node) const {
        return endNodes.find(node) != endNodes.end();
    }
    
    template<typename T>
    void iterateNodes(T callback) const {
        for (DFANode i(0); i < incidence.size(); ++i)
            callback(i);
    }
    
    template<typename T>
    void iterateEdges(DFANode source, T callback) const {
        assert(source < incidence.size());
        for (const auto& iter : incidence[source])
            callback(iter.first, iter.second);
    }

private:
    // Could make the map into a std::array because chars are so small, but this is more general.
    std::vector<std::unordered_map<char, DFANode>> incidence;
    // Start at index 0
    std::unordered_set<std::size_t> endNodes;
};

#endif /* defined(__Regex__DFA__) */
