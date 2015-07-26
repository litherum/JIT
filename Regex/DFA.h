//
//  DFA.h
//  Regex
//
//  Created by Litherum on 7/21/15.
//  Copyright (c) 2015 Litherum. All rights reserved.
//

#ifndef __Regex__DFA__
#define __Regex__DFA__

#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace Regex {

class NFA;

typedef std::size_t DFANode;

class DFA {
public:
    static const DFANode invalidNode;

    DFA(const NFA&);
    void addEdge(DFANode source, char c, DFANode destination);
    DFANode follow(DFANode source, char c) const;
    std::size_t size() const;
    bool isEndNode(DFANode node) const;
    // Autoboxing to std::function is worse than templates, but this file is API and I don't want
    // to expose implementation
    void iterateNodes(std::function<void(DFANode)> callback) const;
    void iterateEdges(DFANode source, std::function<void(char, DFANode)> callback) const;

private:
    // Could make the map into a std::array because chars are so small, but this is more general.
    std::vector<std::unordered_map<char, DFANode>> incidence;
    // Start at index 0
    std::unordered_set<std::size_t> endNodes;
};

}

#endif /* defined(__Regex__DFA__) */
