//
//  main.cpp
//  Regex
//
//  Created by Litherum on 7/11/15.
//  Copyright © 2015 Litherum. All rights reserved.
//

#include <iostream>
#include <map>
#include <memory>
#include <string>

class Node {
public:
    void addEdge(char c, Node& n) {
        edges.insert(std::make_pair(c, std::reference_wrapper<Node>(n)));
    }
    Node* follow(char c) {
        auto iter = edges.find(c);
        if (iter == edges.end())
            return nullptr;
        return &iter->second.get();
    }
private:
    std::map<char, std::reference_wrapper<Node>> edges;
};

class Machine {
public:
    Machine(Node& startNode, Node& endNode): startNode(startNode), endNode(endNode) {
    }

    virtual bool run(std::string s) = 0;

protected:
    Node& startNode;
    Node& endNode;
};

class Interpreter: public Machine {
public:
    Interpreter(Node& startNode, Node& endNode): Machine(startNode, endNode) {
    }

    virtual bool run(std::string s) override {
        Node* state = &startNode;
        for (char c : s) {
            if (Node* n = state->follow(c))
                state = n;
            else
                return false;
        }
        return state == &endNode;
    }
};

class JIT: public Machine {
    
};

std::unique_ptr<Machine> compile(Node& startNode, Node& endNode) {
    return std::unique_ptr<Interpreter>(new Interpreter(startNode, endNode));
}

int main(int argc, const char * argv[]) {
    Node startNode;
    Node endNode;
    startNode.addEdge('1', startNode);
    startNode.addEdge('0', endNode);
    std::unique_ptr<Machine> m = compile(startNode, endNode);
    std::cout << m->run("11110") << std::endl;
    return 0;
}
