//
//  NFA.h
//  Regex
//
//  Created by Litherum on 7/21/15.
//  Copyright (c) 2015 Litherum. All rights reserved.
//

#ifndef __Regex__NFA__
#define __Regex__NFA__

#include <set>
#include <vector>

namespace Regex {

class NFANode {
public:
    void addEdge(char c, NFANode& n) {
        edges.emplace(std::make_pair(c, std::reference_wrapper<NFANode>(n)));
    }

    template<typename T>
    void iterateEdges(T callback) const {
        for (const auto& iter : edges)
            callback(iter.first, iter.second);
    }

private:
    typedef std::pair<char, std::reference_wrapper<const NFANode>> Edge;

    class EdgesComparator {
    public:
        bool operator() (const Edge& e1, const Edge& e2) const {
            return &e1 < &e2;
        }
    };

    std::set<Edge, EdgesComparator> edges;
};

class NFANodeComparator {
public:
    bool operator() (const NFANode& n1, const NFANode& n2) const {
        return &n1 < &n2;
    }
};

typedef std::set<std::reference_wrapper<const NFANode>, NFANodeComparator> NFANodeCollection;

class NFANodeCollectionComparator {
public:
    bool operator() (const NFANodeCollection& nc1, const NFANodeCollection& nc2) const;
};

class NFA {
public:
    NFA(char c);

    const NFANode& start() const {
        return *startNode;
    }

    const NFANode& end() const {
        return *endNode;
    }

    void takeNodes(NFA&& nfa) {
        std::move(nfa.nodes.begin(), nfa.nodes.end(), std::back_inserter(nodes));
        nfa.nodes.clear();
    }

    void concatenate(NFA&& nfa);
    void star();
    void plus();
    void alternate(NFA&& nfa);

private:
    NFANode* startNode;
    NFANode* endNode;
    // Having lots of tiny allocations sucks, but it seems better than copying all the nodes every time you mutate an NFA
    std::vector<std::unique_ptr<NFANode>> nodes;
};

}

#endif /* defined(__Regex__NFA__) */
