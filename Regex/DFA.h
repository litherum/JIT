//
//  DFA.h
//  Regex
//
//  Created by Litherum on 7/21/15.
//  Copyright (c) 2015 Litherum. All rights reserved.
//

#ifndef __Regex__DFA__
#define __Regex__DFA__

#include <map>
#include <memory>
#include <set>
#include <vector>

class DFANode {
public:
    void addEdge(char c, DFANode& n) {
        edges.emplace(std::make_pair(c, std::reference_wrapper<DFANode>(n)));
    }
    
    const DFANode* follow(char c) const {
        auto iter = edges.find(c);
        if (iter == edges.end())
            return nullptr;
        return &iter->second.get();
    }
    
    template<typename T>
    void iterateEdges(T callback) const {
        for (const auto& iter : edges)
            callback(iter.first, iter.second.get());
    }
    
private:
    std::map<char, std::reference_wrapper<const DFANode>> edges;
};

class DFANodeComparator {
public:
    bool operator() (const DFANode& n1, const DFANode& n2) const {
        return &n1 < &n2;
    }
};

typedef std::set<std::reference_wrapper<const DFANode>, DFANodeComparator> DFANodeReferenceCollection;
typedef std::vector<DFANode> DFANodeCollection;

class NFANode;
class DFA {
public:
    DFA(const NFANode& startNode, const NFANode& endNode);

    const DFANodeCollection& getNodes() const {
        return nodes;
    }

    const DFANode& getStartNode() const {
        return *startNode;
    }

    const DFANodeReferenceCollection& getEndNodes() const {
        return endNodes;
    }

private:
    DFANodeCollection nodes;
    const DFANode* startNode;
    DFANodeReferenceCollection endNodes;
};

#endif /* defined(__Regex__DFA__) */
