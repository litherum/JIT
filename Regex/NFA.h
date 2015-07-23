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
    bool operator() (const NFANodeCollection& nc1, const NFANodeCollection& nc2) const {
        // Compare lexicographically
        for (auto i1(nc1.begin()), i2(nc2.begin()); i1 != nc1.end() && i2 != nc2.end(); ++i1, ++i2) {
            if (&i1->get() < &i2->get())
                return true;
            if (&i1->get() > &i2->get())
                return false;
        }
        return nc1.size() < nc2.size();
    }
};

class NFA {
public:
    NFA(char c) {
        std::unique_ptr<NFANode> start(new NFANode());
        std::unique_ptr<NFANode> end(new NFANode());
        start->addEdge(c, *end.get());
        startNode = start.get();
        endNode = end.get();
        nodes.emplace_back(std::move(start));
        nodes.emplace_back(std::move(end));
    }

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

    void concatenate(NFA&& nfa) {
        endNode->addEdge(0, *nfa.startNode);
        endNode = nfa.endNode;
        takeNodes(std::move(nfa));
    }

    void star() {
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

    void alternate(NFA&& nfa) {
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

private:
    NFANode* startNode;
    NFANode* endNode;
    // Having lots of tiny allocations sucks, but it seems better than copying all the nodes every time you mutate an NFA
    std::vector<std::unique_ptr<NFANode>> nodes;
};

#endif /* defined(__Regex__NFA__) */
